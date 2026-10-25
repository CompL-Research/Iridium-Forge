// pta_trace.hpp — header-only writer for the PTA Trace Format (PTF) v1.
//
//   Spec: docs/trace-format.md
//   C++17. No required third-party dependencies.
//
// Optional: define PTF_WITH_ZLIB and link zlib to gzip-compress state blobs.
// Typically 8-15x on DOT payloads; strongly recommended for real traces.
//
// Usage
// -----
//   #include "pta_trace.hpp"
//
//   ptf::Options opt;
//   opt.path           = "out.pta";
//   opt.producer_name  = "pta-pass";
//   opt.producer_version = "0.4.1";
//   opt.module_name    = "foo.bc";
//   ptf::TraceWriter T(opt);
//
//   // 1. Declare program structure (may also be done after analysis).
//   auto &c = T.declareClosure("c0", "main").kind("function").source("a.c", 10);
//   auto &bb = c.block("c0/bb0", "entry");
//   bb.inst("c0/bb0/i0", "%1 = alloca i32").op("alloca").attr("site", "h1");
//   bb.succ("c0/bb1");
//   T.writeClosures();
//
//   // 2. Trace evaluation. All scopes are RAII; exit events are automatic.
//   {
//     auto cs = T.enterClosure("c0", "ctx0", "entry");
//     auto bs = T.enterBlock("c0/bb0", /*iter=*/0);
//     {
//       auto e = T.eval("c0/bb0/i0");
//       e.state(dumpStateAsDot());        // hashed + deduped + packed
//       e.changed(true);
//     }                                   // <- event line written here
//   }
//   T.finish();
//
// Nested evaluation needs no special handling: if any event is emitted while an
// eval scope is open, that eval is automatically written with "pending":true and
// closed with an "eval_done" when its scope ends (spec section 6.4).

#ifndef PTA_TRACE_HPP_
#define PTA_TRACE_HPP_

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef PTF_WITH_ZLIB
#include <zlib.h>
#endif

namespace ptf {

// ---------------------------------------------------------------------------
// SHA-256 (public-domain style compact implementation).
// Only used to content-address state blobs; readers never verify it.
// ---------------------------------------------------------------------------
namespace detail {

class Sha256 {
 public:
  Sha256() { reset(); }

  void reset() {
    len_ = 0;
    buflen_ = 0;
    static const uint32_t kInit[8] = {0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u,
                                      0xa54ff53au, 0x510e527fu, 0x9b05688cu,
                                      0x1f83d9abu, 0x5be0cd19u};
    std::memcpy(h_, kInit, sizeof(h_));
  }

  void update(const void *data, size_t n) {
    const uint8_t *p = static_cast<const uint8_t *>(data);
    len_ += n;
    while (n > 0) {
      size_t take = 64 - buflen_;
      if (take > n) take = n;
      std::memcpy(buf_ + buflen_, p, take);
      buflen_ += take;
      p += take;
      n -= take;
      if (buflen_ == 64) {
        block(buf_);
        buflen_ = 0;
      }
    }
  }

  void update(const std::string &s) { update(s.data(), s.size()); }

  // Returns the digest as lowercase hex, truncated to `bytes` bytes.
  std::string hex(size_t bytes = 32) {
    uint64_t bitlen = len_ * 8;
    uint8_t pad = 0x80;
    update(&pad, 1);
    uint8_t zero = 0;
    while (buflen_ != 56) update(&zero, 1);
    uint8_t lenbe[8];
    for (int i = 0; i < 8; ++i) lenbe[i] = uint8_t(bitlen >> (56 - 8 * i));
    // Bypass update() so the length field is not counted into len_.
    std::memcpy(buf_ + buflen_, lenbe, 8);
    block(buf_);

    static const char *kHex = "0123456789abcdef";
    std::string out;
    if (bytes > 32) bytes = 32;
    out.reserve(bytes * 2);
    for (size_t i = 0; i < bytes; ++i) {
      uint8_t b = uint8_t(h_[i / 4] >> (24 - 8 * (i % 4)));
      out.push_back(kHex[b >> 4]);
      out.push_back(kHex[b & 0xf]);
    }
    return out;
  }

