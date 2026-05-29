#include "Storage/IridiumSEXP.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumMeta.h"
#include "Generated/IridiumTypes.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Support/IRIS.hpp"
#include <iomanip>
#include <stdexcept>

namespace IRI_STORAGE {

void append_escaped_json_string(std::ostream &oss, std::string_view s) {
  oss << '"';
  for (size_t i = 0; i < s.size();) {
    size_t start = i;
    uint8_t b1 = static_cast<uint8_t>(s[i]);
    uint32_t cp = 0;
    size_t len = 0;

    // Decode UTF-8 (simple validation of continuation bytes)
    if (b1 < 0x80) {
      cp = b1;
      len = 1;
    } else if ((b1 >> 5) == 0x6) { // 2-byte
      if (i + 1 < s.size()) {
        uint8_t b2 = static_cast<uint8_t>(s[i + 1]);
        if ((b2 & 0xC0) == 0x80) {
          cp = ((b1 & 0x1F) << 6) | (b2 & 0x3F);
          len = 2;
        }
      }
    } else if ((b1 >> 4) == 0xE) { // 3-byte
      if (i + 2 < s.size()) {
        uint8_t b2 = static_cast<uint8_t>(s[i + 1]);
        uint8_t b3 = static_cast<uint8_t>(s[i + 2]);
        if ((b2 & 0xC0) == 0x80 && (b3 & 0xC0) == 0x80) {
          cp = ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
          len = 3;
        }
      }
    } else if ((b1 >> 3) == 0x1E) { // 4-byte
      if (i + 3 < s.size()) {
        uint8_t b2 = static_cast<uint8_t>(s[i + 1]);
        uint8_t b3 = static_cast<uint8_t>(s[i + 2]);
        uint8_t b4 = static_cast<uint8_t>(s[i + 3]);
        if ((b2 & 0xC0) == 0x80 && (b3 & 0xC0) == 0x80 && (b4 & 0xC0) == 0x80) {
          cp = ((b1 & 0x07) << 18) | ((b2 & 0x3F) << 12) | ((b3 & 0x3F) << 6) |
               (b4 & 0x3F);
          len = 4;
        }
      }
    }

    if (len == 0) {
      // Invalid/partial sequence -> replacement character
      oss << "\\uFFFD";
      ++i;
      continue;
    }

    i += len;

    // Escape logic
    if (cp == 0x22) { // "
      oss << "\\\"";
    } else if (cp == 0x5C) { // backslash
      oss << "\\\\";
    } else if (cp == 0x0A) {
      oss << "\\n";
    } else if (cp == 0x0D) {
      oss << "\\r";
    } else if (cp == 0x09) {
      oss << "\\t";
    } else if (cp == 0x08) {
      oss << "\\b";
    } else if (cp == 0x0C) {
      oss << "\\f";
    } else if (cp < 0x20 || cp == 0x2028 || cp == 0x2029) {
      // Control or LS/PS — emit \uXXXX
      char buf[8];
      snprintf(buf, sizeof(buf), "%04x", static_cast<unsigned>(cp));
      oss << "\\u" << buf;
    } else if (cp <= 0x7F) {
      // ASCII printable
      oss << static_cast<char>(cp);
    } else if (cp <= 0xFFFF) {
      // Non-ASCII BMP: preserve original UTF-8 bytes
      oss.write(s.data() + start, static_cast<std::streamsize>(len));
    } else if (cp <= 0x10FFFF) {
      // Above BMP — emit surrogate pair as \uXXXX\uXXXX
      uint32_t U = cp - 0x10000;
      uint16_t hi = static_cast<uint16_t>(0xD800 + (U >> 10));
      uint16_t lo = static_cast<uint16_t>(0xDC00 + (U & 0x3FF));
      char buf[8];
      snprintf(buf, sizeof(buf), "%04x", static_cast<unsigned>(hi));
      oss << "\\u" << buf;
      snprintf(buf, sizeof(buf), "%04x", static_cast<unsigned>(lo));
      oss << "\\u" << buf;
    } else {
      // Shouldn't happen, but safe fallback
      oss << "\\uFFFD";
    }
  }
  oss << '"';
}

// --- Implementations for IridiumSEXP ---

void IridiumSEXP::dump(std::ostream &oss, IridiumPool *pool,
                       bool compressed, int indent) const {
  std::string pad = compressed ? "" : std::string(indent, ' ');
  std::string pad1 = compressed ? "" : std::string(indent + 2, ' ');
  std::string nl = compressed ? "" : "\n";

  oss << pad << "[" << nl;
  oss << pad1 << "\"" << IRI_GEN::dump_tag(tag == IRI_GEN::ScriptBinding ? GlobalBinding : tag) << "\"," << nl;

  // Serialize args
  if (num_args == 0) {
    oss << pad1 << "[]," << nl;
  } else {
    oss << pad1 << "[" << nl;
    auto args_span = pool->get_args_view(this);

    for (uint32_t i = 0; i < num_args; ++i) {
      IRID child_id = args_span[i];

      pool->operator[](child_id).dump(oss, pool, compressed, indent + 4);

      if (i + 1 < num_args)
        oss << "," << nl;
    }
    oss << nl << pad1 << "]," << nl;
  }

  // Serialize flags
  oss << pad1 << "[";
  bool first_flag = true;
  auto flags_span = pool->get_flags(this);

  for (uint32_t i = 0; i < num_flag_slots; ++i) {
    const auto &v = flags_span[i];

    IRI_GEN::IRI_FLAG flagEnum = IRI_GEN::IridiumMeta::get_flag_enum(tag, i);
    if (flagEnum == IRI_GEN::IRI_FLAG::LINK || std::holds_alternative<std::monostate>(v))
      continue;

    if (!first_flag)
      oss << ",";
    first_flag = false;

    oss << "[\"" << IRI_GEN::dump_flag(flagEnum) << "\",";

    if (std::holds_alternative<double>(v)) {
      oss << std::setprecision(17) << std::get<double>(v);
    } else if (std::holds_alternative<bool>(v)) {
      oss << std::boolalpha << std::get<bool>(v);
    } else if (std::holds_alternative<StringID>(v)) {
      // Unpack the StringID using the pool
      std::string_view str = pool->strings.get(std::get<StringID>(v));
      append_escaped_json_string(oss, str);
    } else if (std::holds_alternative<std::nullptr_t>(v)) {
      oss << "null";
    }

    oss << "]";
  }
  oss << "]" << nl << pad << "]";
}

void IridiumSEXP::dumpFlat(std::ostream &oss, IridiumPool *pool,
                           int depth, bool full) const {
  // 1. Setup Indentation
  std::string indent(depth, ' ');

  // if (tag == IRI_GEN::EnvBinding) {
  //   auto flags_span = pool->get_flags(this);
  //   const auto &v = flags_span[IRI_GEN::EnvBindingSEXP::FLAG_IDX_LINK];
  //   if (!std::holds_alternative<double>(v)) {
  //     throw std::runtime_error("Expected LINK to be a double");
  //   }
  //   double link = std::get<double>(v);
  //   oss << indent;
  //   pool->iris->getBindingMetaFromLINK(link).dump(*pool, oss, full);
  //   oss << "\n";

  //   return;
  // }

  // if (tag == IRI_GEN::RemoteEnvBinding) {
  //   auto flags_span = pool->get_flags(this);
  //   const auto &v = flags_span[IRI_GEN::RemoteEnvBindingSEXP::FLAG_IDX_LINK];
  //   if (!std::holds_alternative<double>(v)) {
  //     throw std::runtime_error("Expected LINK to be a double");
  //   }
  //   double link = std::get<double>(v);
  //   oss << indent;
  //   pool->iris->getBindingMetaFromLINK(link).dump(*pool, oss, full);
  //   oss << "\n";

  //   return;
  // }

  // 2. Resolve Tag Name
  oss << indent << IRI_GEN::dump_tag(tag);

  // 3. Handle Flags
  bool first_flag = true;
  auto flags_span = pool->get_flags(this);

  for (uint32_t i = 0; i < num_flag_slots; ++i) {
    const auto &v = flags_span[i];


    IRI_GEN::IRI_FLAG flagEnum = IRI_GEN::IridiumMeta::get_flag_enum(tag, i);
    if (flagEnum == IRI_GEN::IRI_FLAG::LINK || std::holds_alternative<std::monostate>(v))
      continue;

    if (first_flag) {
      oss << " [";
      first_flag = false;
    } else {
      oss << ", ";
    }

    // Stream the variant value
    if (std::holds_alternative<double>(v)) {
      oss << IRI_GEN::dump_flag(flagEnum) << ": ";
      oss << std::setprecision(17) << std::get<double>(v);
    } else if (std::holds_alternative<bool>(v)) {
      oss << IRI_GEN::dump_flag(flagEnum) << ": ";
      oss << std::boolalpha << std::get<bool>(v);
    } else if (std::holds_alternative<StringID>(v)) {
      oss << IRI_GEN::dump_flag(flagEnum) << ": ";
      std::string_view str = pool->strings.get(std::get<StringID>(v));
      append_escaped_json_string(oss, str);
    } else if (std::holds_alternative<std::nullptr_t>(v)) {
      oss << IRI_GEN::dump_flag(flagEnum);
    }
  }

  if (!first_flag) {
    oss << "]";
  }

  oss << "\n";

  // 4. Handle Args (Children)
  auto args_span = pool->get_args_view(this);
  for (uint32_t i = 0; i < num_args; ++i) {
    IRID child_id = args_span[i];
    pool->operator[](child_id).dumpFlat(oss, pool, depth + 2, tag == IRI_GEN::BB ? false : full);
  }
}

} // namespace IRI_STORAGE