 private:
  static uint32_t ror(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }

  void block(const uint8_t *p) {
    static const uint32_t K[64] = {
        0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu,
        0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u,
        0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u,
        0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
        0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u,
        0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
        0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
        0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
        0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u,
        0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u, 0x1e376c08u,
        0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu,
        0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
        0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};
    uint32_t w[64];
    for (int i = 0; i < 16; ++i)
      w[i] = (uint32_t(p[4 * i]) << 24) | (uint32_t(p[4 * i + 1]) << 16) |
             (uint32_t(p[4 * i + 2]) << 8) | uint32_t(p[4 * i + 3]);
    for (int i = 16; i < 64; ++i) {
      uint32_t s0 = ror(w[i - 15], 7) ^ ror(w[i - 15], 18) ^ (w[i - 15] >> 3);
      uint32_t s1 = ror(w[i - 2], 17) ^ ror(w[i - 2], 19) ^ (w[i - 2] >> 10);
      w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    uint32_t a = h_[0], b = h_[1], c = h_[2], d = h_[3];
    uint32_t e = h_[4], f = h_[5], g = h_[6], hh = h_[7];
    for (int i = 0; i < 64; ++i) {
      uint32_t S1 = ror(e, 6) ^ ror(e, 11) ^ ror(e, 25);
      uint32_t ch = (e & f) ^ (~e & g);
      uint32_t t1 = hh + S1 + ch + K[i] + w[i];
      uint32_t S0 = ror(a, 2) ^ ror(a, 13) ^ ror(a, 22);
      uint32_t mj = (a & b) ^ (a & c) ^ (b & c);
      uint32_t t2 = S0 + mj;
      hh = g; g = f; f = e; e = d + t1;
      d = c; c = b; b = a; a = t1 + t2;
    }
    h_[0] += a; h_[1] += b; h_[2] += c; h_[3] += d;
    h_[4] += e; h_[5] += f; h_[6] += g; h_[7] += hh;
  }

  uint32_t h_[8];
  uint8_t buf_[64];
  size_t buflen_ = 0;
  uint64_t len_ = 0;
};

inline std::string jsonEscape(const std::string &s) {
  std::string o;
  o.reserve(s.size() + 8);
  for (unsigned char c : s) {
    switch (c) {
      case '"':  o += "\\\""; break;
      case '\\': o += "\\\\"; break;
      case '\n': o += "\\n";  break;
      case '\r': o += "\\r";  break;
      case '\t': o += "\\t";  break;
      case '\b': o += "\\b";  break;
      case '\f': o += "\\f";  break;
      default:
        if (c < 0x20) {
          char b[8];
          std::snprintf(b, sizeof(b), "\\u%04x", c);
          o += b;
        } else {
          o.push_back(char(c));
        }
    }
  }
  return o;
}

inline std::string jstr(const std::string &s) {
  return "\"" + jsonEscape(s) + "\"";
}

#ifdef PTF_WITH_ZLIB
inline bool gzipCompress(const std::string &in, std::string &out) {
  z_stream zs{};
  if (deflateInit2(&zs, Z_BEST_SPEED, Z_DEFLATED, 15 + 16, 8,
                   Z_DEFAULT_STRATEGY) != Z_OK)
    return false;
  out.resize(deflateBound(&zs, uLong(in.size())));
  zs.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(in.data()));
  zs.avail_in = uInt(in.size());
  zs.next_out = reinterpret_cast<Bytef *>(&out[0]);
  zs.avail_out = uInt(out.size());
  int rc = deflate(&zs, Z_FINISH);
  size_t produced = out.size() - zs.avail_out;
  deflateEnd(&zs);
  if (rc != Z_STREAM_END) return false;
  out.resize(produced);
  return true;
}
#endif

}  // namespace detail

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

enum class Detail {
  Blocks,       // closure/block enter+exit only
  Insts,        // + eval events, no state payloads
  InstsStates,  // + state payloads (full trace)
};

inline const char *detailName(Detail d) {
  switch (d) {
    case Detail::Blocks: return "blocks";
    case Detail::Insts:  return "insts";
    default:             return "insts+states";
  }
}

struct Options {
  std::string path;                     // trace directory, e.g. "out.pta"
  std::string producer_name = "unknown";
  std::string producer_version;
  std::string producer_commit;
  std::string module_name;
  std::string source_root;
  std::string triple;
  std::string title;
  Detail detail = Detail::InstsStates;
  bool compress = true;                 // gzip blobs (needs PTF_WITH_ZLIB)
  bool timings = true;                  // record dur_us on eval scopes
  size_t pack_roll_bytes = 512ull << 20;
  size_t flush_every = 2048;            // events between fsync-ish flushes
  size_t max_state_bytes = 0;           // 0 = unlimited
};

// ---------------------------------------------------------------------------
// Program-structure builders (produce closures.json)
// ---------------------------------------------------------------------------

class InstBuilder {
 public:
  InstBuilder &op(const std::string &v) { op_ = v; return *this; }
  InstBuilder &source(const std::string &f, int line = 0, int col = 0) {
    sfile_ = f; sline_ = line; scol_ = col; return *this;
  }
  InstBuilder &def(const std::string &v) { defs_.push_back(v); return *this; }
  InstBuilder &use(const std::string &v) { uses_.push_back(v); return *this; }
  InstBuilder &attr(const std::string &k, const std::string &v) {
    attrs_[k] = v; return *this;
  }

 private:
  friend class BlockBuilder;
  friend class TraceWriter;
  std::string id_, text_, op_, sfile_;
  int sline_ = 0, scol_ = 0;
  std::vector<std::string> defs_, uses_;
  std::map<std::string, std::string> attrs_;
};

class BlockBuilder {
 public:
  BlockBuilder &label(const std::string &v) { label_ = v; return *this; }
  BlockBuilder &pred(const std::string &v) { preds_.push_back(v); return *this; }
  BlockBuilder &succ(const std::string &v) { succs_.push_back(v); return *this; }
  BlockBuilder &attr(const std::string &k, const std::string &v) {
    attrs_[k] = v; return *this;
  }
  InstBuilder &inst(const std::string &id, const std::string &text) {
    insts_.emplace_back();
    insts_.back().id_ = id;
    insts_.back().text_ = text;
    return insts_.back();
  }

 private:
  friend class ClosureBuilder;
  friend class TraceWriter;
  std::string id_, label_;
  std::vector<std::string> preds_, succs_;
  std::map<std::string, std::string> attrs_;
  std::vector<InstBuilder> insts_;
};

class ClosureBuilder {
 public:
  ClosureBuilder &name(const std::string &v) { name_ = v; return *this; }
  ClosureBuilder &kind(const std::string &v) { kind_ = v; return *this; }
  ClosureBuilder &parent(const std::string &v) { parent_ = v; return *this; }
  ClosureBuilder &source(const std::string &f, int line = 0, int col = 0) {
    sfile_ = f; sline_ = line; scol_ = col; return *this;
  }
  ClosureBuilder &attr(const std::string &k, const std::string &v) {
    attrs_[k] = v; return *this;
  }
  ClosureBuilder &entry(const std::string &v) { entry_ = v; return *this; }

  BlockBuilder &block(const std::string &id, const std::string &label = "") {
    blocks_.emplace_back();
    blocks_.back().id_ = id;
    blocks_.back().label_ = label;
    if (entry_.empty()) entry_ = id;
    return blocks_.back();
  }

  ClosureBuilder &edge(const std::string &from, const std::string &to,
                       const std::string &kind = "") {
    edges_.push_back({from, to, kind});
    return *this;
  }

 private:
  friend class TraceWriter;
  struct Edge { std::string from, to, kind; };
  std::string id_, name_, kind_ = "function", parent_, sfile_, entry_;
  int sline_ = 0, scol_ = 0;
  std::map<std::string, std::string> attrs_;
  std::vector<BlockBuilder> blocks_;
  std::vector<Edge> edges_;
};

class TraceWriter;

// ---------------------------------------------------------------------------
// RAII event scopes
// ---------------------------------------------------------------------------

class ClosureScope {
 public:
  ClosureScope(ClosureScope &&o) noexcept { moveFrom(o); }
  ClosureScope &operator=(ClosureScope &&o) noexcept { moveFrom(o); return *this; }
  ClosureScope(const ClosureScope &) = delete;
  ClosureScope &operator=(const ClosureScope &) = delete;
  ~ClosureScope();

  // Attach the closure's summary/exit state to the exit_closure event.
  void state(const std::string &payload, size_t nodes = 0, size_t edges = 0);

 private:
  friend class TraceWriter;
  ClosureScope(TraceWriter *w, std::string id, std::string ctx)
      : w_(w), id_(std::move(id)), ctx_(std::move(ctx)) {}
  void moveFrom(ClosureScope &o) {
    w_ = o.w_; id_ = std::move(o.id_); ctx_ = std::move(o.ctx_);
    state_ = std::move(o.state_); o.w_ = nullptr;
  }
  TraceWriter *w_ = nullptr;
  std::string id_, ctx_, state_;
};

class BlockScope {
 public:
  BlockScope(BlockScope &&o) noexcept { w_ = o.w_; id_ = std::move(o.id_); o.w_ = nullptr; }
  BlockScope &operator=(BlockScope &&o) noexcept {
    w_ = o.w_; id_ = std::move(o.id_); o.w_ = nullptr; return *this;
  }
  BlockScope(const BlockScope &) = delete;
  BlockScope &operator=(const BlockScope &) = delete;
  ~BlockScope();

 private:
  friend class TraceWriter;
  BlockScope(TraceWriter *w, std::string id) : w_(w), id_(std::move(id)) {}
  TraceWriter *w_ = nullptr;
  std::string id_;
};

class EvalScope {
 public:
  EvalScope(EvalScope &&o) noexcept { moveFrom(o); }
  EvalScope &operator=(EvalScope &&o) noexcept { moveFrom(o); return *this; }
  EvalScope(const EvalScope &) = delete;
  EvalScope &operator=(const EvalScope &) = delete;
  ~EvalScope();

  // Record the analysis state after this instruction. Hashed, deduplicated and
  // appended to the current pack. Omit entirely when the state did not change
  // (spec section 6.3, "effective state").
  //
  // `nodes`/`edges` are optional element counts. Supplying them is cheap for the
  // producer and lets a viewer draw graph-size timelines without decompressing
  // a single blob -- worth passing if you have them to hand.
  void state(const std::string &payload, size_t nodes = 0, size_t edges = 0);
  void changed(bool c) { changed_ = c; hasChanged_ = true; }

 private:
  friend class TraceWriter;
  // `slot` indexes the writer's eval stack; scopes are destroyed LIFO by RAII,
  // so the index stays valid and survives moves (a raw pointer would not).
  EvalScope(TraceWriter *w, size_t slot)
      : w_(w), slot_(slot), t0_(std::chrono::steady_clock::now()) {}
  void moveFrom(EvalScope &o) {
    w_ = o.w_; slot_ = o.slot_; hash_ = std::move(o.hash_);
    changed_ = o.changed_; hasChanged_ = o.hasChanged_; t0_ = o.t0_;
    o.w_ = nullptr;
  }
  TraceWriter *w_ = nullptr;
  size_t slot_ = 0;
  std::string hash_;
  bool changed_ = false, hasChanged_ = false;
  std::chrono::steady_clock::time_point t0_;
};

// ---------------------------------------------------------------------------
// TraceWriter
// ---------------------------------------------------------------------------

class TraceWriter {
 public:
  explicit TraceWriter(const Options &opt) : opt_(opt) {
    namespace fs = std::filesystem;
    root_ = fs::path(opt_.path);
    fs::create_directories(root_ / "states");
    events_.open(root_ / "events.jsonl", std::ios::binary | std::ios::trunc);
    sindex_.open(root_ / "states" / "index.jsonl",
                 std::ios::binary | std::ios::trunc);
    openPack(0);
    t0_ = std::chrono::steady_clock::now();
    writeManifest(false);
  }

  ~TraceWriter() { finish(); }

  TraceWriter(const TraceWriter &) = delete;
  TraceWriter &operator=(const TraceWriter &) = delete;

  // --- program structure -------------------------------------------------

  ClosureBuilder &declareClosure(const std::string &id,
                                 const std::string &name) {
    closures_.emplace_back();
    closures_.back().id_ = id;
    closures_.back().name_ = name;
    return closures_.back();
  }

  // Safe to call at any point; call again to overwrite if structure is
  // discovered incrementally.
  void writeClosures() {
    if (closures_.empty()) return;
    std::ofstream f(root_ / "closures.json", std::ios::binary | std::ios::trunc);
    f << "{\"roots\":[";
    bool first = true;
    for (auto &c : closures_) {
      if (!c.parent_.empty()) continue;
      if (!first) f << ',';
      first = false;
      f << detail::jstr(c.id_);
    }
    f << "],\"closures\":[";
    for (size_t ci = 0; ci < closures_.size(); ++ci) {
      if (ci) f << ',';
      emitClosure(f, closures_[ci]);
    }
    f << "]}\n";
    hasClosures_ = true;
    for (const auto &c : closures_)
      for (const auto &b : c.blocks_)
        for (const auto &in : b.insts_)
          if (!in.defs_.empty() || !in.uses_.empty()) hasDefUse_ = true;
  }

  // --- events ------------------------------------------------------------

  ClosureScope enterClosure(const std::string &id, const std::string &ctx = "",
                            const std::string &reason = "",
                            const std::string &caller = "") {
    materializePending();
    std::string j = "{\"i\":" + std::to_string(next_) + ",\"t\":\"enter_closure\"" +
                    ",\"closure\":" + detail::jstr(id);
    if (!ctx.empty())    j += ",\"ctx\":" + detail::jstr(ctx);
    if (!reason.empty()) j += ",\"reason\":" + detail::jstr(reason);
    if (!caller.empty()) j += ",\"caller\":" + detail::jstr(caller);
    emit(j + tsField() + "}");
    return ClosureScope(this, id, ctx);
  }

  BlockScope enterBlock(const std::string &id, int iter = -1) {
    materializePending();
    std::string j = "{\"i\":" + std::to_string(next_) + ",\"t\":\"enter_block\"" +
                    ",\"block\":" + detail::jstr(id);
    if (iter >= 0) { j += ",\"iter\":" + std::to_string(iter); hasFixpoint_ = true; }
    emit(j + tsField() + "}");
    return BlockScope(this, id);
  }

  EvalScope eval(const std::string &inst) {
    // Flush any enclosing eval as pending *before* this one opens, so the
    // pending line always precedes the events nested inside it (spec 6.4).
    materializePending();
    evalStack_.push_back({inst, opt_.detail == Detail::Blocks});
    return EvalScope(this, evalStack_.size() - 1);
  }

  void note(const std::string &msg, const std::string &level = "info",
            const std::string &inst = "") {
    materializePending();
    std::string j = "{\"i\":" + std::to_string(next_) + ",\"t\":\"note\",\"msg\":" +
                    detail::jstr(msg);
    if (level != "info")  j += ",\"level\":" + detail::jstr(level);
    if (!inst.empty())    j += ",\"inst\":" + detail::jstr(inst);
    emit(j + tsField() + "}");
    hasNotes_ = true;
  }

  void fixpoint(int round, int changed_count = -1,
                const std::string &scope = "") {
    materializePending();
    std::string j = "{\"i\":" + std::to_string(next_) +
                    ",\"t\":\"fixpoint\",\"round\":" + std::to_string(round);
    if (changed_count >= 0)
      j += ",\"changed_count\":" + std::to_string(changed_count);
    if (!scope.empty()) j += ",\"scope\":" + detail::jstr(scope);
    emit(j + tsField() + "}");
    hasFixpoint_ = true;
  }

  // Idempotent. Rewrites the manifest with final counts and complete:true.
  void finish() {
    if (finished_) return;
    finished_ = true;
    if (!hasClosures_) writeClosures();
    events_.flush();
    sindex_.flush();
    if (pack_) { std::fflush(pack_); std::fclose(pack_); pack_ = nullptr; }
    events_.close();
    sindex_.close();
    writeManifest(true);
  }

  size_t eventCount() const { return next_; }
  size_t stateCount() const { return seen_.size(); }

 private:
  friend class ClosureScope;
  friend class BlockScope;
  friend class EvalScope;

  // -- state blob storage -------------------------------------------------

  // Returns "" if the payload was not stored (detail level or size cap).
  std::string putState(const std::string &payload, size_t nodes, size_t edges) {
    if (opt_.detail != Detail::InstsStates) return "";
    if (opt_.max_state_bytes && payload.size() > opt_.max_state_bytes) return "";

    detail::Sha256 sh;
    sh.update(payload);
    std::string h = "sha256:" + sh.hex(16);
    if (seen_.count(h)) return h;   // deduplicated
    seen_.insert(h);

    const std::string *body = &payload;
    std::string comp;
    const char *codec = "none";
#ifdef PTF_WITH_ZLIB
    if (opt_.compress && detail::gzipCompress(payload, comp)) {
      body = &comp;
      codec = "gzip";
    }
#endif
    if (packOff_ + body->size() > opt_.pack_roll_bytes && packOff_ > 0)
      openPack(packNo_ + 1);

    std::fwrite(body->data(), 1, body->size(), pack_);
    size_t off = packOff_;
    packOff_ += body->size();

    sindex_ << "{\"h\":" << detail::jstr(h) << ",\"pack\":" << packNo_
            << ",\"off\":" << off << ",\"len\":" << body->size()
            << ",\"raw\":" << payload.size() << ",\"codec\":\"" << codec
            << "\",\"fmt\":\"dot\"";
    if (nodes) sindex_ << ",\"nodes\":" << nodes;
    if (edges) sindex_ << ",\"edges\":" << edges;
    sindex_ << "}\n";
    // Blob and its index line are both durable before the referencing event
    // line is written (spec section 9).
    std::fflush(pack_);
    sindex_.flush();
    return h;
  }

  void openPack(size_t no) {
    if (pack_) { std::fflush(pack_); std::fclose(pack_); }
    char name[64];
    std::snprintf(name, sizeof(name), "pack-%04zu.blob", no);
    pack_ = std::fopen((root_ / "states" / name).string().c_str(), "wb");
    packNo_ = no;
    packOff_ = 0;
  }

  // -- event emission -----------------------------------------------------

  void emit(const std::string &line) {
    events_ << line << '\n';
    ++next_;
    if (++sinceFlush_ >= opt_.flush_every) {
      events_.flush();
      sinceFlush_ = 0;
    }
  }

  // Any open eval scope that has not yet written its line becomes a pending
  // eval, because an event is about to be nested inside it.
  void materializePending() {
    for (auto &pe : evalStack_) {
      if (pe.emitted) continue;
      std::string j = "{\"i\":" + std::to_string(next_) +
                      ",\"t\":\"eval\",\"inst\":" + detail::jstr(pe.inst) +
                      ",\"pending\":true";
      emit(j + tsField() + "}");
      pe.emitted = true;
    }
  }

  std::string tsField() const {
    if (!opt_.timings) return "";
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::steady_clock::now() - t0_).count();
    return ",\"ts_us\":" + std::to_string(us);
  }

  void closeEval(size_t slot, const std::string &hash, bool hasChanged,
                 bool changedVal, std::chrono::steady_clock::time_point t0) {
    if (slot >= evalStack_.size()) return;  // defensive: non-LIFO destruction
    PendingEval pe = evalStack_[slot];
    evalStack_.resize(slot);

    if (opt_.detail == Detail::Blocks) return;

    long long dur = -1;
    if (opt_.timings)
      dur = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - t0).count();

    // `pe.emitted` means an event was nested inside this eval, so its opening
    // line already went out with pending:true and this one closes it.
    std::string j = "{\"i\":" + std::to_string(next_) + ",\"t\":\"" +
                    (pe.emitted ? "eval_done" : "eval") +
                    "\",\"inst\":" + detail::jstr(pe.inst);
    if (!hash.empty()) j += ",\"state\":" + detail::jstr(hash);
    if (dur >= 0)      j += ",\"dur_us\":" + std::to_string(dur);
    if (hasChanged)    j += std::string(",\"changed\":") + (changedVal ? "true" : "false");
    emit(j + tsField() + "}");
  }

  void closeBlock(const std::string &id) {
    materializePending();
    emit("{\"i\":" + std::to_string(next_) + ",\"t\":\"exit_block\",\"block\":" +
         detail::jstr(id) + tsField() + "}");
  }

  void closeClosure(const std::string &id, const std::string &ctx,
                    const std::string &hash) {
    materializePending();
    std::string j = "{\"i\":" + std::to_string(next_) +
                    ",\"t\":\"exit_closure\",\"closure\":" + detail::jstr(id);
    if (!ctx.empty())  j += ",\"ctx\":" + detail::jstr(ctx);
    if (!hash.empty()) j += ",\"state\":" + detail::jstr(hash);
    emit(j + tsField() + "}");
    if (!ctx.empty()) hasCtx_ = true;
  }

  // -- closures.json ------------------------------------------------------

  static void emitAttrs(std::ostream &f,
                        const std::map<std::string, std::string> &a) {
    if (a.empty()) return;
    f << ",\"attrs\":{";
    bool first = true;
    for (auto &kv : a) {
      if (!first) f << ',';
      first = false;
      f << detail::jstr(kv.first) << ':' << detail::jstr(kv.second);
    }
    f << '}';
  }

  static void emitStrArr(std::ostream &f, const char *key,
                         const std::vector<std::string> &v) {
    if (v.empty()) return;
    f << ",\"" << key << "\":[";
    for (size_t i = 0; i < v.size(); ++i) {
      if (i) f << ',';
      f << detail::jstr(v[i]);
    }
    f << ']';
  }

  static void emitSource(std::ostream &f, const std::string &file, int line,
                         int col) {
    if (file.empty()) return;
    f << ",\"source\":{\"file\":" << detail::jstr(file);
    if (line) f << ",\"line\":" << line;
    if (col)  f << ",\"col\":" << col;
    f << '}';
  }

  void emitClosure(std::ostream &f, const ClosureBuilder &c) {
    f << "{\"id\":" << detail::jstr(c.id_) << ",\"name\":" << detail::jstr(c.name_)
      << ",\"kind\":" << detail::jstr(c.kind_);
    if (c.parent_.empty()) f << ",\"parent\":null";
    else                   f << ",\"parent\":" << detail::jstr(c.parent_);
    emitSource(f, c.sfile_, c.sline_, c.scol_);
    emitAttrs(f, c.attrs_);
    if (!c.blocks_.empty()) {
      f << ",\"cfg\":{\"entry\":" << detail::jstr(c.entry_) << ",\"blocks\":[";
      for (size_t bi = 0; bi < c.blocks_.size(); ++bi) {
        const auto &b = c.blocks_[bi];
        if (bi) f << ',';
        f << "{\"id\":" << detail::jstr(b.id_);
        if (!b.label_.empty()) f << ",\"label\":" << detail::jstr(b.label_);
        emitStrArr(f, "preds", b.preds_);
        emitStrArr(f, "succs", b.succs_);
        emitAttrs(f, b.attrs_);
        f << ",\"insts\":[";
        for (size_t ii = 0; ii < b.insts_.size(); ++ii) {
          const auto &in = b.insts_[ii];
          if (ii) f << ',';
          f << "{\"id\":" << detail::jstr(in.id_) << ",\"text\":"
            << detail::jstr(in.text_);
          if (!in.op_.empty()) f << ",\"op\":" << detail::jstr(in.op_);
          emitSource(f, in.sfile_, in.sline_, in.scol_);
          emitStrArr(f, "defs", in.defs_);
          emitStrArr(f, "uses", in.uses_);
          emitAttrs(f, in.attrs_);
          f << '}';
        }
        f << "]}";
      }
      f << "],\"edges\":[";
      for (size_t ei = 0; ei < c.edges_.size(); ++ei) {
        if (ei) f << ',';
        f << "{\"from\":" << detail::jstr(c.edges_[ei].from) << ",\"to\":"
          << detail::jstr(c.edges_[ei].to);
        if (!c.edges_[ei].kind.empty())
          f << ",\"kind\":" << detail::jstr(c.edges_[ei].kind);
        f << '}';
      }
      f << "]}";
    }
    f << '}';
  }

  // -- manifest -----------------------------------------------------------

  void writeManifest(bool complete) {
    std::ofstream f(root_ / "manifest.json", std::ios::binary | std::ios::trunc);
    f << "{\"format_version\":1,\"producer\":{\"name\":"
      << detail::jstr(opt_.producer_name);
    if (!opt_.producer_version.empty())
      f << ",\"version\":" << detail::jstr(opt_.producer_version);
    if (!opt_.producer_commit.empty())
      f << ",\"commit\":" << detail::jstr(opt_.producer_commit);
    f << '}';
    if (!opt_.module_name.empty() || !opt_.source_root.empty()) {
      f << ",\"module\":{\"name\":" << detail::jstr(opt_.module_name);
      if (!opt_.source_root.empty())
        f << ",\"source_root\":" << detail::jstr(opt_.source_root);
      if (!opt_.triple.empty()) f << ",\"triple\":" << detail::jstr(opt_.triple);
      f << '}';
    }
    if (!opt_.title.empty()) f << ",\"title\":" << detail::jstr(opt_.title);
    f << ",\"state_format\":\"dot\",\"hash_algo\":\"sha256-128\""
      << ",\"detail\":\"" << detailName(opt_.detail) << '"';

    f << ",\"capabilities\":[";
    std::vector<std::string> caps;
    if (hasClosures_) { caps.push_back("closures"); caps.push_back("cfg"); caps.push_back("insts"); }
    if (hasDefUse_) caps.push_back("defuse");
    if (!seen_.empty()) caps.push_back("states");
    if (hasCtx_)        caps.push_back("contexts");
    if (opt_.timings)   caps.push_back("timings");
    if (hasNotes_)      caps.push_back("notes");
    if (hasFixpoint_)   caps.push_back("fixpoint");
    for (size_t i = 0; i < caps.size(); ++i) {
      if (i) f << ',';
      f << detail::jstr(caps[i]);
    }
    f << ']';

    if (complete) {
      f << ",\"counts\":{\"events\":" << next_ << ",\"states\":" << seen_.size()
        << ",\"closures\":" << closures_.size() << ",\"packs\":" << (packNo_ + 1)
        << '}';
    }
    f << ",\"complete\":" << (complete ? "true" : "false") << "}\n";
  }

  Options opt_;
  std::filesystem::path root_;
  std::ofstream events_, sindex_;
  std::FILE *pack_ = nullptr;
  size_t packNo_ = 0, packOff_ = 0;
  size_t next_ = 0, sinceFlush_ = 0;
  std::unordered_set<std::string> seen_;
  std::vector<ClosureBuilder> closures_;
  struct PendingEval { std::string inst; bool emitted; };
  std::vector<PendingEval> evalStack_;
  std::chrono::steady_clock::time_point t0_;
  bool finished_ = false, hasClosures_ = false, hasCtx_ = false;
  bool hasNotes_ = false, hasFixpoint_ = false, hasDefUse_ = false;
};

// --- scope implementations (need the complete TraceWriter) -----------------

inline ClosureScope::~ClosureScope() {
  if (w_) w_->closeClosure(id_, ctx_, state_);
}

inline void ClosureScope::state(const std::string &payload, size_t nodes,
                                size_t edges) {
  if (w_) state_ = w_->putState(payload, nodes, edges);
}

inline BlockScope::~BlockScope() {
  if (w_) w_->closeBlock(id_);
}

inline EvalScope::~EvalScope() {
  if (w_) w_->closeEval(slot_, hash_, hasChanged_, changed_, t0_);
}

inline void EvalScope::state(const std::string &payload, size_t nodes,
                             size_t edges) {
  if (w_) hash_ = w_->putState(payload, nodes, edges);
}

}  // namespace ptf

#endif  // PTA_TRACE_HPP_
