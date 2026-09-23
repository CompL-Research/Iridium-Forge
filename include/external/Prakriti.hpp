#ifndef PRAKRITI_COMBINED_HPP
#define PRAKRITI_COMBINED_HPP

#include <string>
#include <vector>

// Global Symbols, these appear as leaf nodes in the global environment of
// Prakriti, this takes care of auto init of such symbols.
// These symbols are paired with special Edges, when used in field reference
// context these nodes are meant to resolve to a unique symbol, for Prakriti, we
// are maintaining the convention of [[Symbol]], one may draw a correlation
// between these symbols and %Symbol.XYZ% stuff in ECMA. 
//
// ~ Meetesh
//
#define DEF_WELL_KNOWN_SYMBOLS(V)                                              \
  V(toPrimitive)                                                               \
  V(toStringTag)                                                               \
  V(hasInstance)                                                               \
  V(iterator)

#define PKR_SYMBOL_TAG(name) SYMBOL_##name##_VAL
#define PKR_SYMBOL_LABEL(name) "[[Symbol." #name "]]"

#include <cstdint>
#include <iostream>

#define GSTK_globalThis "globalThis"
#define GSTK_Infinity "Infinity"
#define GSTK_NaN "NaN"
#define GSTK_undefined "undefined"
#define GSTK_Function "Function"
#define GSTK_Boolean "Boolean"
#define GSTK_Symbol "Symbol"
#define GSTK_Error "Error"
#define GSTK_Object "Object"
#define GSTK_Array "Array"
#define GSTK_String "String"
#define GSTK_console "console"

#define DEF_NODE_EVAL(V) V(JSFILE)

#define DEF_NODE_DYNAMIC(V)                                                    \
  V(STKOBJ)                                                                    \
  V(TSTKOBJ)                                                                   \
  V(WIPSTKOBJ)                                                                 \
  V(OOBJ)                                                                      \
  V(FOX)                                                                       \
  V(FOBJ)                                                                      \
  V(ARGSOBJ)                                                                   \
  V(MARGSOBJ)                                                                  \
  V(ARRAYOBJ)                                                                  \
  V(STROBJ)                                                                  \
  V(ACT)                                                                       \
  V(AWAIT)

#define DEF_NODE_STATIC(V)                                                     \
  V(UNDEF_VAL)                                                                 \
  V(NAN_VAL)                                                                   \
  V(INF_VAL)                                                                   \
  V(NULL_VAL)                                                                  \
  V(TRUE_VAL)                                                                  \
  V(FALSE_VAL)                                                                 \
  V(STATE_VAL)                                                                 \
  V(NUMBER_VAL)                                                                \
  V(STRING_VAL)                                                                \
  V(BIGINT_VAL)

#define DEF_NODE_TYPES(V)                                                      \
  DEF_NODE_EVAL(V)                                                             \
  DEF_NODE_DYNAMIC(V)                                                          \
  DEF_NODE_STATIC(V)                                                           \
  V(UNKNOWN)

namespace Prakriti {

enum class TAG : uint8_t {
#define NODE_TYPES(name) name,
  DEF_NODE_TYPES(NODE_TYPES)
#undef NODE_TYPES
#define SYM_TAG(name) PKR_SYMBOL_TAG(name),
      DEF_WELL_KNOWN_SYMBOLS(SYM_TAG)
#undef SYM_TAG
};

inline const char *dumpPKRTagToString(TAG tag) {
  switch (tag) {
#define NODE_TYPES(name)                                                       \
  case TAG::name:                                                              \
    return #name;
    DEF_NODE_TYPES(NODE_TYPES)
#undef NODE_TYPES
#define SYM_TAG(name)                                                          \
  case TAG::PKR_SYMBOL_TAG(name):                                              \
    return "Symbol." #name;
    DEF_WELL_KNOWN_SYMBOLS(SYM_TAG)
#undef SYM_TAG
  default:
    return "<UNKNOWN_TAG>";
  }
}

inline std::ostream &operator<<(std::ostream &os, TAG tag) {
  return os << dumpPKRTagToString(tag);
}

} // namespace Prakriti

#include <algorithm>
#include <boost/bimap.hpp>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

template <typename NodeUID, typename EdgeUID> struct PackedEdge {
  NodeUID source;
  NodeUID target;
  EdgeUID label;

  bool operator==(const PackedEdge &other) const {
    return source == other.source && target == other.target &&
           label == other.label;
  }
};

namespace std {
template <typename NodeUID, typename EdgeUID>
struct hash<PackedEdge<NodeUID, EdgeUID>> {
  std::size_t operator()(const PackedEdge<NodeUID, EdgeUID> &e) const noexcept {
    if constexpr (std::is_same_v<NodeUID, uint32_t> &&
                  std::is_same_v<EdgeUID, uint32_t>) {
      // Pack source (32 bits) and target (32 bits) into a single 64-bit word
      uint64_t high = (static_cast<uint64_t>(e.source) << 32) | e.target;
      uint64_t low = e.label;

      // Fast SplitMix64-style bit mixer over the two 64-bit chunks
      uint64_t h = high ^ (low + 0x9e3779b97f4a7c15ULL);
      h = (h ^ (h >> 30)) * 0xbf58476d1ce4e5b9ULL;
      h = (h ^ (h >> 27)) * 0x94d049bb133111ebULL;
      return static_cast<std::size_t>(h ^ (h >> 31));
    } else {
      // Generic fallback for non-uint32_t types
      std::size_t h1 = std::hash<NodeUID>{}(e.source);
      std::size_t h2 = std::hash<NodeUID>{}(e.target);
      std::size_t h3 = std::hash<EdgeUID>{}(e.label);

      std::size_t seed = h1;
      seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      return seed;
    }
  }
};
} // namespace std

namespace Graph {

using Prakriti::TAG;

template <typename Derived, typename NodeUID, typename EdgeUID>
class GraphStorage {
protected:
  bool isUnreachable_ = true;

public:
  using Self = GraphStorage<Derived, NodeUID, EdgeUID>;
  using EdgeType = PackedEdge<NodeUID, EdgeUID>;
  using StateHash = std::size_t;

  explicit GraphStorage(bool isUnreachable) : isUnreachable_(isUnreachable) {}
  ~GraphStorage() = default;

  GraphStorage(const GraphStorage &) = default;
  GraphStorage(GraphStorage &&) noexcept = default;
  GraphStorage &operator=(const GraphStorage &) = default;
  GraphStorage &operator=(GraphStorage &&) noexcept = default;

  class NodeHelper {
  public:
    NodeHelper() = default;
    NodeHelper(Derived *G, NodeUID id) : G_(G), id_(id) {}
    virtual ~NodeHelper() = default;

    bool isLinked() const { return G_ != nullptr; }

    Derived *getGraph() const {
      assert(isLinked());
      return G_;
    }

    NodeUID getID() const {
      assert(isLinked());
      return id_;
    }

  private:
    Derived *G_ = nullptr;
    NodeUID id_{};
  };

  struct PJSSL_ARG {
    Derived *G;
    std::vector<NodeUID> L;
    std::vector<std::string> A;
    std::vector<std::shared_ptr<NodeHelper>> X;
  };

  struct PJSSL_RET {
    std::vector<NodeUID> L;
    std::vector<std::string> A;
  };

  using ActionClosureImpl = std::function<PJSSL_RET(PJSSL_ARG)>;
  using ActionClosure = std::shared_ptr<ActionClosureImpl>;

  bool isUnreachable() const { return isUnreachable_; }

  void mutateMergeUnion(const std::vector<Derived> &sources) {
    static_cast<Derived *>(this)->mutateMergeUnionImpl(sources);
  }

  void addNode(const NodeUID &uid, const TAG &tag) {
    static_cast<Derived *>(this)->addNodeImpl(uid, tag);
  }

  bool removeNode(const NodeUID &uid) {
    return static_cast<Derived *>(this)->removeNodeImpl(uid);
  }

  bool hasNode(const NodeUID &uid) const {
    return static_cast<const Derived *>(this)->hasNodeImpl(uid);
  }

  TAG getNodeTAG(const NodeUID &uid) const {
    return static_cast<const Derived *>(this)->getNodeTAGImpl(uid);
  }

  void setNodeTAG(const NodeUID &uid, const TAG &tag) {
    static_cast<Derived *>(this)->setNodeTAGImpl(uid, tag);
  }

  std::vector<NodeUID> getAllNodes() const {
    return static_cast<const Derived *>(this)->getAllNodesImpl();
  }

  std::vector<NodeUID> getPointees(NodeUID id, EdgeUID label) {
    auto edges = getAllOutgoingEdgesByLabel(id, label);
    std::vector<NodeUID> res;
    res.reserve(edges.size());
    std::ranges::transform(edges, std::back_inserter(res), &EdgeType::target);
    return res;
  }

  void addEdge(const EdgeType &edge) {
    static_cast<Derived *>(this)->addEdgeImpl(edge.source, edge.target,
                                              edge.label);
  }

  void addEdge(const NodeUID &source, const NodeUID &target,
               const EdgeUID &label) {
    static_cast<Derived *>(this)->addEdgeImpl(source, target, label);
  }

  bool removeEdge(const NodeUID &source, const NodeUID &target,
                  const EdgeUID &edge) {
    return static_cast<Derived *>(this)->removeEdgeImpl(source, target, edge);
  }

  bool hasEdge(const NodeUID &source, const NodeUID &target,
               const EdgeUID &label) const {
    return static_cast<const Derived *>(this)->hasEdgeImpl(source, target,
                                                           label);
  }

  std::vector<EdgeType> getAllEdges() const {
    return static_cast<const Derived *>(this)->getAllEdgesImpl();
  }

  std::vector<EdgeType> getAllOutgoingEdges(const NodeUID &uid) const {
    return static_cast<const Derived *>(this)->getAllOutgoingEdgesImpl(uid);
  }

  void removeAllOutgoingEdgesByLabel(const NodeUID &uid, const EdgeUID &label) {
    static_cast<Derived *>(this)->removeAllOutgoingEdgesByLabelImpl(uid, label);
  }

  std::vector<EdgeType> getAllOutgoingEdgesByLabel(const NodeUID &uid,
                                                   const EdgeUID &label) const {
    return static_cast<const Derived *>(this)->getAllOutgoingEdgesByLabelImpl(
        uid, label);
  }

  bool equals(const Derived &other) const {
    return static_cast<const Derived *>(this)->equalsImpl(other);
  }

  bool operator==(const Derived &other) const { return equals(other); }
  bool operator!=(const Derived &other) const { return !equals(other); }
};

} // namespace Graph

#include <stdexcept>

#define STRINGIFY_DETAIL(x) #x
#define STRINGIFY(x) STRINGIFY_DETAIL(x)

#define ASSERT(condition)                                                      \
  do {                                                                         \
    if (!(condition))                                                          \
      throw std::runtime_error("Assertion failed: " #condition                 \
                               " at line " STRINGIFY(__LINE__));               \
  } while (0)

// Invariant checks too expensive for the hot path - graph comparisons and the
// like. Off unless PKR_PARANOID is defined.
#ifdef PKR_PARANOID
#define ASSERT_SLOW(condition) ASSERT(condition)
#else
#define ASSERT_SLOW(condition) ((void)0)
#endif

#define DEF_GRAPH_CLOSURES(V)                                                  \
  V(NAC_OOBJ_GetPrototypeOf)                                                   \
  V(NAC_OOBJ_SetPrototypeOf)                                                   \
  V(NAC_OOBJ_IsExtensible)                                                     \
  V(NAC_OOBJ_PreventExtensions)                                                \
  V(NAC_OOBJ_GetOwnProperty)                                                   \
  V(NAC_OOBJ_DefineOwnProperty)                                                \
  V(NAC_OOBJ_HasProperty)                                                      \
  V(NAC_OOBJ_Get)                                                              \
  V(NAC_OOBJ_Set)                                                              \
  V(NAC_OOBJ_Delete)                                                           \
  V(NAC_OOBJ_OwnPropertyKeys)                                                  \
  V(NAC_SOBJ_Get)                                                              \
  V(NAC_SOBJ_Set)                                                              \
  V(NAC_TSOBJ_Get)                                                             \
  V(NAC_TSOBJ_Set)                                                             \
  V(NAC_WIPSTKOBJ_Get)                                                         \
  V(NAC_WIPSTKOBJ_Set)                                                         \
  V(NAC_ECMASCRIPT_Eval)                                                       \
  V(NAC_ECMAMODULE_Eval)                                                       \
  V(NAC_NODESCRIPT_Eval)                                                       \
  V(NAC_NODEMODULE_Eval)                                                       \
  V(NAC_QJSSCRIPT_Eval)                                                        \
  V(NAC_QJSMODULE_Eval)                                                        \
  V(NAC_Await_Eval)                                                            \
  V(NAC_MARGSOBJ_Get)                                                          \
  V(NAC_MARGSOBJ_Set)                                                          \
  V(NAC_MARGSOBJ_Unsupported)                                                  \
  V(NAC_ToNumber)                                                              \
  V(NAC_OrdinaryToPrimitive)                                                   \
  V(NAC_ToPrimitive)                                                           \
  V(NAC_ToString)                                                              \
  V(NAC_ToNumeric)                                                             \
  V(NAC_HandleBinop)                                                           \
  V(NAC_HandleRelop)                                                           \
  V(NAC_HandleJSBinop)                                                         \
  V(NAC_OrdinaryHasInstance)

// Global object identities. PKRGlobalState (ECMAGraph.hpp) turns each of
// these into a NodeUID field + getter + Init() reservation.
#define DEF_GLOBAL_IDENTITIES(V)                                              \
  V(GFOBJ_Object)                                                             \
  V(GFOBJ_Function)                                                           \
  V(GFOBJ_Boolean)                                                            \
  V(GFOBJ_Symbol)                                                             \
  V(GFOBJ_Error)                                                              \
  V(GFOBJ_Array)                                                              \
  V(GFOBJ_String)                                                              \
  V(GFOBJ_Function_prototype)                                                 \
  V(GOOBJ_Object_prototype)                                                   \
  V(GOOBJ_Boolean_prototype)                                                  \
  V(GOOBJ_Symbol_prototype)                                                   \
  V(GOOBJ_Error_prototype)                                                    \
  V(GOOBJ_Array_prototype)                                                    \
  V(GOOBJ_String_prototype)                                                    \
  V(GOOBJ_console)

#include <algorithm>
#include <cstddef>
#include <functional>
#include <immer/map.hpp>
#include <immer/map_transient.hpp>
#include <immer/set.hpp>
#include <immer/set_transient.hpp>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <optional>
#include <sched.h>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace Prakriti {

using NodeUID = uint32_t;
using EdgeUID = uint32_t;

// Splitmix64 implementation for hashing the nodes
inline std::size_t mix_hash(std::size_t h) {
  h ^= h >> 30;
  h *= 0xbf58476d1ce4e5b9ULL;
  h ^= h >> 27;
  h *= 0x94d049bb133111ebULL;
  h ^= h >> 31;
  return h;
}

class ECMAGraph : public Graph::GraphStorage<ECMAGraph, NodeUID, EdgeUID> {
public:
  using Base = Graph::GraphStorage<ECMAGraph, NodeUID, EdgeUID>;
  using Self = ECMAGraph;
  using EdgeType = Base::EdgeType;

  using NodeMap = immer::map<NodeUID, TAG>;
  using EdgeSet = immer::set<EdgeType>;

private:
  NodeMap nodes_;
  EdgeSet edges_;

public:
  explicit ECMAGraph(bool isUnreachable = false) : Base(isUnreachable) {}

  ECMAGraph(const ECMAGraph &other) = default;
  ECMAGraph(ECMAGraph &&other) noexcept = default;
  ECMAGraph &operator=(const ECMAGraph &other) = default;
  ECMAGraph &operator=(ECMAGraph &&other) noexcept = default;
  ~ECMAGraph() = default;

  ECMAGraph clone() const {
    return *this; // Invokes the O(1) immer copy constructor
  }

  static ECMAGraph bottom() { return ECMAGraph(/*isUnreachable=*/true); }

  ECMAGraph joinWith(const ECMAGraph &other) const {
    if (this->isUnreachable_)
      return other;
    if (other.isUnreachable_)
      return *this;

    ECMAGraph joined = *this;
    std::vector<ECMAGraph> sources = {other};
    joined.mutateMergeUnionImpl(sources);
    return joined;
  }

  void mutateMergeUnionImpl(const std::vector<ECMAGraph> &sources) {
    auto nodeTransient = nodes_.transient();
    auto edgeTransient = edges_.transient();

    for (const auto &src : sources) {
      for (const auto &[uid, tag] : src.nodes_) {
        nodeTransient.insert({uid, tag});
      }
      for (const auto &edge : src.edges_) {
        edgeTransient.insert(edge);
      }
      if (src.isUnreachable_) {
        this->isUnreachable_ = true;
      }
    }

    nodes_ = nodeTransient.persistent();
    edges_ = edgeTransient.persistent();
  }

  void pruneUnreachable(const std::vector<NodeUID> &roots) {
    std::unordered_set<NodeUID> reachable;
    std::vector<NodeUID> worklist(roots.begin(), roots.end());

    while (!worklist.empty()) {
      NodeUID cur = worklist.back();
      worklist.pop_back();
      if (reachable.contains(cur) || !hasNodeImpl(cur))
        continue;
      reachable.insert(cur);
      for (const auto &e : getAllOutgoingEdgesImpl(cur)) {
        if (!reachable.contains(e.target)) {
          worklist.push_back(e.target);
        }
      }
    }

    std::vector<NodeUID> toRemove;
    for (const auto &[uid, tag] : nodes_) {
      if (!reachable.contains(uid)) {
        toRemove.push_back(uid);
      }
    }
    for (const auto &uid : toRemove) {
      removeNodeImpl(uid);
    }
  }

  void addNodeImpl(const NodeUID &uid, const TAG &tag) {
    nodes_ = nodes_.set(uid, tag);
  }

  bool removeNodeImpl(const NodeUID &uid) {
    if (!nodes_.find(uid))
      return false;

    nodes_ = nodes_.erase(uid);

    auto edgeTransient = edges_.transient();
    for (const auto &e : edges_) {
      if (e.source == uid || e.target == uid) {
        edgeTransient.erase(e);
      }
    }
    edges_ = edgeTransient.persistent();
    return true;
  }

  bool hasNodeImpl(const NodeUID &uid) const {
    return nodes_.find(uid) != nullptr;
  }

  TAG getNodeTAGImpl(const NodeUID &uid) const {
    if (auto tagPtr = nodes_.find(uid)) {
      return *tagPtr;
    }
    throw std::runtime_error(
        "Expected a tag to be associated with each node!!");
  }

  void setNodeTAGImpl(const NodeUID &uid, const TAG &tag) {
    nodes_ = nodes_.set(uid, tag);
  }

  std::vector<NodeUID> getAllNodesImpl() const {
    std::vector<NodeUID> res;
    res.reserve(nodes_.size());
    for (const auto &[uid, _] : nodes_) {
      res.push_back(uid);
    }
    return res;
  }

  void addEdgeImpl(const NodeUID &source, const NodeUID &target,
                   const EdgeUID &label) {
    ASSERT(hasNode(source) && hasNode(target));
    EdgeType edge{source, target, label};
    edges_ = edges_.insert(edge);
  }

  bool removeEdgeImpl(const NodeUID &source, const NodeUID &target,
                      const EdgeUID &label) {
    bool found = false;
    auto edgeTransient = edges_.transient();

    for (const auto &e : edges_) {
      if (e.source == source && e.target == target && e.label == label) {
        edgeTransient.erase(e);
        found = true;
      }
    }
    if (found) {
      edges_ = edgeTransient.persistent();
    }
    return found;
  }

  bool hasEdgeImpl(const NodeUID &source, const NodeUID &target,
                   const EdgeUID &label) const {
    return std::ranges::any_of(edges_, [&](const auto &e) {
      return e.source == source && e.target == e.target && e.label == label;
    });
  }

  std::vector<EdgeType> getAllEdgesImpl() const {
    return std::vector<EdgeType>(edges_.begin(), edges_.end());
  }

  std::vector<EdgeType> getAllOutgoingEdgesImpl(const NodeUID &uid) const {
    std::vector<EdgeType> res;
    std::ranges::copy_if(edges_, std::back_inserter(res),
                         [&](const auto &e) { return e.source == uid; });
    return res;
  }

  void removeAllOutgoingEdgesByLabelImpl(const NodeUID &uid,
                                         const EdgeUID &label) {
    auto edgeTransient = edges_.transient();
    for (const auto &e : edges_) {
      if (e.source == uid && e.label == label) {
        edgeTransient.erase(e);
      }
    }
    edges_ = edgeTransient.persistent();
  }

  [[nodiscard]] std::vector<EdgeType>
  getAllOutgoingEdgesByLabelImpl(const NodeUID &uid,
                                 const EdgeUID &label) const {
    std::vector<EdgeType> res;
    std::ranges::copy_if(edges_, std::back_inserter(res), [&](const auto &e) {
      return e.source == uid && e.label == label;
    });
    return res;
  }

  // Unique order-independent Hash
  std::size_t hash() const {
    // Hash NodeMap (Key: uint32_t, Value: uint8_t)
    std::size_t nodes_hash = 0;
    for (const auto &[node_id, node_type] : nodes_) {
      // Pack node_id and node_type together directly into 64-bit int
      std::size_t pair_value = (static_cast<std::size_t>(node_id) << 8) |
                               (static_cast<std::size_t>(node_type));
      nodes_hash +=
          mix_hash(pair_value); // Summation ensures order independence
    }

    // Hash EdgeSet (PackedEdge<uint32_t, uint32_t>)
    std::size_t edges_hash =
        std::accumulate(edges_.begin(), edges_.end(), std::size_t{0},
                        [](std::size_t acc, const auto &edge) {
                          return acc + mix_hash(std::hash<EdgeType>{}(edge));
                        });

    // Combine nodes and edges hash
    std::size_t combined = nodes_hash;
    combined ^= edges_hash + 0x9e3779b9 + (combined << 6) + (combined >> 2);
    return combined;
  }

  bool equalsImpl(const ECMAGraph &other) const {
    if (this->isUnreachable_ != other.isUnreachable_)
      return false;

    // Fast pointer-equality path via immer HAMT root sharing
    if (nodes_ == other.nodes_ && edges_ == other.edges_)
      return true;

    if (nodes_.size() != other.nodes_.size() ||
        edges_.size() != other.edges_.size())
      return false;

    for (const auto &[uid, tag] : nodes_) {
      auto otherTag = other.nodes_.find(uid);
      if (!otherTag || *otherTag != tag)
        return false;
    }
    for (const auto &e : edges_) {
      // cppcheck-suppress useStlAlgorithm
      if (!other.edges_.count(e))
        return false;
    }
    return true;
  }

  void dumpDOT(
      std::ostream &os, const std::string &title,
      const std::function<std::unordered_map<std::string, std::string>(NodeUID)>
          &nodeMetaMapper);
};

class PKRGlobalState {
  using ActionClosure = ECMAGraph::ActionClosure;
  using StateHash = ECMAGraph::StateHash;

private:
  inline static NodeUID INF_VAL = 0;
  inline static NodeUID NAN_VAL = 0;
  inline static NodeUID UNDEF_VAL = 0;
  inline static NodeUID NULL_VAL = 0;
  inline static NodeUID TRUE_VAL = 0;
  inline static NodeUID FALSE_VAL = 0;
  inline static NodeUID NUMBER_VAL = 0;
  inline static NodeUID STRING_VAL = 0;
  inline static NodeUID BIGINT_VAL = 0;
#define AS_SYM_FIELD(name) inline static NodeUID PKR_SYMBOL_TAG(name) = 0;
  DEF_WELL_KNOWN_SYMBOLS(AS_SYM_FIELD)
#undef AS_SYM_FIELD

  // See list-globals.hpp.
#define AS_GLOBAL_FIELDS(name) inline static NodeUID name = 0;
  DEF_GLOBAL_IDENTITIES(AS_GLOBAL_FIELDS)
#undef AS_GLOBAL_FIELDS

  inline static std::unordered_map<EdgeUID, NodeUID> globalStackBindings;
  inline static bool isInitialized = false;
  inline static std::map<std::pair<NodeUID, EdgeUID>, NodeUID> sentinelMap_;

public:
  inline static std::function<NodeUID()> ReserveNodeUID = nullptr;
  inline static std::function<EdgeUID(std::string_view)> EdgeIntern = nullptr;
  inline static std::function<std::string_view(EdgeUID)> EdgeGet = nullptr;
  inline static boost::bimap<NodeUID, ActionClosure> ActionClosureMap;

  // Populated by Init from DEF_WELL_KNOWN_SYMBOLS: symbol node -> edge label.
  inline static std::unordered_map<NodeUID, std::string> WellKnownSymbolEdges;
  inline static std::unordered_map<NodeUID, std::string> ActionNameMap;
  inline static std::unordered_map<StateHash, std::vector<ECMAGraph>>
      StateHashToState;
  inline static boost::bimap<StateHash, NodeUID> StateMap;

#define AS_FIELDS(name) inline static ActionClosure name = nullptr;
  DEF_GRAPH_CLOSURES(AS_FIELDS)
#undef AS_FIELDS

  static NodeUID getINF() {
    ASSERT(isInitialized);
    return INF_VAL;
  }

  static NodeUID getNAN() {
    ASSERT(isInitialized);
    return NAN_VAL;
  }

  static NodeUID getUNDEF() {
    ASSERT(isInitialized);
    return UNDEF_VAL;
  }

  static NodeUID getNULL() {
    ASSERT(isInitialized);
    return NULL_VAL;
  }

  static NodeUID getTRUE() {
    ASSERT(isInitialized);
    return TRUE_VAL;
  }

  static NodeUID getFALSE() {
    ASSERT(isInitialized);
    return FALSE_VAL;
  }

  static NodeUID getNUMBER() {
    ASSERT(isInitialized);
    return NUMBER_VAL;
  }

  static NodeUID getSTRING() {
    ASSERT(isInitialized);
    return STRING_VAL;
  }

  static NodeUID getBIGINT() {
    ASSERT(isInitialized);
    return BIGINT_VAL;
  }

#define AS_SYM_GETTER(name)                                                    \
  static NodeUID getSYMBOL_##name() {                                          \
    ASSERT(isInitialized);                                                     \
    return PKR_SYMBOL_TAG(name);                                               \
  }
  DEF_WELL_KNOWN_SYMBOLS(AS_SYM_GETTER)
#undef AS_SYM_GETTER

  // A well-known symbol used as a property key names a real edge, so a computed
  // key that resolves to one does not have to fall back to the bucket.
  static const std::string *wellKnownSymbolLabel(NodeUID node) {
    auto it = WellKnownSymbolEdges.find(node);
    return it == WellKnownSymbolEdges.end() ? nullptr : &it->second;
  }

#define AS_GLOBAL_GETTERS(name)                                                \
  static NodeUID get##name() { return name; }
  DEF_GLOBAL_IDENTITIES(AS_GLOBAL_GETTERS)
#undef AS_GLOBAL_GETTERS

  static std::vector<EdgeUID> getGlobals() {
    std::vector<EdgeUID> res;
    res.reserve(globalStackBindings.size());
    std::transform(globalStackBindings.begin(), globalStackBindings.end(),
                   std::back_inserter(res),
                   [&](const auto e) { return e.first; });
    return res;
  }

  // Reserves the binding for a named global the environment provides.
  static NodeUID declareGlobal(EdgeUID stackBindingRef) {
    ASSERT(isInitialized);
    auto [it, fresh] = globalStackBindings.try_emplace(stackBindingRef, 0);
    if (fresh)
      it->second = ReserveNodeUID();
    return it->second;
  }

  static NodeUID getGlobal(EdgeUID stackBindingRef) {
    auto it = globalStackBindings.find(stackBindingRef);
    ASSERT(it != globalStackBindings.end());
    return it->second;
  }

  static bool isKnownGlobal(EdgeUID stackBindingRef) {
    return globalStackBindings.find(stackBindingRef) !=
           globalStackBindings.end();
  }

  static NodeUID getGlobal(const char *stackBindingRef) {
    return getGlobal(EdgeIntern(stackBindingRef));
  }

  static void Init(const std::function<NodeUID()> &reserveNodeUID,
                   const std::function<EdgeUID(std::string_view)> &edgeIntern,
                   const std::function<std::string_view(EdgeUID)> &edgeGet) {
    ASSERT(!isInitialized);
    ReserveNodeUID = reserveNodeUID;
    EdgeIntern = edgeIntern;
    EdgeGet = edgeGet;
    isInitialized = true;
    INF_VAL = reserveNodeUID();
    NAN_VAL = reserveNodeUID();
    UNDEF_VAL = reserveNodeUID();
    NULL_VAL = reserveNodeUID();
    TRUE_VAL = reserveNodeUID();
    FALSE_VAL = reserveNodeUID();
    NUMBER_VAL = reserveNodeUID();
    STRING_VAL = reserveNodeUID();
    BIGINT_VAL = reserveNodeUID();
#define AS_SYM_INIT(name)                                                      \
  PKR_SYMBOL_TAG(name) = reserveNodeUID();                                     \
  WellKnownSymbolEdges.emplace(PKR_SYMBOL_TAG(name),                           \
                               PKR_SYMBOL_LABEL(name));
    DEF_WELL_KNOWN_SYMBOLS(AS_SYM_INIT)
#undef AS_SYM_INIT

#define AS_GLOBAL_INIT(name) name = reserveNodeUID();
    DEF_GLOBAL_IDENTITIES(AS_GLOBAL_INIT)
#undef AS_GLOBAL_INIT

#define AS_ASSIGN(name)                                                        \
  if (!PKRGlobalState::name)                                                   \
    throw std::runtime_error("Action Closure not attached! " #name);           \
  associateActionClosure(reserveNodeUID(), PKRGlobalState::name);
    DEF_GRAPH_CLOSURES(AS_ASSIGN)
#undef AS_ASSIGN

#define AS_NAME(name)                                                          \
  ActionNameMap.emplace(getActionNode(PKRGlobalState::name), #name);
    DEF_GRAPH_CLOSURES(AS_NAME)
#undef AS_NAME
  }

  static std::string getActionName(NodeUID node) {
    auto it = ActionNameMap.find(node);
    if (it != ActionNameMap.end())
      return it->second;
    return "<unnamed:" + std::to_string(node) + ">";
  }

  static void DumpDebugInfo(std::ostream &os = std::cout) {
    if (!isInitialized) {
      os << "[DEBUG] System is NOT initialized.\n";
      return;
    }

    os << "=== System Initialization Debug Dump ===\n\n";

    os << "[Primitives]\n";
    os << "  INF_VAL:   " << INF_VAL << "\n";
    os << "  NAN_VAL:   " << NAN_VAL << "\n";
    os << "  UNDEF_VAL: " << UNDEF_VAL << "\n";
    os << "  NULL_VAL:  " << NULL_VAL << "\n";
    os << "  TRUE_VAL:  " << TRUE_VAL << "\n";
    os << "  FALSE_VAL: " << FALSE_VAL << "\n\n";

    os << "[Global Identities]\n";
#define AS_GLOBAL_DEBUG(name) os << "  " #name ": " << name << "\n";
    DEF_GLOBAL_IDENTITIES(AS_GLOBAL_DEBUG)
#undef AS_GLOBAL_DEBUG
    os << "\n";

    os << "[Global Stack Bindings]\n";
    for (const auto &[edgeUid, nodeUid] : globalStackBindings) {
      std::string_view edgeName =
          EdgeGet ? EdgeGet(edgeUid) : "<unbound_edge_getter>";
      os << "  Edge [" << edgeName << "] (UID: " << edgeUid
         << ") -> NodeUID: " << nodeUid << "\n";
    }
    os << "\n";

    os << "[Action Closures Status]\n";
#define PRINT_CLOSURE_STATUS(name)                                             \
  os << "  " << #name << ": "                                                  \
     << (PKRGlobalState::name ? "Attached" : "Missing")                        \
     << (PKRGlobalState::name ? (" [" +                                        \
                                 std::to_string(ActionClosureMap.right.at(     \
                                     PKRGlobalState::name)) +                  \
                                 "]")                                          \
                              : "")                                            \
     << "\n";
    DEF_GRAPH_CLOSURES(PRINT_CLOSURE_STATUS)
#undef PRINT_CLOSURE_STATUS

    os << "========================================\n";
  }

  static bool nodeHasActionClosure(NodeUID node) {
    return ActionClosureMap.left.find(node) != ActionClosureMap.left.end();
  }

  static bool actionClosureHasNode(ActionClosure clos) {
    return ActionClosureMap.right.find(clos) != ActionClosureMap.right.end();
  }

  static void associateActionClosure(NodeUID node, ActionClosure clos) {
    if (nodeHasActionClosure(node))
      return;
    PKRGlobalState::ActionClosureMap.insert({node, clos});
  }

  static ActionClosure getActionClosure(NodeUID node) {
    ASSERT(nodeHasActionClosure(node));
    return ActionClosureMap.left.at(node);
  }

  static NodeUID getActionNode(ActionClosure clos) {
    ASSERT(actionClosureHasNode(clos));
    return ActionClosureMap.right.at(clos);
  }

  [[nodiscard]] static const std::vector<ECMAGraph> &
  getStateVector(NodeUID node) {
    auto stateMapIt = StateMap.right.find(node);
    ASSERT(stateMapIt != StateMap.right.end());
    auto it = StateHashToState.find(stateMapIt->first);
    ASSERT(it != StateHashToState.end());
    return it->second;
  }

  [[nodiscard]] static NodeUID storeState(const std::vector<ECMAGraph> &state) {
    StateHash hash = std::accumulate(
        state.begin(), state.end(), 0,
        [](std::size_t acc, const auto &s) { return acc + s.hash(); });
    const auto stateMapIt = StateMap.left.find(hash);
    if (stateMapIt == StateMap.left.end()) {
      NodeUID node = ReserveNodeUID();
      StateMap.insert({hash, node});
      StateHashToState[hash] = state;
      return node;
    } else {
      const auto it = StateHashToState.find(hash);
      ASSERT(it != StateHashToState.end());
      ASSERT(it->second.size() == state.size());
      for (auto i = 0; i < state.size(); i++) {
        ASSERT(it->second[i].hash() == state[i].hash());
      }
      return stateMapIt->second;
    }
  }

  static NodeUID generateSentinel(const NodeUID &uid, const EdgeUID &label) {
    auto key = std::make_pair(uid, label);
    auto it = sentinelMap_.find(key);
    if (it != sentinelMap_.end()) {
      return it->second;
    }
    NodeUID sID = ReserveNodeUID();
    sentinelMap_[key] = sID;
    return sID;
  }

  static std::optional<std::string> getNodeName(NodeUID uid) {
    if (!isInitialized)
      return std::nullopt;

    // Primitive values
    if (uid == INF_VAL)
      return "INF";
    if (uid == NAN_VAL)
      return "NaN";
    if (uid == UNDEF_VAL)
      return "undefined";
    if (uid == NULL_VAL)
      return "null";
    if (uid == TRUE_VAL)
      return "true";
    if (uid == FALSE_VAL)
      return "false";
    if (uid == NUMBER_VAL)
      return "Number";
    if (uid == STRING_VAL)
      return "String";
    if (uid == BIGINT_VAL)
      return "BigInt";
#define AS_SYM_NAME(name)                                                      \
  if (uid == PKR_SYMBOL_TAG(name))                                             \
    return "Symbol." #name;
    DEF_WELL_KNOWN_SYMBOLS(AS_SYM_NAME)
#undef AS_SYM_NAME

    // Global Identities
#define AS_GLOBAL_NAME(name)                                                   \
  if (uid == name)                                                             \
    return #name;
    DEF_GLOBAL_IDENTITIES(AS_GLOBAL_NAME)
#undef AS_GLOBAL_NAME

    // Global Stack Bindings
    for (const auto &[edgeUid, nodeUid] : globalStackBindings) {
      if (nodeUid == uid) {
        std::string_view binding = EdgeGet ? EdgeGet(edgeUid) : "bound_node";
        return std::string(binding);
      }
    }

    // Action Closures lookup via ActionClosureMap - prefer the registered
    // name (e.g. "NAC_OOBJ_Get") over the generic fallback.
    if (nodeHasActionClosure(uid)) {
      std::string name = getActionName(uid);
      if (name.rfind("<unnamed:", 0) == 0)
        return "ActionClosure";
      return name;
    }

    // Saved states (see storeState/getStateVector), reachable via the
    // Await mechanism's [[State]] edge.
    auto stateIt = StateMap.right.find(uid);
    if (stateIt != StateMap.right.end())
      return "SavedState_" + std::to_string(stateIt->second);

    // Sentinels (see generateSentinel): a synthetic per-(node, edge label)
    // node, e.g. a closure's shared `arguments` object.
    for (const auto &[key, sentinelUid] : sentinelMap_) {
      if (sentinelUid == uid) {
        const auto &[owner, label] = key;
        std::string_view labelName = EdgeGet ? EdgeGet(label) : "sentinel";
        return "Sentinel(" + std::string(labelName) + "@" +
               std::to_string(owner) + ")";
      }
    }

    return std::nullopt;
  }
};

inline void ECMAGraph::dumpDOT(
    std::ostream &os, const std::string &title,
    const std::function<std::unordered_map<std::string, std::string>(NodeUID)>
        &nodeMetaMapper) {
  os << "digraph \"" << title << "\" {\n";

  std::vector<Prakriti::NodeUID> nodes = getAllNodes();
  for (const auto uid : nodes) {
    auto tag = getNodeTAG(uid);
    // Check if node is a known global state node
    auto globalName = Prakriti::PKRGlobalState::getNodeName(uid);

    os << "  node_" << uid << " [";

    auto nodeMetadata = nodeMetaMapper(uid);
    nodeMetadata["kind"] = dumpPKRTagToString(tag);
    size_t numMeta = nodeMetadata.size();

    for (const auto e : nodeMetadata) {
      os << "\"" << e.first << "\"=" << "\"" << e.second << "\"";
      os << (--numMeta == 0 ? " " : ", ");
    }
    os << "];\n";
  }

  std::vector<Prakriti::ECMAGraph::EdgeType> edges = getAllEdges();
  for (const auto &e : edges) {
    os << "  node_" << e.source << " -> node_" << e.target << " [label=\""
       << Prakriti::PKRGlobalState::EdgeGet(e.label) << "\"];\n";
  }

  os << "}\n";
}

} // namespace Prakriti

#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace Prakriti {

struct TraceEvent {
  std::optional<NodeUID> node;
  const std::string &name;
  size_t depth;
  const std::vector<NodeUID> &args;
  const std::vector<std::string> &argsStr;
  const std::string &extra; // free-form context: Karma's candidate/skipped
                             // node lists on enter, an action's return value
                             // or a merge summary on exit
  bool isEnter;
  std::chrono::nanoseconds duration;
};

inline std::string formatNodeVec(const std::vector<NodeUID> &v) {
  std::ostringstream o;
  o << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    if (i)
      o << ",";
    o << v[i];
  }
  o << "]";
  return o.str();
}

inline std::string formatStrVec(const std::vector<std::string> &v) {
  std::ostringstream o;
  o << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    if (i)
      o << ",";
    o << "\"" << v[i] << "\"";
  }
  o << "]";
  return o.str();
}

inline std::string formatDuration(std::chrono::nanoseconds d) {
  double ns = static_cast<double>(d.count());
  std::ostringstream out;
  out.precision(1);
  out << std::fixed;
  if (ns < 1'000.0)
    out << ns << "ns";
  else if (ns < 1'000'000.0)
    out << (ns / 1'000.0) << "us";
  else if (ns < 1'000'000'000.0)
    out << (ns / 1'000'000.0) << "ms";
  else
    out << (ns / 1'000'000'000.0) << "s";
  return out.str();
}

// Renders one call-graph line per event, using tree guides ("|  ") to show
// ancestry and an arrow ("->"/"<-") at the event's own depth - these ligate
// into single glyphs in fonts like Fira Code, e.g.:
//   -> Karma node=? acts=[10,11] args=[44,45]
//   |  -> NAC_OOBJ_Get node=10 args=[44,45]
//   |  <- NAC_OOBJ_Get node=10 (18.5us) ret=[99]
//   <- Karma node=? (52.1us) invoked=1/2 skipped=[11]
inline void defaultTraceSink(const TraceEvent &ev) {
  std::ostringstream line;
  for (size_t i = 0; i < ev.depth; ++i)
    line << "|  ";
  line << (ev.isEnter ? "-> " : "<- ") << ev.name
       << " node=" << (ev.node ? std::to_string(*ev.node) : "?");
  if (ev.isEnter) {
    if (!ev.args.empty())
      line << " args=" << formatNodeVec(ev.args);
    if (!ev.argsStr.empty())
      line << " strs=" << formatStrVec(ev.argsStr);
  } else {
    line << " (" << formatDuration(ev.duration) << ")";
  }
  if (!ev.extra.empty())
    line << " " << ev.extra;
  line << "\n";
  std::cerr << line.str();
}

inline std::function<void(const TraceEvent &)> g_TraceSink = defaultTraceSink;
inline thread_local size_t g_TraceDepth = 0;

// The NodeUID an action closure is being invoked for, when known. Actions
// are shared closures (e.g. NAC_OOBJ_Get is one closure invoked for every
// ordinary object), so the closure body itself has no way to know which
// node it was called on - callers that know it (Karma, invokeAction) record
// it here for the duration of the call so tracing can report it and resolve
// a friendly registered name for it.
inline thread_local std::optional<NodeUID> g_CurrentActionNode;

class CurrentActionNodeGuard {
public:
  explicit CurrentActionNodeGuard(NodeUID node)
      : previous_(g_CurrentActionNode) {
    g_CurrentActionNode = node;
  }
  ~CurrentActionNodeGuard() { g_CurrentActionNode = previous_; }

  CurrentActionNodeGuard(const CurrentActionNodeGuard &) = delete;
  CurrentActionNodeGuard &operator=(const CurrentActionNodeGuard &) = delete;

private:
  std::optional<NodeUID> previous_;
};

inline bool &traceEnabledFlag() {
  static bool enabled = [] {
    const char *env = std::getenv("PRAKRITI_TRACE");
    return env != nullptr && std::string_view(env) != "0";
  }();
  return enabled;
}

inline bool isTraceEnabled() { return traceEnabledFlag(); }
inline void setTraceEnabled(bool enabled) { traceEnabledFlag() = enabled; }
inline void setTraceSink(std::function<void(const TraceEvent &)> sink) {
  g_TraceSink = std::move(sink);
}

inline std::string_view basename(std::string_view path) {
  auto pos = path.find_last_of("/\\");
  return pos == std::string_view::npos ? path : path.substr(pos + 1);
}

// Generic RAII span for tracing a named call (e.g. Karma) that
// isn't itself a registered action closure. No-ops entirely (single bool
// check) when tracing is disabled. `args`/`argsStr` must outlive the span.
// `enterExtra` is baked in at construction (callers should only bother
// building it when isTraceEnabled()); `setExitExtra` may be called any time
// before destruction to attach extra context to the exit line (e.g. how
// many of the candidate nodes were actually invoked).
class TraceSpan {
public:
  TraceSpan(std::optional<NodeUID> node, std::string name,
           const std::vector<NodeUID> &args,
           const std::vector<std::string> &argsStr, std::string enterExtra)
      : node_(node), name_(std::move(name)), args_(args), argsStr_(argsStr),
        active_(isTraceEnabled()) {
    if (!active_)
      return;
    depth_ = g_TraceDepth++;
    start_ = std::chrono::steady_clock::now();
    g_TraceSink(
        TraceEvent{node_, name_, depth_, args_, argsStr_, enterExtra, true, {}});
  }

  void setExitExtra(std::string extra) {
    if (active_)
      exitExtra_ = std::move(extra);
  }

  ~TraceSpan() {
    if (!active_)
      return;
    auto elapsed = std::chrono::steady_clock::now() - start_;
    --g_TraceDepth;
    g_TraceSink(TraceEvent{node_, name_, depth_, args_, argsStr_, exitExtra_,
                           false,
                           std::chrono::duration_cast<std::chrono::nanoseconds>(
                               elapsed)});
  }

  TraceSpan(const TraceSpan &) = delete;
  TraceSpan &operator=(const TraceSpan &) = delete;

private:
  std::optional<NodeUID> node_;
  std::string name_;
  const std::vector<NodeUID> &args_;
  const std::vector<std::string> &argsStr_;
  std::string exitExtra_;
  bool active_;
  size_t depth_ = 0;
  std::chrono::steady_clock::time_point start_;
};

// RAII scope logging a single action invocation's enter/exit, including its
// return value. No-ops entirely (single bool check) when tracing is
// disabled. `fallbackName` identifies the action by where its
// DEFINE_ACTION() was written; if the invoking node is known (via
// CurrentActionNodeGuard) and registered under a friendlier name (e.g.
// "NAC_OOBJ_Get"), that name is used instead.
class ActionTraceScope {
public:
  ActionTraceScope(const std::string &fallbackName,
                    const ECMAGraph::PJSSL_ARG &args)
      : args_(args.L), argsStr_(args.A), active_(isTraceEnabled()) {
    if (!active_)
      return;
    node_ = g_CurrentActionNode;
    name_ = resolveName(fallbackName, node_);
    depth_ = g_TraceDepth++;
    start_ = std::chrono::steady_clock::now();
    g_TraceSink(
        TraceEvent{node_, name_, depth_, args_, argsStr_, exitExtra_, true, {}});
  }

  void setResult(const ECMAGraph::PJSSL_RET &ret) {
    if (!active_)
      return;
    exitExtra_ = "ret=" + formatNodeVec(ret.L);
    if (!ret.A.empty())
      exitExtra_ += " retStrs=" + formatStrVec(ret.A);
  }

  ~ActionTraceScope() {
    if (!active_)
      return;
    auto elapsed = std::chrono::steady_clock::now() - start_;
    --g_TraceDepth;
    g_TraceSink(TraceEvent{node_, name_, depth_, args_, argsStr_, exitExtra_,
                           false,
                           std::chrono::duration_cast<std::chrono::nanoseconds>(
                               elapsed)});
  }

  ActionTraceScope(const ActionTraceScope &) = delete;
  ActionTraceScope &operator=(const ActionTraceScope &) = delete;

private:
  static std::string resolveName(const std::string &fallbackName,
                                  const std::optional<NodeUID> &node) {
    if (!node)
      return fallbackName;
    std::string registered = PKRGlobalState::getActionName(*node);
    if (registered.rfind("<unnamed:", 0) == 0)
      return fallbackName;
    return registered;
  }

  std::optional<NodeUID> node_;
  std::string name_;
  const std::vector<NodeUID> &args_;
  const std::vector<std::string> &argsStr_;
  std::string exitExtra_;
  bool active_;
  size_t depth_ = 0;
  std::chrono::steady_clock::time_point start_;
};

// Wraps a DEFINE_ACTION() lambda so every action closure is traced no
// matter how it later gets invoked (Karma, a direct closure call, or any
// future call site) - tracing is baked into the closure itself rather than
// depending on the call site to opt in.
template <typename F>
inline ECMAGraph::ActionClosure makeTracedAction(std::string_view file,
                                                 int line, F &&f) {
  std::string name = std::string(basename(file)) + ":" + std::to_string(line);
  return std::make_shared<ECMAGraph::ActionClosureImpl>(
      [name = std::move(name), f = std::forward<F>(f)](
          const ECMAGraph::PJSSL_ARG &args) -> ECMAGraph::PJSSL_RET {
        ActionTraceScope trace(name, args);
        auto ret = f(args);
        trace.setResult(ret);
        return ret;
      });
}

//
// Public tracing API
//
// Everything above is wired into Prakriti's own internals (action closures,
// Karma). TraceHelper is the surface for code outside those internals that
// wants its own events on the same call-graph output: they nest inside
// Prakriti's spans, share the sink, and are gated by the same PRAKRITI_TRACE
// env var / setTraceEnabled() switch.
//
//   {
//     TraceHelper th("My Event");
//     th.start();
//     ...
//     th.end();            // or just let the destructor close it
//   }
//
//   { TraceHelperAuto th("Scoped"); ... }   // starts on construction
//
// Optional detail, all of it a no-op when tracing is off:
//
//   TraceHelper th("Lookup", {nodeId}, {"propName"});  // enter-line args
//   th.setExtra("hit=1");        // free-form context on the exit line
//   if (th.enabled()) ...        // skip building an expensive extra
//
// Unlike TraceSpan the args are copied, so nothing has to outlive the helper.
// Spans are expected to close in LIFO order; an early return or a throw is
// fine, the destructor closes whatever is still open.
class TraceHelper {
public:
  explicit TraceHelper(std::string name,
                       std::optional<NodeUID> node = std::nullopt)
      : node_(node), name_(std::move(name)) {}

  TraceHelper(std::string name, std::vector<NodeUID> args,
              std::vector<std::string> argsStr = {},
              std::optional<NodeUID> node = std::nullopt)
      : node_(node), name_(std::move(name)), args_(std::move(args)),
        argsStr_(std::move(argsStr)) {}

  ~TraceHelper() { end(); }

  // Args are printed on the enter line only, so they have to be set before
  // start(); afterwards they are ignored.
  TraceHelper &arg(NodeUID node) {
    args_.push_back(node);
    return *this;
  }
  TraceHelper &arg(std::string str) {
    argsStr_.push_back(std::move(str));
    return *this;
  }
  TraceHelper &setNode(NodeUID node) {
    node_ = node;
    return *this;
  }

  // Free-form context. Set before start() it lands on the enter line, after
  // it on the exit line; the last value set before each line wins.
  TraceHelper &setExtra(std::string extra) {
    extra_ = std::move(extra);
    return *this;
  }

  void start() {
    if (running_ || !isTraceEnabled())
      return;
    running_ = true;
    depth_ = g_TraceDepth++;
    start_ = std::chrono::steady_clock::now();
    g_TraceSink(
        TraceEvent{node_, name_, depth_, args_, argsStr_, extra_, true, {}});
    extra_.clear();
  }

  void start(std::string enterExtra) {
    extra_ = std::move(enterExtra);
    start();
  }

  void end() {
    if (!running_)
      return;
    running_ = false;
    auto elapsed = std::chrono::steady_clock::now() - start_;
    // Restore rather than decrement: for the expected LIFO use this is the
    // same thing, and it keeps a stray out-of-order end() from underflowing
    // the shared depth counter.
    g_TraceDepth = depth_;
    g_TraceSink(TraceEvent{node_, name_, depth_, args_, argsStr_, extra_, false,
                           std::chrono::duration_cast<std::chrono::nanoseconds>(
                               elapsed)});
    extra_.clear();
  }

  void end(std::string exitExtra) {
    extra_ = std::move(exitExtra);
    end();
  }

  // True between the enter line and the exit line.
  bool running() const { return running_; }
  // False when tracing is off, i.e. when building extras would be wasted work.
  bool enabled() const { return isTraceEnabled(); }
  const std::string &name() const { return name_; }

  TraceHelper(const TraceHelper &) = delete;
  TraceHelper &operator=(const TraceHelper &) = delete;

private:
  std::optional<NodeUID> node_;
  std::string name_;
  std::vector<NodeUID> args_;
  std::vector<std::string> argsStr_;
  std::string extra_;
  bool running_ = false;
  size_t depth_ = 0;
  std::chrono::steady_clock::time_point start_;
};

// TraceHelper that opens on construction and closes at the end of the scope.
// Args have to go through the constructor here, since start() has already
// run by the time the caller gets the object.
class TraceHelperAuto : public TraceHelper {
public:
  explicit TraceHelperAuto(std::string name,
                           std::optional<NodeUID> node = std::nullopt)
      : TraceHelper(std::move(name), node) {
    start();
  }

  TraceHelperAuto(std::string name, std::vector<NodeUID> args,
                  std::vector<std::string> argsStr = {},
                  std::optional<NodeUID> node = std::nullopt)
      : TraceHelper(std::move(name), std::move(args), std::move(argsStr),
                    node) {
    start();
  }
};

} // namespace Prakriti

//
// Common Edge Labels
//
#define PKR_STK "[[stack-ref]]"
#define PKR_PROTOTYPE "[[Prototype]]"
#define PKR_EXTENSIBLE "[[Extensible]]"
#define PKR_VALUE "[[Value]]"
#define PKR_WRITABLE "[[Writable]]"
#define PKR_ENUMERABLE "[[Enumerable]]"
#define PKR_CONFIGURABLE "[[Configurable]]"
#define PKR_GetPrototypeOf "[[GetPrototypeOf]]"
#define PKR_SetPrototypeOf "[[SetPrototypeOf]]"
#define PKR_IsExtensible "[[IsExtensible]]"
#define PKR_PreventExtensions "[[PreventExtensions]]"
#define PKR_GetOwnProperty "[[GetOwnProperty]]"
#define PKR_DefineOwnProperty "[[DefineOwnProperty]]"
#define PKR_HasProperty "[[HasProperty]]"
#define PKR_Get "[[Get]]"
#define PKR_Set "[[Set]]"
#define PKR_Delete "[[Delete]]"
#define PKR_OwnPropertyKeys "[[OwnPropertyKeys]]"
#define PKR_NULL_HasProperty "[[NULL_HasProperty]]"
#define PKR_NULL_Get "[[NULL_Get]]"
#define PKR_NULL_GetPrototypeOf "[[NULL_GetPrototypeOf]]"
#define PKR_Call "[[Call]]"
#define PKR_Eval "[[Eval]]"
#define PKR_StoreTarget "[[StoreTarget]]"
#define PKR_State "[[State]]"
#define PKR_TRANSIENCE "[[transience]]"
#define PKR_ARGUMENTS "[[Arguments]]"
#define PKR_MAPPED_ARGUMENTS "[[MappedArguments]]"
#define PKR_ITERATED "[[IteratedObject]]"
// The primitive a String exotic object wraps.
#define PKR_STRING_DATA "[[StringData]]"
#define PKR_TRANSIENCE_BACKUP "[[transience-backup]]"
#define PKR_IS_EXECUTING "[[is-executing]]"
#define PKR_DEFINITE "[[Definite]]"
#define PKR_SENSITIVE "[[Sensitive]]"
#define PKR_UNKNOWN_FIELD "[[Unknown-Field]]"

#include <algorithm>
#include <iterator>
#include <set>
#include <string>
#include <vector>

#define SET_AC(src, ac, edge)                                                  \
  temp = PKRGlobalState::getActionNode(PKRGlobalState::ac);                    \
  G->removeAllOutgoingEdgesByLabel(src, PKRGlobalState::EdgeIntern(edge));     \
  G->addNode(temp, TAG::ACT);                                                  \
  G->addEdge(src, temp, PKRGlobalState::EdgeIntern(edge))

#define DEFINE_ACTION()                                                        \
      Prakriti::makeTracedAction(__FILE__, __LINE__,                              \
          [](const ECMAGraph::PJSSL_ARG & args) -> ECMAGraph::PJSSL_RET

#define FIELD_VEC(Name)                                                        \
public:                                                                        \
  void add##Name(NodeUID id) { Name##_.push_back(id); }                        \
  const std::vector<NodeUID> &get##Name() const { return Name##_; }            \
                                                                               \
private:                                                                       \
  std::vector<NodeUID> Name##_;

namespace Prakriti {

//
// Closure calling convention
//
// PJSSL_ARG.L is flat, so a call flattens the this-value and every positional
// argument into it and records their index ranges in A: A[0] is the
// this-value's range (empty when there is none), A[1] the comma-joined ranges
// of the positional arguments. Both sides of the call must agree, so encode
// and decode live here rather than in the interpreter. ~Meetesh
//
inline std::string encodeArgRanges(const std::vector<std::set<NodeUID>> &vals,
                                   size_t start = 0) {
  std::string out;
  for (size_t i = 0; i < vals.size(); i++) {
    size_t count = vals[i].size();
    size_t end = start + (count == 0 ? 0 : count - 1);
    if (i)
      out += ",";
    out += std::to_string(start) + "-" + std::to_string(end);
    start += count;
  }
  return out;
}

inline std::vector<std::set<NodeUID>>
decodeArgRanges(const std::vector<NodeUID> &flat, const std::string &encoded) {
  std::vector<std::set<NodeUID>> vals;
  size_t pos = 0;
  while (pos < encoded.size()) {
    size_t comma = encoded.find(',', pos);
    std::string token = encoded.substr(pos, comma - pos);
    size_t dash = token.find('-');
    size_t start = std::stoul(token.substr(0, dash));
    size_t end = std::stoul(token.substr(dash + 1));
    vals.push_back(
        std::set<NodeUID>(flat.begin() + start, flat.begin() + end + 1));
    if (comma == std::string::npos)
      break;
    pos = comma + 1;
  }
  return vals;
}

struct CallArgs {
  std::vector<NodeUID> L;
  std::vector<std::string> A;
};

inline CallArgs makeCall(const std::set<NodeUID> &thisVals,
                         const std::vector<std::set<NodeUID>> &positional) {
  CallArgs c;
  c.L.insert(c.L.end(), thisVals.begin(), thisVals.end());
  for (const auto &p : positional)
    c.L.insert(c.L.end(), p.begin(), p.end());
  c.A.push_back(thisVals.empty() ? "" : encodeArgRanges({thisVals}));
  c.A.push_back(encodeArgRanges(positional, thisVals.size()));
  return c;
}

//
// Helper Methods
//

struct KarmaResult {
  ECMAGraph clonedG;
  ECMAGraph::PJSSL_RET ret;
};

inline ECMAGraph::PJSSL_RET invokeAction(NodeUID id,
                                         const ECMAGraph::PJSSL_ARG &args) {
  CurrentActionNodeGuard nodeGuard(id);
  return (*PKRGlobalState::getActionClosure(id))(args);
}

inline std::vector<KarmaResult> Karma(const ECMAGraph *G,
                                      const std::vector<NodeUID> &acts,
                                      const ECMAGraph::PJSSL_ARG &args) {
  std::string enterExtra;
  if (isTraceEnabled())
    enterExtra = "acts=" + formatNodeVec(acts);
  TraceSpan span(std::nullopt, "Karma", args.L, args.A, enterExtra);

  std::vector<KarmaResult> res;
  std::vector<NodeUID> skipped;
  for (const NodeUID aID : acts) {
    if (PKRGlobalState::nodeHasActionClosure(aID)) {
      auto G_ = G->clone();
      auto ret =
          invokeAction(aID, ECMAGraph::PJSSL_ARG{&G_, args.L, args.A, args.X});
      res.push_back(KarmaResult{std::move(G_), ret});
    } else if (isTraceEnabled()) {
      skipped.push_back(aID);
    }
  }

  if (isTraceEnabled()) {
    std::string exitExtra = "invoked=" + std::to_string(res.size()) + "/" +
                            std::to_string(acts.size());
    if (!skipped.empty())
      exitExtra += " skipped=" + formatNodeVec(skipped);
    span.setExitExtra(exitExtra);
  }
  return res;
}

// Accumulating form: several independent invocations contribute to one join.
// Each act still runs on its own clone, and the caller decides when to join,
// so no invocation can observe another's side effects.
inline void Karma(const ECMAGraph *G, const std::vector<NodeUID> &acts,
                  const ECMAGraph::PJSSL_ARG &args, std::vector<NodeUID> &res,
                  std::vector<ECMAGraph> &branches) {
  for (auto &r : Karma(G, acts, args)) {
    res.insert(res.end(), r.ret.L.begin(), r.ret.L.end());
    branches.push_back(std::move(r.clonedG));
  }
}

// Replaces G with the union of the branches. Nothing ran means nothing to say
// about G, so an empty join leaves it alone.
inline void KarmaJoin(ECMAGraph *G, const std::vector<ECMAGraph> &branches) {
  if (branches.empty())
    return;
  ECMAGraph merged;
  merged.mutateMergeUnion(branches);
  *G = std::move(merged);
}

inline void KarmaJoin(ECMAGraph *G, const std::vector<KarmaResult> &rs) {
  std::vector<ECMAGraph> branches;
  branches.reserve(rs.size());
  std::transform(rs.begin(), rs.end(), std::back_inserter(branches),
                 [](const KarmaResult &r) { return r.clonedG; });
  KarmaJoin(G, branches);
}

template <typename T> inline bool isOnlyFalse(const T &nodes) {
  ASSERT(!nodes.empty());
  return std::ranges::all_of(
      nodes, [](const auto &tgt) { return tgt == PKRGlobalState::getFALSE(); });
}

[[nodiscard]] inline bool isOnlyFalse(ECMAGraph *G, NodeUID id, EdgeUID label) {
  return isOnlyFalse(G->getPointees(id, label));
}

template <typename T> inline bool isOnlyTrue(const T &nodes) {
  ASSERT(!nodes.empty());
  return std::ranges::all_of(
      nodes, [](const auto &tgt) { return tgt == PKRGlobalState::getTRUE(); });
}

[[nodiscard]] inline bool isOnlyTrue(ECMAGraph *G, NodeUID id, EdgeUID label) {
  return isOnlyTrue(G->getPointees(id, label));
}

inline bool hasSetInterface(const ECMAGraph *G, NodeUID node) {
  return G->getAllOutgoingEdgesByLabel(node,
                                       PKRGlobalState::EdgeIntern(PKR_Set))
             .size() > 0;
}

inline bool hasGetInterface(const ECMAGraph *G, NodeUID node) {
  return G->getAllOutgoingEdgesByLabel(node,
                                       PKRGlobalState::EdgeIntern(PKR_Get))
             .size() > 0;
}

} // namespace Prakriti

#include <set>

namespace Prakriti {

//
// == HELPER CLASSES ==
//

class FieldDescriptor : public ECMAGraph::NodeHelper {
public:
  using ECMAGraph::NodeHelper::NodeHelper;
  virtual ~FieldDescriptor() = default;
};

class TempFieldDescriptor : public FieldDescriptor {
public:
  TempFieldDescriptor() = default;
  virtual ~TempFieldDescriptor() = default;

  FIELD_VEC(Value)
  FIELD_VEC(Writable)
  FIELD_VEC(Enumerable)
  FIELD_VEC(Configurable)
  FIELD_VEC(Get)
  FIELD_VEC(Set)
  FIELD_VEC(Definite)
};

//
// FDUnion
//
// Only the accumulating form exists: an object node may stand for several
// concrete objects, so a property is a may-location. Replace semantics belong
// with singleton allocation nodes, which the abstraction does not have yet.
//

inline void appendFieldIfSpecified(const FieldDescriptor *self,
                                   const std::vector<NodeUID> &vals,
                                   const char *label) {
  if (vals.empty())
    return;
  EdgeUID edgeLabel = PKRGlobalState::EdgeIntern(label);
  for (NodeUID tgt : vals)
    self->getGraph()->addEdge(self->getID(), tgt, edgeLabel);
}

// Folds a descriptor into a linked field proxy without ever removing an
// existing edge, so an earlier write's contribution is never dropped.
inline void FDUnionAccumulateFT(const FieldDescriptor *self,
                                const TempFieldDescriptor *other) {
  if (!self->isLinked())
    throw std::runtime_error("FDUnion called on unlinked FieldDescriptor");

  appendFieldIfSpecified(self, other->getValue(), PKR_VALUE);
  appendFieldIfSpecified(self, other->getWritable(), PKR_WRITABLE);
  appendFieldIfSpecified(self, other->getEnumerable(), PKR_ENUMERABLE);
  appendFieldIfSpecified(self, other->getConfigurable(), PKR_CONFIGURABLE);
  appendFieldIfSpecified(self, other->getGet(), PKR_Get);
  appendFieldIfSpecified(self, other->getSet(), PKR_Set);
  appendFieldIfSpecified(self, other->getDefinite(), PKR_DEFINITE);
}

//
// GetAllSetters
//

inline std::vector<NodeUID> GetAllSetters(FieldDescriptor *self) {
  if (auto *t = dynamic_cast<TempFieldDescriptor *>(self))
    return GetAllSetters(t);

  return self->getGraph()->getPointees(self->getID(),
                                       PKRGlobalState::EdgeIntern(PKR_Set));
}

inline std::vector<NodeUID> GetAllSetters(const TempFieldDescriptor *self) {
  return self->getSet();
}

//
// SameValue
//

inline std::vector<NodeUID> GetValue(FieldDescriptor *self) {
  if (const auto *t = dynamic_cast<TempFieldDescriptor *>(self))
    return t->getValue();
  return self->getGraph()->getPointees(self->getID(),
                                       PKRGlobalState::EdgeIntern(PKR_VALUE));
}

inline bool SameValue(FieldDescriptor *a, FieldDescriptor *b) {
  auto av = GetValue(a);
  auto bv = GetValue(b);
  std::set<NodeUID> as(av.begin(), av.end());
  std::set<NodeUID> bs(bv.begin(), bv.end());
  return as == bs;
}

//
// IsNotConfigurable
//

inline bool IsNotConfigurable(FieldDescriptor *self) {
  if (auto *t = dynamic_cast<TempFieldDescriptor *>(self))
    return IsNotConfigurable(t);

  return isOnlyFalse(self->getGraph(), self->getID(),
                     PKRGlobalState::EdgeIntern(PKR_CONFIGURABLE));
}

inline bool IsNotConfigurable(TempFieldDescriptor *self) {
  auto &pts = self->getConfigurable();

  if (pts.empty())
    throw std::runtime_error("[[Configurable]] is empty");

  return isOnlyFalse(pts);
}

//
// IsNotEnumerable
//

inline bool IsNotEnumerable(FieldDescriptor *self) {
  if (auto *t = dynamic_cast<TempFieldDescriptor *>(self))
    return IsNotEnumerable(t);

  return isOnlyFalse(self->getGraph(), self->getID(),
                     PKRGlobalState::EdgeIntern(PKR_ENUMERABLE));
}

inline bool IsNotEnumerable(TempFieldDescriptor *self) {
  auto &pts = self->getEnumerable();

  if (pts.empty())
    throw std::runtime_error("[[Enumerable]] is empty");

  return isOnlyFalse(pts);
}

//
// IsNotWritable
//

inline bool IsNotWritable(FieldDescriptor *self) {
  if (auto *t = dynamic_cast<TempFieldDescriptor *>(self))
    return IsNotWritable(t);

  return isOnlyFalse(self->getGraph(), self->getID(),
                     PKRGlobalState::EdgeIntern(PKR_WRITABLE));
}

inline bool IsNotWritable(TempFieldDescriptor *self) {
  auto &pts = self->getWritable();

  if (pts.empty())
    throw std::runtime_error("[[Writable]] is empty");

  return isOnlyFalse(pts);
}

//
// IsAccessorDescriptor
//

inline bool IsAccessorDescriptor(FieldDescriptor *self) {
  if (auto *t = dynamic_cast<TempFieldDescriptor *>(self))
    return IsAccessorDescriptor(t);

  auto setters = self->getGraph()->getAllOutgoingEdgesByLabel(
      self->getID(), PKRGlobalState::EdgeIntern(PKR_Set));

  auto getters = self->getGraph()->getAllOutgoingEdgesByLabel(
      self->getID(), PKRGlobalState::EdgeIntern(PKR_Get));

  return !setters.empty() || !getters.empty();
}

inline bool IsAccessorDescriptor(const TempFieldDescriptor *self) {
  return !self->getGet().empty() || !self->getSet().empty();
}

//
// IsDataDescriptor
//

inline bool IsDataDescriptor(FieldDescriptor *self) {
  return !IsAccessorDescriptor(self);
}

inline bool IsDataDescriptor(TempFieldDescriptor *self) {
  return !IsAccessorDescriptor(self);
}

inline void AllocFieldProxyObject(ECMAGraph *G, NodeUID id) {
  // Declare FOX node
  G->addNode(id, TAG::FOX);
}

} // namespace Prakriti

#include <string>

namespace Prakriti {

namespace OOHelpers {
//
// Local Helpers
//

// Some action closures are meant to be pure, mutation here most likely means
// broken implementation logic, not always enabled
// PRECISION: discarding the branches asserts the queried action is pure.
// Build with PKR_PARANOID to check it.
inline static std::set<NodeUID> pureQuery(const ECMAGraph *G,
                                          const std::vector<NodeUID> &acts,
                                          const ECMAGraph::PJSSL_ARG &args) {
  std::set<NodeUID> res;
  for (auto &r : Karma(G, acts, args)) {
    ASSERT_SLOW(r.clonedG.equals(*G));
    res.insert(r.ret.L.begin(), r.ret.L.end());
  }
  return res;
}

inline static bool isExtensible(ECMAGraph *G, NodeUID ctx) {
  return isOnlyFalse(G, ctx, PKRGlobalState::EdgeIntern(PKR_EXTENSIBLE)) ? false
                                                                         : true;
}

inline static bool isFieldSensitive(ECMAGraph *G, NodeUID ctx) {
  return isOnlyTrue(G, ctx, PKRGlobalState::EdgeIntern(PKR_SENSITIVE));
}

// Every object this node stands for has this property, on every path. Only an
// object literal's own defines can claim it.
inline static bool isDefinite(ECMAGraph *G, NodeUID fp) {
  auto d = G->getPointees(fp, PKRGlobalState::EdgeIntern(PKR_DEFINITE));
  return !d.empty() && isOnlyTrue(d);
}

// A real JS property label: anything not a reserved "[[...]]" slot, plus the
// [[Unknown-Field]] bucket (a reserved name, but a real property).
inline static bool isOwnFieldLabel(std::string_view lbl) {
  return lbl.rfind("[[", 0) != 0 || lbl == PKR_UNKNOWN_FIELD;
}

inline static std::vector<NodeUID> collectOwnFieldFPs(const ECMAGraph *G,
                                                      NodeUID ctx) {
  std::vector<NodeUID> res;
  for (auto &[_, tgt, lab] : G->getAllOutgoingEdges(ctx))
    if (isOwnFieldLabel(PKRGlobalState::EdgeGet(lab)))
      res.push_back(tgt);
  return res;
}

// The FieldProxy node for an own key, or undefined. This is the internal
// lookup Get/Set/Delete want; the [[GetOwnProperty]] action is the spec
// operation and may legitimately answer with two values.
// ~ Meetesh - I had earlier tried to squash both these into the same, but this
// is much less error prone way to handle this query
inline static NodeUID findOwnFP(ECMAGraph *G, NodeUID ctx,
                                const std::string &field) {
  auto fps = G->getPointees(ctx, PKRGlobalState::EdgeIntern(field.c_str()));
  if (fps.empty())
    return PKRGlobalState::getUNDEF();
  ASSERT(fps.size() == 1);
  return fps[0];
}

inline static std::set<NodeUID> getPrototypes(ECMAGraph *G, NodeUID ctx) {
  auto acts =
      G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_GetPrototypeOf));
  return pureQuery(G, acts, {NULL, {ctx}});
}

// Whether the key may exist anywhere on the chain, and whether it must.
// Presence is a may-fact unless [[Definite]] says otherwise, so only `must`
// licenses answering [[HasProperty]] with a single value.
struct Presence {
  bool may = false;
  bool must = false;
};

inline static Presence presence(ECMAGraph *G, NodeUID ctx,
                                const std::string &field,
                                std::set<NodeUID> &visited) {
  Presence p;
  if (ctx == PKRGlobalState::getNULL())
    return p;
  if (!visited.insert(ctx).second)
    return p;

  if (field == PKR_UNKNOWN_FIELD) {
    // Any own field could be the one named, but none of them proves it is.
    if (!collectOwnFieldFPs(G, ctx).empty())
      p.may = true;
  } else {
    NodeUID fp = findOwnFP(G, ctx, field);
    if (fp != PKRGlobalState::getUNDEF()) {
      p.may = true;
      if (isDefinite(G, fp))
        p.must = true;
    }
    // An earlier unknown-keyed write may have targeted this very key.
    if (!isFieldSensitive(G, ctx))
      p.may = true;
  }
  // PRECISION: a definite own property settles it; the chain cannot remove it.
  if (p.must)
    return p;

  // The prototype is itself a may-set, so it only proves presence when every
  // candidate has the key.
  auto protos = getPrototypes(G, ctx);
  bool allMust = !protos.empty();
  for (auto proto : protos) {
    Presence q = presence(G, proto, field, visited);
    p.may |= q.may;
    allMust &= q.must;
  }
  p.must = allMust;
  return p;
}

// Unknown-key case: the real key could be any own field at any level of the
// chain, so (unlike searchFPNodes) never stop early - collect every own
// field FP, at every level, unconditionally.
inline static void collectAllFPsUpChain(ECMAGraph *G, NodeUID ctx,
                                        std::vector<NodeUID> &res,
                                        std::set<NodeUID> &visited) {
  if (ctx == PKRGlobalState::getNULL())
    return;
  if (!visited.insert(ctx).second)
    return;
  auto own = collectOwnFieldFPs(G, ctx);
  res.insert(res.end(), own.begin(), own.end());
  auto pp = G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
  for (auto p : pp)
    collectAllFPsUpChain(G, p, res, visited);
}

// Known-key setter search: the field's FP (and the bucket, if insensitive) at
// every level of the chain. No match stops the walk, see readPrototypes.
inline static void searchFPNodes(ECMAGraph *G, NodeUID ctx,
                                 const std::string &field,
                                 std::vector<NodeUID> &res,
                                 std::set<NodeUID> &visited) {
  if (ctx == PKRGlobalState::getNULL())
    return;
  if (!visited.insert(ctx).second)
    return;

  auto r = G->getPointees(ctx, PKRGlobalState::EdgeIntern(field.c_str()));
  if (!r.empty()) {
    ASSERT(r.size() == 1);
    res.push_back(r[0]);
  }
  if (!isFieldSensitive(G, ctx)) {
    auto rb =
        G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_UNKNOWN_FIELD));
    if (!rb.empty())
      res.push_back(rb[0]);
  }
  for (auto p : G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_PROTOTYPE)))
    searchFPNodes(G, p, field, res, visited);
}

} // namespace OOHelpers

inline bool initOOBJ = []() {
  PKRGlobalState::NAC_OOBJ_GetPrototypeOf = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID ctx = L[0];
    auto protos =
        G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
    ASSERT(!protos.empty());
    return {protos};
  });

  PKRGlobalState::NAC_OOBJ_SetPrototypeOf = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 2);
    NodeUID ctx = L[0];
    NodeUID V = L[1];
    if (!OOHelpers::isExtensible(G, ctx))
      return {{PKRGlobalState::getFALSE()}};
    // Weak: the node may stand for several objects, so the new prototype joins
    // the existing ones instead of replacing them. [[Get]] already walks the
    // whole may-set.
    G->addEdge(ctx, V, PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));

    auto ext = G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_EXTENSIBLE));
    // PRECISION: only a must-extensible receiver makes the result certain.
    if (!ext.empty() && isOnlyTrue(ext))
      return {{PKRGlobalState::getTRUE()}};
    return {{PKRGlobalState::getTRUE(), PKRGlobalState::getFALSE()}};
  });

  PKRGlobalState::NAC_OOBJ_IsExtensible = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID ctx = L[0];
    return {{OOHelpers::isExtensible(G, ctx) ? PKRGlobalState::getTRUE()
                                             : PKRGlobalState::getFALSE()}};
  });

  PKRGlobalState::NAC_OOBJ_PreventExtensions = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID ctx = L[0];
    // Weak: some objects the node stands for may still be extensible, so
    // [[Extensible]] accumulates rather than being replaced.
    G->addEdge(ctx, PKRGlobalState::getFALSE(),
               PKRGlobalState::EdgeIntern(PKR_EXTENSIBLE));
    return {};
  });

  PKRGlobalState::NAC_OOBJ_GetOwnProperty = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;

    ASSERT(L.size() == 1 && A.size() == 1);
    NodeUID ctx = L[0];
    std::string P = A[0];
    auto fps = G->getPointees(ctx, PKRGlobalState::EdgeIntern(P.c_str()));
    if (fps.empty())
      return {{PKRGlobalState::getUNDEF()}};
    // Assertions, We dont expect more than one field proxies at a node...
    ASSERT(fps.size() == 1);
    ASSERT(G->getNodeTAG(fps[0]) == TAG::FOX);
    // PRECISION: the node may stand for objects that lack the key, so the
    // descriptor is the whole answer only when the FieldProxy is [[Definite]].
    if (OOHelpers::isDefinite(G, fps[0]))
      return {fps};
    return {{fps[0], PKRGlobalState::getUNDEF()}};
  });

  PKRGlobalState::NAC_OOBJ_DefineOwnProperty = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;
    auto &X = args.X;

    ASSERT(L.size() == 1 && A.size() == 1 && X.size() == 1);
    NodeUID ctx = L[0];
    std::string field = A[0];
    std::shared_ptr<FieldDescriptor> rhsPtr =
        std::dynamic_pointer_cast<FieldDescriptor>(X[0]);
    ASSERT(rhsPtr);
    NodeUID currentFP = OOHelpers::findOwnFP(G, ctx, field);
    auto isExtensible = OOHelpers::isExtensible(G, ctx);

    if (currentFP == PKRGlobalState::getUNDEF()) {
      // Non-extensible blocks new keys only; an unknown key may still hit an
      // existing own field, so the bucket is refused only when it can't.
      bool mayAliasOwn = field == PKR_UNKNOWN_FIELD &&
                         !OOHelpers::collectOwnFieldFPs(G, ctx).empty();
      if (!isExtensible && !mayAliasOwn)
        return {{PKRGlobalState::getFALSE()}};

      // Brand new own property; FDUnion copies whatever fields rhsPtr set.
      NodeUID id = PKRGlobalState::generateSentinel(
          ctx, PKRGlobalState::EdgeIntern(field));
      AllocFieldProxyObject(G, id);
      G->addEdge(ctx, id, PKRGlobalState::EdgeIntern(field));

      FieldDescriptor newFD(G, id);
      auto *tempNew = dynamic_cast<TempFieldDescriptor *>(rhsPtr.get());
      ASSERT(tempNew);
      FDUnionAccumulateFT(&newFD, tempNew);
    } else {
      auto currentFD = std::make_shared<FieldDescriptor>(G, currentFP);
      if (IsNotConfigurable(currentFD.get())) {
        if (!IsNotConfigurable(rhsPtr.get()))
          return {{PKRGlobalState::getFALSE()}};
        if (IsNotEnumerable(currentFD.get()) != IsNotEnumerable(rhsPtr.get()))
          return {{PKRGlobalState::getFALSE()}};
        if (IsAccessorDescriptor(currentFD.get()) !=
            IsAccessorDescriptor(rhsPtr.get()))
          return {{PKRGlobalState::getFALSE()}};
        if (IsNotWritable(currentFD.get())) {
          if (!IsNotWritable(rhsPtr.get()))
            return {{PKRGlobalState::getFALSE()}};
          if (!SameValue(currentFD.get(), rhsPtr.get()))
            return {{PKRGlobalState::getFALSE()}};
        }
      }
      // An object node may stand for several concrete objects, so every
      // property is a may-location: accumulate, never replace. Strong updates
      // need singleton allocation nodes, which the abstraction does not have.
      auto *tempOther = dynamic_cast<TempFieldDescriptor *>(rhsPtr.get());
      ASSERT(tempOther);
      FDUnionAccumulateFT(currentFD.get(), tempOther);
    }

    // Only a successful bucket write makes ctx insensitive, so the flag and
    // the bucket always exist together.
    if (field == PKR_UNKNOWN_FIELD)
      G->addEdge(ctx, PKRGlobalState::getFALSE(),
                 PKRGlobalState::EdgeIntern(PKR_SENSITIVE));
    return {{PKRGlobalState::getTRUE()}};
  });

  PKRGlobalState::NAC_OOBJ_HasProperty = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;
    auto &X = args.X;

    ASSERT(L.size() == 1 && A.size() == 1);
    NodeUID ctx = L[0];
    std::string field = A[0];

    std::set<NodeUID> seen;
    OOHelpers::Presence pr = OOHelpers::presence(G, ctx, field, seen);
    // PRECISION: `must` is the only thing that rules out the FALSE branch.
    if (pr.must)
      return {{PKRGlobalState::getTRUE()}};
    if (pr.may)
      return {{PKRGlobalState::getTRUE(), PKRGlobalState::getFALSE()}};
    return {{PKRGlobalState::getFALSE()}};
  });

  PKRGlobalState::NAC_OOBJ_Get = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;
    auto &X = args.X;

    ASSERT(L.size() == 2 && A.size() == 1);
    NodeUID ctx = L[0];
    NodeUID rcvr = L[1];
    std::string field = A[0];

    // Which own field proxies this read has to consult.
    std::vector<NodeUID> fps;
    bool ownIsDefinite = false;
    if (field == PKR_UNKNOWN_FIELD) {
      fps = OOHelpers::collectOwnFieldFPs(G, ctx);
    } else {
      NodeUID currentFP = OOHelpers::findOwnFP(G, ctx, field);
      if (currentFP != PKRGlobalState::getUNDEF()) {
        fps.push_back(currentFP);
        ownIsDefinite = OOHelpers::isDefinite(G, currentFP);
      }
      // PRECISION: a must-field-sensitive object has no unknown-keyed writes to
      // account for, so the bucket is skipped.
      if (!OOHelpers::isFieldSensitive(G, ctx)) {
        // An earlier unknown-keyed write may have targeted this very key.
        NodeUID bucketFP = OOHelpers::findOwnFP(G, ctx, PKR_UNKNOWN_FIELD);
        ASSERT(bucketFP != PKRGlobalState::getUNDEF());
        fps.push_back(bucketFP);
      }
    }

    std::vector<NodeUID> res;
    std::vector<ECMAGraph> branches;

    // Data values are a pure read; accessors and prototypes each fork.
    for (auto fp : fps) {
      auto vals = G->getPointees(fp, PKRGlobalState::EdgeIntern(PKR_VALUE));
      res.insert(res.end(), vals.begin(), vals.end());
      auto getters = G->getPointees(fp, PKRGlobalState::EdgeIntern(PKR_Get));
      // 10.1.8.1 step 8: an accessor whose [[Get]] is absent reads as
      // undefined, and it is the only value such a property can produce.
      auto setters = G->getPointees(fp, PKRGlobalState::EdgeIntern(PKR_Set));
      if (getters.empty() && !setters.empty())
        res.push_back(PKRGlobalState::getUNDEF());
      auto call = makeCall({rcvr}, {});
      Karma(G, getters, {NULL, call.L, call.A}, res, branches);
    }

    // PRECISION: OrdinaryGet consults the prototype only when the own property
    // is absent, so [[Definite]] - a must-fact - is what licenses skipping it.
    if (!ownIsDefinite) {
      for (auto &p : OOHelpers::getPrototypes(G, ctx)) {
        if (p == PKRGlobalState::getNULL()) {
          res.push_back(PKRGlobalState::getUNDEF());
          continue;
        }
        auto acts = G->getPointees(p, PKRGlobalState::EdgeIntern(PKR_Get));
        Karma(G, acts, {NULL, {p, rcvr}, {field}}, res, branches);
      }
    }

    KarmaJoin(G, branches);

    ASSERT(!res.empty());
    return {{res.begin(), res.end()}};
  });

  PKRGlobalState::NAC_OOBJ_Set = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;
    ASSERT(args.L.size() == 2 && args.A.size() == 1);

    NodeUID ctx = L[0];
    NodeUID valToSet = L[1];
    std::string field = A[0];

    // Setter search: an unknown key may match any field at any level.
    std::vector<NodeUID> fpNodes;
    std::set<NodeUID> seen;
    if (field == PKR_UNKNOWN_FIELD)
      OOHelpers::collectAllFPsUpChain(G, ctx, fpNodes, seen);
    else
      OOHelpers::searchFPNodes(G, ctx, field, fpNodes, seen);
    // Every setter and the data write are alternatives over the same incoming
    // state, so each forks and the whole operation joins once at the end.
    std::vector<NodeUID> discarded;
    std::vector<ECMAGraph> branches;

    for (auto &fp : fpNodes) {
      FieldDescriptor f(G, fp);
      if (GetAllSetters(&f).empty())
        continue;
      auto acts = G->getPointees(fp, PKRGlobalState::EdgeIntern(PKR_Set));
      // Call(setter, Receiver, [V])
      auto call = makeCall({ctx}, {{valToSet}});
      Karma(G, acts, {NULL, call.L, call.A}, discarded, branches);
    }

    auto currentFP = OOHelpers::findOwnFP(G, ctx, field);

    // PRECISION: 10.1.9.2 step 3 writes an accessor own property through its
    // setter and nothing else. Skipping the data write needs [[Definite]] and
    // no [[Value]] -- an FP that merged accessor and data shapes is ambiguous,
    // so it still gets the write.
    if (currentFP != PKRGlobalState::getUNDEF()) {
      auto accessorFD = FieldDescriptor(G, currentFP);
      if (IsAccessorDescriptor(&accessorFD) && GetValue(&accessorFD).empty() &&
          OOHelpers::isDefinite(G, currentFP)) {
        KarmaJoin(G, branches);
        return {{}};
      }
    }

    // Setters may not exist in every merged branch, so the data write always
    // happens as well.
    auto tmp = std::make_shared<TempFieldDescriptor>();
    tmp->addValue(valToSet);
    if (currentFP == PKRGlobalState::getUNDEF()) {
      tmp->addWritable(PKRGlobalState::getTRUE());
      tmp->addEnumerable(PKRGlobalState::getTRUE());
      tmp->addConfigurable(PKRGlobalState::getTRUE());
    } else {
      auto currentFD = FieldDescriptor(G, currentFP);
      // An accessor descriptor has no [[Writable]] and IsNotWritable asserts on
      // an empty set, so only a data property can answer the question.
      bool hasWritable =
          !G->getPointees(currentFP, PKRGlobalState::EdgeIntern(PKR_WRITABLE))
               .empty();
      tmp->addWritable(!hasWritable || !IsNotWritable(&currentFD)
                           ? PKRGlobalState::getTRUE()
                           : PKRGlobalState::getFALSE());
      tmp->addEnumerable(!IsNotEnumerable(&currentFD)
                             ? PKRGlobalState::getTRUE()
                             : PKRGlobalState::getFALSE());
      tmp->addConfigurable(!IsNotConfigurable(&currentFD)
                               ? PKRGlobalState::getTRUE()
                               : PKRGlobalState::getFALSE());
    }

    auto acts =
        G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_DefineOwnProperty));
    Karma(G, acts, {NULL, {ctx}, {field}, {tmp}}, discarded, branches);

    KarmaJoin(G, branches);
    return {{}};
  });

  PKRGlobalState::NAC_OOBJ_Delete = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;
    auto &X = args.X;
    ASSERT(L.size() == 1 && A.size() == 1);

    NodeUID ctx = L[0];
    std::string field = A[0];

    // A delete on a summary node is weak: leave the FieldProxy in place and
    // record that the property may now be absent. Removing it is a strong
    // update, legal only once allocation nodes are singletons.
    auto markMayBeAbsent = [&](NodeUID fp) {
      G->addEdge(fp, PKRGlobalState::getFALSE(),
                 PKRGlobalState::EdgeIntern(PKR_DEFINITE));
    };

    if (!OOHelpers::isFieldSensitive(G, ctx)) {
      // An unknown key may name any own field, so none of them stays definite
      // - otherwise [[Get]] would keep skipping the prototype walk for a
      // property this delete may have removed.
      for (auto fp : OOHelpers::collectOwnFieldFPs(G, ctx))
        markMayBeAbsent(fp);
      return {{PKRGlobalState::getTRUE(), PKRGlobalState::getFALSE()}};
    }

    NodeUID currentFP = OOHelpers::findOwnFP(G, ctx, field);
    if (currentFP == PKRGlobalState::getUNDEF())
      return {{PKRGlobalState::getTRUE()}};

    FieldDescriptor currentFD(G, currentFP);
    if (IsNotConfigurable(&currentFD))
      return {{PKRGlobalState::getFALSE()}};

    auto cfg =
        G->getPointees(currentFP, PKRGlobalState::EdgeIntern(PKR_CONFIGURABLE));
    bool mustBeConfigurable = !cfg.empty() && isOnlyTrue(cfg);
    markMayBeAbsent(currentFP);
    // PRECISION: the delete can only be reported as certain to succeed when
    // every object the node stands for has a configurable property.
    if (mustBeConfigurable)
      return {{PKRGlobalState::getTRUE()}};
    return {{PKRGlobalState::getTRUE(), PKRGlobalState::getFALSE()}};
  });

  PKRGlobalState::NAC_OOBJ_OwnPropertyKeys = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;
    auto &X = args.X;
    ASSERT(args.L.size() == 1);

    NodeUID ctx = args.L[0];

    // Includes the bucket key: iterating it re-enters the unknown-key paths.
    std::vector<std::string> res;
    for (auto &[_, __, lab] : G->getAllOutgoingEdges(ctx)) {
      auto lblStr = PKRGlobalState::EdgeGet(lab);
      if (OOHelpers::isOwnFieldLabel(lblStr))
        res.push_back(std::string(lblStr));
    }
    return {{}, res};
  });

  return true;
}();

inline void AllocOrdinaryObject(ECMAGraph *G, NodeUID id, NodeUID ext,
                                NodeUID proto) {
  G->addNode(id, TAG::OOBJ);

  NodeUID temp;
  SET_AC(id, NAC_OOBJ_GetPrototypeOf, PKR_GetPrototypeOf);
  SET_AC(id, NAC_OOBJ_SetPrototypeOf, PKR_SetPrototypeOf);
  SET_AC(id, NAC_OOBJ_IsExtensible, PKR_IsExtensible);
  SET_AC(id, NAC_OOBJ_PreventExtensions, PKR_PreventExtensions);
  SET_AC(id, NAC_OOBJ_GetOwnProperty, PKR_GetOwnProperty);
  SET_AC(id, NAC_OOBJ_DefineOwnProperty, PKR_DefineOwnProperty);
  SET_AC(id, NAC_OOBJ_HasProperty, PKR_HasProperty);
  SET_AC(id, NAC_OOBJ_Get, PKR_Get);
  SET_AC(id, NAC_OOBJ_Set, PKR_Set);
  SET_AC(id, NAC_OOBJ_Delete, PKR_Delete);
  SET_AC(id, NAC_OOBJ_OwnPropertyKeys, PKR_OwnPropertyKeys);

  G->addEdge(id, proto, PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
  G->addEdge(id, ext, PKRGlobalState::EdgeIntern(PKR_EXTENSIBLE));
  G->addEdge(id, PKRGlobalState::getTRUE(),
             PKRGlobalState::EdgeIntern(PKR_SENSITIVE));
}
} // namespace Prakriti

#include <cctype>
#include <stdexcept>
#include <string>

namespace Prakriti {

namespace MappedArgsHelpers {
inline bool isNumericIndex(const std::string &field) {
  if (field.empty()) {
    return false;
  }
  return std::all_of(field.begin(), field.end(),
                     [](unsigned char c) { return std::isdigit(c); });
}
} // namespace MappedArgsHelpers

inline bool initARGSOBJ = []() {
  PKRGlobalState::NAC_MARGSOBJ_Unsupported = DEFINE_ACTION() {
    throw std::runtime_error(
        "PKR: MappedArgumentsObject semantics not implemented");
  });

  PKRGlobalState::NAC_MARGSOBJ_Get = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    ASSERT(args.L.size() == 2 && args.A.size() == 1);
    NodeUID ctx = args.L[0];
    const std::string &field = args.A[0];
    ASSERT(MappedArgsHelpers::isNumericIndex(field));
    auto targets =
        G->getPointees(ctx, PKRGlobalState::EdgeIntern(field.c_str()));
    ASSERT(!targets.empty());
    std::vector<NodeUID> res;
    std::vector<ECMAGraph> branches;
    for (NodeUID stackCell : targets) {
      auto acts =
          G->getPointees(stackCell, PKRGlobalState::EdgeIntern(PKR_Get));
      Karma(G, acts, {NULL, {stackCell}}, res, branches);
    }
    KarmaJoin(G, branches);
    return {res};
  });

  PKRGlobalState::NAC_MARGSOBJ_Set = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    ASSERT(args.L.size() == 2 && args.A.size() == 1);
    NodeUID ctx = args.L[0];
    NodeUID val = args.L[1];
    const std::string &field = args.A[0];
    ASSERT(MappedArgsHelpers::isNumericIndex(field));
    auto targets =
        G->getPointees(ctx, PKRGlobalState::EdgeIntern(field.c_str()));
    ASSERT(!targets.empty());
    std::vector<NodeUID> discarded;
    std::vector<ECMAGraph> branches;
    for (NodeUID stackCell : targets) {
      auto acts =
          G->getPointees(stackCell, PKRGlobalState::EdgeIntern(PKR_Set));
      Karma(G, acts, {NULL, {stackCell, val}}, discarded, branches);
    }
    KarmaJoin(G, branches);
    return {};
  });

  return true;
}();

inline void AllocArgumentsObject(ECMAGraph *G, NodeUID id, NodeUID ext,
                                 NodeUID proto) {
  AllocOrdinaryObject(G, id, ext, proto);
  G->addNode(id, TAG::ARGSOBJ);
  G->addEdge(id, PKRGlobalState::getNUMBER(),
             PKRGlobalState::EdgeIntern("length"));
}

inline void AllocMappedArgumentsObject(ECMAGraph *G, NodeUID id, NodeUID ext,
                                       NodeUID proto) {
  AllocOrdinaryObject(G, id, ext, proto);
  G->addNode(id, TAG::MARGSOBJ);
  G->addEdge(id, PKRGlobalState::getNUMBER(),
             PKRGlobalState::EdgeIntern("length"));

  NodeUID temp;
  SET_AC(id, NAC_MARGSOBJ_Get, PKR_Get);
  SET_AC(id, NAC_MARGSOBJ_Set, PKR_Set);
  SET_AC(id, NAC_MARGSOBJ_Unsupported, PKR_GetOwnProperty);
  SET_AC(id, NAC_MARGSOBJ_Unsupported, PKR_DefineOwnProperty);
  SET_AC(id, NAC_MARGSOBJ_Unsupported, PKR_Delete);
}

} // namespace Prakriti

namespace Prakriti {

inline bool initClosure = []() { return true; }();

inline void AllocClosure(ECMAGraph *G, NodeUID id,
                         ECMAGraph::ActionClosure clos, NodeUID ext,
                         NodeUID proto) {
  // Declare FOBJ node
  G->addNode(id, TAG::FOBJ);
  PKRGlobalState::associateActionClosure(id, clos);

  NodeUID temp;
  SET_AC(id, NAC_OOBJ_GetPrototypeOf, PKR_GetPrototypeOf);
  SET_AC(id, NAC_OOBJ_SetPrototypeOf, PKR_SetPrototypeOf);
  SET_AC(id, NAC_OOBJ_IsExtensible, PKR_IsExtensible);
  SET_AC(id, NAC_OOBJ_PreventExtensions, PKR_PreventExtensions);
  SET_AC(id, NAC_OOBJ_GetOwnProperty, PKR_GetOwnProperty);
  SET_AC(id, NAC_OOBJ_DefineOwnProperty, PKR_DefineOwnProperty);
  SET_AC(id, NAC_OOBJ_HasProperty, PKR_HasProperty);
  SET_AC(id, NAC_OOBJ_Get, PKR_Get);
  SET_AC(id, NAC_OOBJ_Set, PKR_Set);
  SET_AC(id, NAC_OOBJ_Delete, PKR_Delete);
  SET_AC(id, NAC_OOBJ_OwnPropertyKeys, PKR_OwnPropertyKeys);

  G->addEdge(id, proto, PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
  G->addEdge(id, ext, PKRGlobalState::EdgeIntern(PKR_EXTENSIBLE));
  G->addEdge(id, PKRGlobalState::getTRUE(),
             PKRGlobalState::EdgeIntern(PKR_SENSITIVE));
}

} // namespace Prakriti

namespace Prakriti {

inline void collectArrayElementValues(ECMAGraph *G, NodeUID arr,
                                      std::vector<NodeUID> &vals,
                                      std::vector<ECMAGraph> &branches) {
  std::set<std::string> keys;
  for (auto &r :
       Karma(G,
             G->getPointees(arr, PKRGlobalState::EdgeIntern(PKR_OwnPropertyKeys)),
             {NULL, {arr}}))
    for (auto &k : r.ret.A)
      if (OOHelpers::isOwnFieldLabel(k) && k != "length")
        keys.insert(k);

  for (const auto &k : keys) {
    NodeUID fp = OOHelpers::findOwnFP(G, arr, k);
    if (fp == PKRGlobalState::getUNDEF())
      continue;
    auto direct = G->getPointees(fp, PKRGlobalState::EdgeIntern(PKR_VALUE));
    vals.insert(vals.end(), direct.begin(), direct.end());
    auto getters = G->getPointees(fp, PKRGlobalState::EdgeIntern(PKR_Get));
    if (!getters.empty()) {
      auto call = makeCall({arr}, {});
      Karma(G, getters, {NULL, call.L, call.A}, vals, branches);
    }
  }
}

// 23.1.5.2 %ArrayIteratorPrototype%.next, uses the PKR_ITERATED node to get the
// context of the object being iterated, gets all its keys,
inline const ECMAGraph::ActionClosure arrayIteratorNextAC = DEFINE_ACTION() {
  ECMAGraph *G = args.G;
  ASSERT(args.A.size() == 2);
  auto thisSlots = decodeArgRanges(args.L, args.A[0]);
  if (thisSlots.empty() || thisSlots[0].empty())
    return {{PKRGlobalState::getUNDEF()}};

  std::set<NodeUID> out;
  std::vector<ECMAGraph> branches;
  std::vector<NodeUID> discardedL;

  for (NodeUID self : thisSlots[0]) {
    NodeUID res = PKRGlobalState::generateSentinel(
        self, PKRGlobalState::EdgeIntern("[[IterResult]]"));
    if (!G->hasNode(res))
      AllocOrdinaryObject(G, res, PKRGlobalState::getTRUE(),
                          PKRGlobalState::getGOOBJ_Object_prototype());
    auto resSet = G->getPointees(res, PKRGlobalState::EdgeIntern(PKR_Set));

    for (NodeUID arr :
         G->getPointees(self, PKRGlobalState::EdgeIntern(PKR_ITERATED))) {
      std::vector<NodeUID> vals;
      std::vector<ECMAGraph> valBranches;
      collectArrayElementValues(G, arr, vals, valBranches);
      KarmaJoin(G, valBranches);

      for (NodeUID v : std::set<NodeUID>(vals.begin(), vals.end()))
        Karma(G, resSet, {NULL, {res, v}, {"value"}}, discardedL, branches);
    }

    Karma(G, resSet, {NULL, {res, PKRGlobalState::getUNDEF()}, {"value"}},
          discardedL, branches);
    Karma(G, resSet, {NULL, {res, PKRGlobalState::getTRUE()}, {"done"}},
          discardedL, branches);
    Karma(G, resSet, {NULL, {res, PKRGlobalState::getFALSE()}, {"done"}},
          discardedL, branches);
    out.insert(res);
  }
  KarmaJoin(G, branches);
  ASSERT(!out.empty());
  return {{out.begin(), out.end()}};
});

// 23.1.3.40 Array.prototype[%Symbol.iterator%] = () => { next() {} }
inline const ECMAGraph::ActionClosure arrayIteratorAC = DEFINE_ACTION() {
  ECMAGraph *G = args.G;
  ASSERT(args.A.size() == 2);
  auto thisSlots = decodeArgRanges(args.L, args.A[0]);
  if (thisSlots.empty() || thisSlots[0].empty())
    return {{PKRGlobalState::getUNDEF()}};

  std::set<NodeUID> out;
  for (NodeUID arr : thisSlots[0]) {
    NodeUID it = PKRGlobalState::generateSentinel(
        arr, PKRGlobalState::EdgeIntern("[[ArrayIterator]]"));
    if (!G->hasNode(it))
      AllocOrdinaryObject(G, it, PKRGlobalState::getTRUE(),
                          PKRGlobalState::getGOOBJ_Object_prototype());

    // Iterator ---[[IteratedObject]]-> arrObj
    //     |
    //     |-------------next---------> arrayIteratorNextAC
    G->addEdge(it, arr, PKRGlobalState::EdgeIntern(PKR_ITERATED));

    NodeUID nextFn = PKRGlobalState::generateSentinel(
        PKRGlobalState::getGOOBJ_Array_prototype(),
        PKRGlobalState::EdgeIntern("[[ArrayIteratorNext]]"));

    if (!G->hasNode(nextFn))
      AllocClosure(G, nextFn, arrayIteratorNextAC, PKRGlobalState::getTRUE(),
                   PKRGlobalState::getGFOBJ_Function_prototype());

    auto fd = std::make_shared<TempFieldDescriptor>();
    fd->addValue(nextFn);
    fd->addWritable(PKRGlobalState::getTRUE());
    fd->addEnumerable(PKRGlobalState::getFALSE());
    fd->addConfigurable(PKRGlobalState::getTRUE());
    // PRECISION: every iterator object this node stands for carries next(),
    // so a lookup stops here instead of walking to Object.prototype.
    fd->addDefinite(PKRGlobalState::getTRUE());
    KarmaJoin(G, Karma(G,
                       G->getPointees(it, PKRGlobalState::EdgeIntern(
                                              PKR_DefineOwnProperty)),
                       {NULL, {it}, {"next"}, {fd}}));
    out.insert(it);
  }
  ASSERT(!out.empty());
  return {{out.begin(), out.end()}};
});

// 23.1.3.23 Array.prototype.push
inline const ECMAGraph::ActionClosure arrayPushAC = DEFINE_ACTION() {
  ECMAGraph *G = args.G;
  ASSERT(args.A.size() == 2);
  auto thisSlots = decodeArgRanges(args.L, args.A[0]);
  auto argSlots = decodeArgRanges(args.L, args.A[1]);
  if (thisSlots.empty() || thisSlots[0].empty())
    return {{PKRGlobalState::getNUMBER()}};

  std::vector<NodeUID> discarded;
  std::vector<ECMAGraph> branches;
  for (NodeUID recv : thisSlots[0]) {
    auto setActs = G->getPointees(recv, PKRGlobalState::EdgeIntern(PKR_Set));
    if (setActs.empty())
      continue;
    for (const auto &slot : argSlots)
      for (NodeUID v : slot)
        Karma(G, setActs, {NULL, {recv, v}, {PKR_UNKNOWN_FIELD}}, discarded,
              branches);
  }
  KarmaJoin(G, branches);
  return {{PKRGlobalState::getNUMBER()}};
});

// 23.1.3.22 Array.prototype.pop
inline const ECMAGraph::ActionClosure arrayPopAC = DEFINE_ACTION() {
  ECMAGraph *G = args.G;
  ASSERT(args.A.size() == 2);
  auto thisSlots = decodeArgRanges(args.L, args.A[0]);

  std::set<NodeUID> out{PKRGlobalState::getUNDEF()};
  std::vector<ECMAGraph> branches;
  if (!thisSlots.empty()) {
    for (NodeUID recv : thisSlots[0]) {
      std::vector<NodeUID> vals;
      collectArrayElementValues(G, recv, vals, branches);
      out.insert(vals.begin(), vals.end());
    }
  }
  KarmaJoin(G, branches);
  return {{out.begin(), out.end()}};
});

inline void AllocArrayObject(ECMAGraph *G, NodeUID id) {
  G->addNode(id, TAG::ARRAYOBJ);

  NodeUID temp;
  SET_AC(id, NAC_OOBJ_GetPrototypeOf, PKR_GetPrototypeOf);
  SET_AC(id, NAC_OOBJ_SetPrototypeOf, PKR_SetPrototypeOf);
  SET_AC(id, NAC_OOBJ_IsExtensible, PKR_IsExtensible);
  SET_AC(id, NAC_OOBJ_PreventExtensions, PKR_PreventExtensions);
  SET_AC(id, NAC_OOBJ_GetOwnProperty, PKR_GetOwnProperty);
  SET_AC(id, NAC_OOBJ_DefineOwnProperty, PKR_DefineOwnProperty);
  SET_AC(id, NAC_OOBJ_HasProperty, PKR_HasProperty);
  SET_AC(id, NAC_OOBJ_Get, PKR_Get);
  SET_AC(id, NAC_OOBJ_Set, PKR_Set);
  SET_AC(id, NAC_OOBJ_Delete, PKR_Delete);
  SET_AC(id, NAC_OOBJ_OwnPropertyKeys, PKR_OwnPropertyKeys);

  G->addEdge(id, PKRGlobalState::getTRUE(),
             PKRGlobalState::EdgeIntern(PKR_EXTENSIBLE));
  G->addEdge(id, PKRGlobalState::getGOOBJ_Array_prototype(),
             PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
  G->addEdge(id, PKRGlobalState::getTRUE(),
             PKRGlobalState::EdgeIntern(PKR_SENSITIVE));

  NodeUID lengthFP = PKRGlobalState::generateSentinel(
      id, PKRGlobalState::EdgeIntern("length"));
  AllocFieldProxyObject(G, lengthFP);
  G->addEdge(id, lengthFP, PKRGlobalState::EdgeIntern("length"));
  G->addEdge(lengthFP, PKRGlobalState::getNUMBER(),
             PKRGlobalState::EdgeIntern(PKR_VALUE));
  G->addEdge(lengthFP, PKRGlobalState::getTRUE(),
             PKRGlobalState::EdgeIntern(PKR_WRITABLE));
  G->addEdge(lengthFP, PKRGlobalState::getFALSE(),
             PKRGlobalState::EdgeIntern(PKR_ENUMERABLE));
  G->addEdge(lengthFP, PKRGlobalState::getFALSE(),
             PKRGlobalState::EdgeIntern(PKR_CONFIGURABLE));
}

} // namespace Prakriti

namespace Prakriti {

inline bool initAwait = []() {
  PKRGlobalState::NAC_Await_Eval = DEFINE_ACTION() {
    ASSERT(false);
    // WIP ~ Meetesh

    // ECMAGraph *G = args.G;
    // const auto &L = args.L;
    //
    // ASSERT(L.size() > 0);
    // NodeUID ctx = L[0];
    //
    // auto storeTargets = G->getAllOutgoingEdgesByLabel(
    //     ctx, PKRGlobalState::EdgeIntern(PKR_StoreTarget));
    //
    // for (const auto &st : storeTargets) {
    //   std::vector<NodeUID> L_(L.begin(), L.end());
    //   L_[0] = st.target;
    //
    //   KarmaJoin(G, Karma(G,
    //             G->getPointees(st.target,
    //             PKRGlobalState::EdgeIntern(PKR_Set)), {NULL, L_}));
    // }
    //
    // auto savedStateVec = G->getAllOutgoingEdgesByLabel(
    //     ctx, PKRGlobalState::EdgeIntern(PKR_State));
    // ASSERT(savedStateVec.size() == 1);
    //
    // auto savedState = savedStateVec.at(0).target;
    // ASSERT(G->getNodeTAG(savedState) == TAG::STATE_VAL);
    //
    // // Call Registered Action Closure
    // invokeAction(ctx, {G, {savedState}});

    return {};
  });

  return true;
}();

inline void AllocAwaitNode(ECMAGraph *G, NodeUID id,
                           ECMAGraph::ActionClosure clos, NodeUID stateNode) {
  // Declare Await Node
  G->addNode(id, TAG::AWAIT);
  PKRGlobalState::associateActionClosure(id, clos);

  // Set State
  G->addEdge(id, stateNode, PKRGlobalState::EdgeIntern(PKR_State));

  NodeUID temp;
  SET_AC(id, NAC_Await_Eval, PKR_Eval);
}

inline void SetAwaitStoreTarget(ECMAGraph *G, NodeUID awaitID,
                                NodeUID storeTarget) {
  // Remove Old StoreTargets
  G->removeAllOutgoingEdgesByLabel(awaitID,
                                   PKRGlobalState::EdgeIntern(PKR_StoreTarget));
  // Set new StoreTarget
  G->addEdge(awaitID, storeTarget, PKRGlobalState::EdgeIntern(PKR_StoreTarget));
}

} // namespace Prakriti

#include <set>

namespace Prakriti {

inline bool isObjectNode(const ECMAGraph *G, NodeUID v) {
  switch (G->getNodeTAG(v)) {
  case TAG::OOBJ:
  case TAG::FOBJ:
  case TAG::ARRAYOBJ:
  case TAG::STROBJ:
  case TAG::ARGSOBJ:
  case TAG::MARGSOBJ:
    return true;
  default:
    return false;
  }
}

// ECMA-262 7.1.4 ToNumber
inline bool initBinaryOperators = []() {
  PKRGlobalState::NAC_ToNumber = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID v = L[0];
    switch (G->getNodeTAG(v)) {
    case TAG::UNDEF_VAL:
      return {{PKRGlobalState::getNAN()}};
    case TAG::STRING_VAL:
      return {{PKRGlobalState::getNUMBER(), PKRGlobalState::getNAN()}};
    default:
      // 7.1.4 sends an object through ToPrimitive first. Nothing reaches here
      // with one today, and returning NUMBER would drop that coercion's
      // effects, so refuse instead of answering wrongly.
      if (isObjectNode(G, v))
        throw std::runtime_error(
            "PKR: ToNumber on an object requires ToPrimitive");
      return {{PKRGlobalState::getNUMBER()}};
    }
  });

  // ECMA-262 7.1.1.1 OrdinaryToPrimitive
  PKRGlobalState::NAC_OrdinaryToPrimitive = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID ctx = L[0];

    // Looking the two methods up are alternatives, so they fork.
    auto getActs = G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_Get));
    std::vector<NodeUID> funs1, funs2;
    std::vector<ECMAGraph> lookups;
    Karma(G, getActs, {NULL, {ctx, ctx}, {"toString"}}, funs1, lookups);
    Karma(G, getActs, {NULL, {ctx, ctx}, {"valueOf"}}, funs2, lookups);
    KarmaJoin(G, lookups);

    std::set<NodeUID> toStringSet(funs1.begin(), funs1.end());
    std::set<NodeUID> valueOfSet(funs2.begin(), funs2.end());
    std::vector<NodeUID> toStringFns, valueOfFns;
    for (NodeUID f : toStringSet)
      if (PKRGlobalState::nodeHasActionClosure(f))
        toStringFns.push_back(f);
    for (NodeUID f : valueOfSet)
      if (PKRGlobalState::nodeHasActionClosure(f))
        valueOfFns.push_back(f);

    // I am unsure about that to do here, for a later day ~ Meetesh
    ASSERT(!toStringFns.empty() || !valueOfFns.empty());

    // 7.1.1.1 tries the methods in order, so within one ordering the second
    // call must observe the first's effects - this threading is deliberate.
    // Both orderings are explored as branches and joined.
    auto step = [&](ECMAGraph &H, const std::vector<NodeUID> &fns,
                    std::vector<NodeUID> &out) {
      std::vector<ECMAGraph> branches;
      // Call(method, O)
      auto call = makeCall({ctx}, {});
      Karma(&H, fns, {NULL, call.L, call.A}, out, branches);
      KarmaJoin(&H, branches);
    };

    std::vector<NodeUID> resA, resB;
    ECMAGraph GA = G->clone();
    step(GA, toStringFns, resA);
    step(GA, valueOfFns, resA);

    ECMAGraph GB = G->clone();
    step(GB, valueOfFns, resB);
    step(GB, toStringFns, resB);

    KarmaJoin(G, std::vector<ECMAGraph>{GA, GB});

    std::set<NodeUID> allResults(resA.begin(), resA.end());
    allResults.insert(resB.begin(), resB.end());

    std::vector<NodeUID> finRes;
    for (NodeUID r : allResults)
      if (!isObjectNode(G, r))
        finRes.push_back(r);

    ASSERT(!finRes.empty());

    return {{finRes.begin(), finRes.end()}};
  });

  // ECMA-262 7.1.1 ToPrimitive
  PKRGlobalState::NAC_ToPrimitive = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID val = L[0];

    if (!isObjectNode(G, val))
      return {{val}};

    auto getActs = G->getPointees(val, PKRGlobalState::EdgeIntern(PKR_Get));
    std::vector<NodeUID> funcVec;
    std::vector<ECMAGraph> lookupBranches;
    Karma(G, getActs, {NULL, {val, val}, {PKR_SYMBOL_LABEL(toPrimitive)}}, funcVec,
          lookupBranches);
    KarmaJoin(G, lookupBranches);
    std::set<NodeUID> funcs(funcVec.begin(), funcVec.end());

    bool sawUndefined = false;
    std::vector<NodeUID> callable;
    for (NodeUID f : funcs) {
      if (f == PKRGlobalState::getUNDEF())
        sawUndefined = true;
      else if (PKRGlobalState::nodeHasActionClosure(f))
        callable.push_back(f);
      // A non-callable @@toPrimitive is a TypeError (7.1.1 via GetMethod), so
      // that path has no normal completion and contributes no value - drop it
      // rather than refuse the whole analysis. It is reached by any object with
      // an [[Unknown-Field]] bucket, since a bucket answers every key.
    }

    // The lookup may be undefined on one path and a method on another. Both
    // readings are live, so each forks and the results union.
    std::set<NodeUID> results;
    std::vector<ECMAGraph> branches;

    if (!callable.empty()) {
      ECMAGraph H = G->clone();
      std::vector<NodeUID> r;
      std::vector<ECMAGraph> callBranches;
      // Call(exoticToPrim, input, [hint]) - the hint is not modelled.
      auto call = makeCall({val}, {});
      Karma(&H, callable, {NULL, call.L, call.A}, r, callBranches);
      KarmaJoin(&H, callBranches);
      results.insert(r.begin(), r.end());
      branches.push_back(std::move(H));
    }

    if (sawUndefined || callable.empty()) {
      ECMAGraph H = G->clone();
      NodeUID otpAct = PKRGlobalState::getActionNode(
          PKRGlobalState::NAC_OrdinaryToPrimitive);
      auto r = invokeAction(otpAct, {&H, {val}});
      results.insert(r.L.begin(), r.L.end());
      branches.push_back(std::move(H));
    }

    KarmaJoin(G, branches);
    ASSERT(!results.empty());
    return {{results.begin(), results.end()}};
  });

  // ECMA-262 7.1.17 ToString
  PKRGlobalState::NAC_ToString = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID ctx = L[0];

    NodeUID tpAct =
        PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToPrimitive);
    invokeAction(tpAct, {G, {ctx}});

    return {{PKRGlobalState::getSTRING()}};
  });

  // ECMA-262 7.1.3 ToNumeric
  PKRGlobalState::NAC_ToNumeric = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID ctx = L[0];

    NodeUID tpAct =
        PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToPrimitive);
    auto primRes = invokeAction(tpAct, {G, {ctx}});
    std::set<NodeUID> vals(primRes.L.begin(), primRes.L.end());

    bool allBigInt = true, allNumber = true;
    for (NodeUID v : vals) {
      TAG t = G->getNodeTAG(v);
      allBigInt &= (t == TAG::BIGINT_VAL);
      allNumber &= (t == TAG::NUMBER_VAL);
    }

    if (allBigInt)
      return {{PKRGlobalState::getBIGINT()}};
    if (allNumber)
      return {{PKRGlobalState::getNUMBER()}};

    NodeUID tnAct = PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToNumber);
    std::set<NodeUID> res;
    for (NodeUID v : vals) {
      auto r = invokeAction(tnAct, {G, {v}});
      res.insert(r.L.begin(), r.L.end());
    }

    ASSERT(!res.empty());
    return {{res.begin(), res.end()}};
  });

  // ECMA-262 13.15.3 ApplyStringOrNumericBinaryOperator
  PKRGlobalState::NAC_HandleBinop = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;

    ASSERT(L.size() == 2 && A.size() == 1);
    NodeUID lval = L[0];
    NodeUID rval = L[1];
    std::string op = A[0];

    const std::set<std::string> validOps = {"**", "*",  "/",   "%", "+", "-",
                                            "<<", ">>", ">>>", "&", "^", "|"};
    ASSERT(validOps.count(op) > 0);

    std::set<NodeUID> lvalSet = {lval};
    std::set<NodeUID> rvalSet = {rval};
    std::vector<ECMAGraph> stringBranches, numBranches;

    if (op == "+") {
      NodeUID tpAct =
          PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToPrimitive);
      auto vv1Ret = invokeAction(tpAct, {G, {lval}});
      std::set<NodeUID> vv1(vv1Ret.L.begin(), vv1Ret.L.end());
      auto vv2Ret = invokeAction(tpAct, {G, {rval}});
      std::set<NodeUID> vv2(vv2Ret.L.begin(), vv2Ret.L.end());

      // 13.15.3 step 2 concatenates when *either* operand is a String, so the
      // question is per side: one side that must be a String settles it, whoever
      // the other side is. Asking whether every value on both sides is a String
      // would be far too strong - "a" + 1 would keep a spurious numeric reading.
      auto stringness = [&](const std::set<NodeUID> &vs, bool &may, bool &must) {
        must = !vs.empty();
        for (NodeUID v : vs) {
          bool isStr = G->getNodeTAG(v) == TAG::STRING_VAL;
          may |= isStr;
          must &= isStr;
        }
      };
      bool lMay = false, lMust = false, rMay = false, rMust = false;
      stringness(vv1, lMay, lMust);
      stringness(vv2, rMay, rMust);
      bool anyString = lMay || rMay;
      bool mustString = lMust || rMust;

      if (anyString) {
        NodeUID tsAct =
            PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToString);
        // The string and numeric readings are alternatives over the same
        // incoming state, so the coercions fork and join once.
        std::vector<ECMAGraph> strBranches;
        ECMAGraph GS = G->clone();
        for (NodeUID v : vv1)
          invokeAction(tsAct, {&GS, {v}}); // side effects only
        for (NodeUID v : vv2)
          invokeAction(tsAct, {&GS, {v}}); // side effects only
        strBranches.push_back(std::move(GS));
        // PRECISION: one side that must be a String excludes the numeric
        // reading outright.
        if (mustString) {
          KarmaJoin(G, strBranches);
          return {{PKRGlobalState::getSTRING()}};
        }
        stringBranches = std::move(strBranches);
      }

      lvalSet = vv1;
      rvalSet = vv2;
    }

    NodeUID tnumAct =
        PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToNumeric);
    // Each operand value is an alternative, so each coercion reads the same
    // incoming state; threading G would let one observe another's effects.
    std::set<NodeUID> v1, v2;
    for (NodeUID v : lvalSet) {
      ECMAGraph H = G->clone();
      auto r = invokeAction(tnumAct, {&H, {v}});
      v1.insert(r.L.begin(), r.L.end());
      numBranches.push_back(std::move(H));
    }
    for (NodeUID v : rvalSet) {
      ECMAGraph H = G->clone();
      auto r = invokeAction(tnumAct, {&H, {v}});
      v2.insert(r.L.begin(), r.L.end());
      numBranches.push_back(std::move(H));
    }

    {
      std::vector<ECMAGraph> all;
      all.reserve(stringBranches.size() + numBranches.size());
      for (auto &b : stringBranches)
        all.push_back(std::move(b));
      for (auto &b : numBranches)
        all.push_back(std::move(b));
      KarmaJoin(G, all);
    }

    std::set<NodeUID> res;
    // A may-String operand keeps the concatenation reading alive beside the
    // numeric one; allString returned above.
    if (!stringBranches.empty())
      res.insert(PKRGlobalState::getSTRING());

    // NaN carries its own tag but is a Number, so both count as numeric.
    auto allNumeric = [&](const std::set<NodeUID> &vs, bool bigint) {
      if (vs.empty())
        return false;
      for (NodeUID v : vs) {
        TAG t = G->getNodeTAG(v);
        bool ok = bigint ? (t == TAG::BIGINT_VAL)
                         : (t == TAG::NUMBER_VAL || t == TAG::NAN_VAL);
        if (!ok)
          return false;
      }
      return true;
    };

    // PRECISION: 13.15.3 step 4 coerces both operands to the same numeric type,
    // so one type can be dropped - but only when every value on both sides must
    // have it. A mixed set keeps both.
    if (allNumeric(v1, true) && allNumeric(v2, true)) {
      res.insert(PKRGlobalState::getBIGINT());
      return {{res.begin(), res.end()}};
    }
    if (allNumeric(v1, false) && allNumeric(v2, false)) {
      res.insert(PKRGlobalState::getNUMBER());
      return {{res.begin(), res.end()}};
    }

    // Over-approximated
    res.insert(PKRGlobalState::getNUMBER());
    res.insert(PKRGlobalState::getBIGINT());
    return {{res.begin(), res.end()}};
  });

  // ECMA-262 13.10
  PKRGlobalState::NAC_HandleRelop = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;

    ASSERT(L.size() == 2 && A.size() == 1);
    NodeUID lval = L[0];
    NodeUID rval = L[1];
    std::string op = A[0];

    const std::set<std::string> validOps = {"<", ">", "<=", ">="};
    ASSERT(validOps.count(op) > 0);

    // ToPrimitive is called for its side effects only (left first, then
    // right); the result is always a Boolean regardless of operand types.
    NodeUID tpAct =
        PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToPrimitive);
    invokeAction(tpAct, {G, {lval}});
    invokeAction(tpAct, {G, {rval}});

    return {{PKRGlobalState::getTRUE(), PKRGlobalState::getFALSE()}};
  });

  // 7.3.21 OrdinaryHasInstance
  PKRGlobalState::NAC_OrdinaryHasInstance = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    ASSERT(L.size() == 2);
    NodeUID ctor = L[0];
    NodeUID instance = L[1];

    if (!PKRGlobalState::nodeHasActionClosure(ctor))
      return {{PKRGlobalState::getFALSE()}};

    if (!isObjectNode(G, instance))
      return {{PKRGlobalState::getFALSE()}};

    std::vector<NodeUID> protoVals;
    std::vector<ECMAGraph> branches;
    auto getActs = G->getPointees(ctor, PKRGlobalState::EdgeIntern(PKR_Get));
    Karma(G, getActs, {NULL, {ctor, ctor}, {"prototype"}}, protoVals, branches);
    KarmaJoin(G, branches);

    std::set<NodeUID> protos;
    for (NodeUID p : protoVals)
      if (isObjectNode(G, p))
        protos.insert(p);
    if (protos.empty())
      return {{PKRGlobalState::getTRUE(), PKRGlobalState::getFALSE()}};

    std::set<NodeUID> chain;
    std::vector<NodeUID> work{instance};
    while (!work.empty()) {
      NodeUID cur = work.back();
      work.pop_back();
      if (cur == PKRGlobalState::getNULL())
        continue;
      auto acts =
          G->getPointees(cur, PKRGlobalState::EdgeIntern(PKR_GetPrototypeOf));
      if (acts.empty())
        continue;
      std::vector<NodeUID> out;
      std::vector<ECMAGraph> pb;
      Karma(G, acts, {NULL, {cur}}, out, pb);
      KarmaJoin(G, pb);
      for (NodeUID p : out)
        // The visited set doubles as the cycle guard.
        if (p != PKRGlobalState::getNULL() && chain.insert(p).second)
          work.push_back(p);
    }

    // PRECISION: distinct nodes never denote the same concrete object and the
    // chain is a complete may-superset, so no overlap means no concrete match.
    // Proving TRUE alone would need singleton allocation nodes.
    for (NodeUID p : protos)
      if (chain.count(p))
        return {{PKRGlobalState::getTRUE(), PKRGlobalState::getFALSE()}};
    return {{PKRGlobalState::getFALSE()}};
  });

  // ECMA-262 7.2.13, 7.2.16, 13.10.1
  PKRGlobalState::NAC_HandleJSBinop = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;

    ASSERT(L.size() == 2 && A.size() == 1);
    NodeUID lval = L[0];
    NodeUID rval = L[1];
    std::string op = A[0];

    const std::set<std::string> validOps = {
        "==", "!=", "===", "!==", "in", "instanceof"};
    ASSERT(validOps.count(op) > 0);

    NodeUID tpAct =
        PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToPrimitive);

    if (op == "==" || op == "!=") {
      // Steps 11-12 coerce exactly one side, and only when the other is
      // already primitive. Two objects take step 1 and compare by identity,
      // so they never reach valueOf. ToNumber, in steps 5-10, only ever sees
      // a String or a Boolean and is pure.
      bool lIsObj = isObjectNode(G, lval);
      bool rIsObj = isObjectNode(G, rval);
      if (lIsObj && !rIsObj)
        invokeAction(tpAct, {G, {lval}});
      else if (rIsObj && !lIsObj)
        invokeAction(tpAct, {G, {rval}});
    } else if (op == "in") {
      // ToPropertyKey on the left; [[HasProperty]] runs no user code.
      invokeAction(tpAct, {G, {lval}});
    } else if (op == "instanceof") {
      // 13.10.2: v instanceof F === F[%Symbol.hasInstance%](v).
      std::vector<NodeUID> handlers;
      std::vector<ECMAGraph> lookupBranches;
      auto getActs = G->getPointees(rval, PKRGlobalState::EdgeIntern(PKR_Get));
      Karma(G, getActs, {NULL, {rval, rval}, {PKR_SYMBOL_LABEL(hasInstance)}}, handlers,
            lookupBranches);
      KarmaJoin(G, lookupBranches);

      std::vector<NodeUID> callable;
      for (NodeUID h : std::set<NodeUID>(handlers.begin(), handlers.end()))
        if (PKRGlobalState::nodeHasActionClosure(h))
          callable.push_back(h);

      if (callable.empty())
        return {{PKRGlobalState::getTRUE(), PKRGlobalState::getFALSE()}};

      std::vector<NodeUID> resultVec;
      std::vector<ECMAGraph> callBranches;
      auto call = makeCall({rval}, {{lval}});
      Karma(G, callable, {NULL, call.L, call.A}, resultVec, callBranches);
      KarmaJoin(G, callBranches);
      std::set<NodeUID> results(resultVec.begin(), resultVec.end());

      // 7.1.2 ToBoolean. PRECISION: an all-true or all-false result settles it,
      // which is what carries OrdinaryHasInstance's definite FALSE through to
      // the caller. Anything else falls to boolean top below.
      if (!results.empty() && isOnlyTrue(results))
        return {{PKRGlobalState::getTRUE()}};
      if (!results.empty() && isOnlyFalse(results))
        return {{PKRGlobalState::getFALSE()}};
    }
    // "===" and "!==" do not coerce at all.

    return {{PKRGlobalState::getTRUE(), PKRGlobalState::getFALSE()}};
  });

  return true;
}();

} // namespace Prakriti

// A constructor body that just asserts if actually called.
#define PKR_STUB_FUN                                                           \
  DEFINE_ACTION() {                                                            \
    ASSERT(false);                                                             \
    return {};                                                                 \
  })

// Builds constructor FOBJ `id` and sets its .prototype to `protoField`.
#define ALLOC_CTR(id, func, protoField)                                        \
  AllocClosure(G, id, func, PKRGlobalState::getTRUE(),                         \
               PKRGlobalState::getGFOBJ_Function_prototype());                 \
  KarmaJoin(                                                                   \
      G, Karma(G, G->getPointees(id, PKRGlobalState::EdgeIntern(PKR_Set)),      \
               {NULL, {id, protoField}, {"prototype"}}));

namespace Prakriti {

inline void defineStubMethod(ECMAGraph *G, NodeUID target,
                             const std::string &propName,
                             const std::string &qualifiedName) {
  NodeUID methodID = PKRGlobalState::ReserveNodeUID();
  auto ac = makeTracedAction(
      __FILE__, __LINE__,
      [qualifiedName](const ECMAGraph::PJSSL_ARG &) -> ECMAGraph::PJSSL_RET {
        throw std::runtime_error(
            "[Prakriti] Not implemented: " + qualifiedName + "()");
      });
  AllocClosure(G, methodID, ac, PKRGlobalState::getTRUE(),
               PKRGlobalState::getGFOBJ_Function_prototype());
  KarmaJoin(G,
            Karma(G,
                  G->getPointees(target, PKRGlobalState::EdgeIntern(PKR_Set)),
                  {NULL, {target, methodID}, {propName}}));
}

inline void defineStubMethods(ECMAGraph *G, NodeUID target,
                              const std::string &qualifiedPrefix,
                              std::initializer_list<const char *> names) {
  for (const char *name : names)
    defineStubMethod(G, target, name, qualifiedPrefix + name);
}

inline void defineNoopMethod(ECMAGraph *G, NodeUID target,
                             const std::string &propName) {
  NodeUID methodID = PKRGlobalState::ReserveNodeUID();
  auto ac = DEFINE_ACTION() { return {{PKRGlobalState::getUNDEF()}}; });
  AllocClosure(G, methodID, ac, PKRGlobalState::getTRUE(),
               PKRGlobalState::getGFOBJ_Function_prototype());
  KarmaJoin(G,
            Karma(G,
                  G->getPointees(target, PKRGlobalState::EdgeIntern(PKR_Set)),
                  {NULL, {target, methodID}, {propName}}));
}

inline void defineNoopMethods(ECMAGraph *G, NodeUID target,
                              std::initializer_list<const char *> names) {
  for (const char *name : names)
    defineNoopMethod(G, target, name);
}

inline void linkPrototypeConstructor(ECMAGraph *G, NodeUID ctorID,
                                     NodeUID protoID) {
  KarmaJoin(G,
            Karma(G,
                  G->getPointees(protoID, PKRGlobalState::EdgeIntern(PKR_Set)),
                  {NULL, {protoID, ctorID}, {"constructor"}}));
}

// `ctorAC` is the constructor's own body. Pass PKR_STUB_FUN for one that is not
// modelled yet - explicit at the call site, so an implemented constructor is
// visible rather than hidden behind a default.
inline void
defineStubIntrinsic(ECMAGraph *G, NodeUID ctorID, NodeUID protoID,
                    NodeUID protoParent,
                    std::initializer_list<const char *> staticMethodNames,
                    std::initializer_list<const char *> protoMethodNames,
                    const std::string &name,
                    ECMAGraph::ActionClosure ctorAC) {
  AllocOrdinaryObject(G, protoID, PKRGlobalState::getTRUE(), protoParent);
  ALLOC_CTR(ctorID, ctorAC, protoID);
  linkPrototypeConstructor(G, ctorID, protoID);
  defineStubMethods(G, ctorID, name + ".", staticMethodNames);
  defineStubMethods(G, protoID, name + ".prototype.", protoMethodNames);
}

} // namespace Prakriti

namespace Prakriti {

inline bool initSOBJ = []() {
  PKRGlobalState::NAC_SOBJ_Set = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() >= 2);
    NodeUID ctx = L[0];
    // PRECISION: an uncaptured cell is unreachable outside its frame, so the
    // strong update is always legal. ptaInfo() is what establishes that.
    G->removeAllOutgoingEdgesByLabel(ctx, PKRGlobalState::EdgeIntern(PKR_STK));
    for (auto i = 1; i < L.size(); i++) {
      G->addEdge(ctx, L[i], PKRGlobalState::EdgeIntern(PKR_STK));
    }

    return {};
  });

  PKRGlobalState::NAC_SOBJ_Get = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID ctx = args.L[0];
    return {G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_STK))};
  });

  return true;
}();

inline void AllocStackObject(ECMAGraph *G, NodeUID id) {
  G->addNode(id, TAG::STKOBJ);
  NodeUID temp;
  SET_AC(id, NAC_SOBJ_Set, PKR_Set);
  SET_AC(id, NAC_SOBJ_Get, PKR_Get);
}
} // namespace Prakriti

#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>

#define ALLOC_STKN(name) AllocStackObject(G, PKRGlobalState::getGlobal(name))
#define GSTK_BIND(src, dest)                                                   \
  KarmaJoin(G, Karma(G,                                                        \
                     G->getPointees(PKRGlobalState::getGlobal(src),            \
                                    PKRGlobalState::EdgeIntern(PKR_Set)),      \
                     {NULL, {PKRGlobalState::getGlobal(src), dest}}));

namespace Prakriti {

// Name -> lazy-init-routine table for an environment's named globals.
using GlobalInitTable =
    std::vector<std::pair<const char *, void (*)(ECMAGraph *)>>;

inline GlobalInitTable composeGlobalInitializers(
    std::initializer_list<const GlobalInitTable *> parents) {
  GlobalInitTable out;
  for (const auto *parent : parents)
    out.insert(out.end(), parent->begin(), parent->end());
  return out;
}

// Reserves a stack binding for every named global the environment provides.
inline void declareGlobals(const GlobalInitTable &registry) {
  for (const auto &[gstkName, initFn] : registry)
    PKRGlobalState::declareGlobal(PKRGlobalState::EdgeIntern(gstkName));
}

// Binds `name` using the matching entry in `registry`, throws if unknown.
inline void initNamedGlobal(ECMAGraph *G, EdgeUID name,
                            const GlobalInitTable &registry) {
  for (const auto &[gstkName, initFn] : registry) {
    if (name == PKRGlobalState::EdgeIntern(gstkName)) {
      initFn(G);
      return;
    }
  }
  throw std::runtime_error("Unknown Global: " +
                           std::string(PKRGlobalState::EdgeGet(name)));
}

inline void initGSTK_globalThis(ECMAGraph *G) { ALLOC_STKN(GSTK_globalThis); }

inline void initGSTK_Infinity(ECMAGraph *G) {
  ALLOC_STKN(GSTK_Infinity);
  G->addEdge(PKRGlobalState::getGlobal(GSTK_Infinity), PKRGlobalState::getINF(),
             PKRGlobalState::EdgeIntern(PKR_STK));
}

inline void initGSTK_NaN(ECMAGraph *G) {
  ALLOC_STKN(GSTK_NaN);
  G->addEdge(PKRGlobalState::getGlobal(GSTK_NaN), PKRGlobalState::getNAN(),
             PKRGlobalState::EdgeIntern(PKR_STK));
}

inline void initGSTK_undefined(ECMAGraph *G) {
  ALLOC_STKN(GSTK_undefined);
  G->addEdge(PKRGlobalState::getGlobal(GSTK_undefined),
             PKRGlobalState::getUNDEF(), PKRGlobalState::EdgeIntern(PKR_STK));
}

inline void initGSTK_Function(ECMAGraph *G) {
  ALLOC_STKN(GSTK_Function);
  GSTK_BIND(GSTK_Function, PKRGlobalState::getGFOBJ_Function());
}

inline void initGSTK_Boolean(ECMAGraph *G) {
  ALLOC_STKN(GSTK_Boolean);
  GSTK_BIND(GSTK_Boolean, PKRGlobalState::getGFOBJ_Boolean());
}

inline void initGSTK_Symbol(ECMAGraph *G) {
  ALLOC_STKN(GSTK_Symbol);
  GSTK_BIND(GSTK_Symbol, PKRGlobalState::getGFOBJ_Symbol());
}

inline void initGSTK_Error(ECMAGraph *G) {
  ALLOC_STKN(GSTK_Error);
  GSTK_BIND(GSTK_Error, PKRGlobalState::getGFOBJ_Error());
}

inline void initGSTK_Object(ECMAGraph *G) {
  ALLOC_STKN(GSTK_Object);
  GSTK_BIND(GSTK_Object, PKRGlobalState::getGFOBJ_Object());
}

inline void initGSTK_Array(ECMAGraph *G) {
  ALLOC_STKN(GSTK_Array);
  GSTK_BIND(GSTK_Array, PKRGlobalState::getGFOBJ_Array());
}

inline void initGSTK_String(ECMAGraph *G) {
  ALLOC_STKN(GSTK_String);
  GSTK_BIND(GSTK_String, PKRGlobalState::getGFOBJ_String());
}

// Base named globals every ECMA environment provides.
inline const GlobalInitTable &ecmaGlobalInitializers() {
  static const GlobalInitTable table = {
      {GSTK_globalThis, initGSTK_globalThis},
      {GSTK_Infinity, initGSTK_Infinity},
      {GSTK_NaN, initGSTK_NaN},
      {GSTK_undefined, initGSTK_undefined},
      {GSTK_Function, initGSTK_Function},
      {GSTK_Boolean, initGSTK_Boolean},
      {GSTK_Symbol, initGSTK_Symbol},
      {GSTK_Error, initGSTK_Error},
      {GSTK_Object, initGSTK_Object},
      {GSTK_Array, initGSTK_Array},
      {GSTK_String, initGSTK_String},
  };
  return table;
}

// Weird special case from ECMA
inline void initFunctionPrototype(ECMAGraph *G) {
  NodeUID protoID = PKRGlobalState::getGFOBJ_Function_prototype();
  auto ac = DEFINE_ACTION() { return {{PKRGlobalState::getUNDEF()}}; });
  AllocClosure(G, protoID, ac, PKRGlobalState::getTRUE(),
               PKRGlobalState::getGOOBJ_Object_prototype());
}

inline void initArrayPrototype(ECMAGraph *G) {
  // Array.prototype
  NodeUID arrProtoID = PKRGlobalState::getGOOBJ_Array_prototype();

  // Array.prototype.values == Array.prototype[%Symbol.iterator%]
  NodeUID apvID = PKRGlobalState::ReserveNodeUID();
  AllocClosure(G, apvID, arrayIteratorAC, PKRGlobalState::getTRUE(),
               PKRGlobalState::getGFOBJ_Function_prototype());

  KarmaJoin(
      G,
      Karma(G, G->getPointees(arrProtoID, PKRGlobalState::EdgeIntern(PKR_Set)),
            {NULL, {arrProtoID, apvID}, {"values"}}));

  KarmaJoin(
      G,
      Karma(G, G->getPointees(arrProtoID, PKRGlobalState::EdgeIntern(PKR_Set)),
            {NULL, {arrProtoID, apvID}, {PKR_SYMBOL_LABEL(iterator)}}));

  // 23.1.3.22/23 push and pop, real rather than throwing stubs.
  auto defineArrayMethod = [&](const char *name,
                               const ECMAGraph::ActionClosure &ac) {
    NodeUID fn = PKRGlobalState::ReserveNodeUID();
    AllocClosure(G, fn, ac, PKRGlobalState::getTRUE(),
                 PKRGlobalState::getGFOBJ_Function_prototype());
    auto fd = std::make_shared<TempFieldDescriptor>();
    fd->addValue(fn);
    fd->addWritable(PKRGlobalState::getTRUE());
    fd->addEnumerable(PKRGlobalState::getFALSE());
    fd->addConfigurable(PKRGlobalState::getTRUE());
    // PRECISION: every Array.prototype carries these, so a lookup stops here
    // instead of walking on to Object.prototype and contributing undefined.
    fd->addDefinite(PKRGlobalState::getTRUE());
    KarmaJoin(G, Karma(G,
                       G->getPointees(arrProtoID, PKRGlobalState::EdgeIntern(
                                                      PKR_DefineOwnProperty)),
                       {NULL, {arrProtoID}, {name}, {fd}}));
  };
  defineArrayMethod("push", arrayPushAC);
  defineArrayMethod("pop", arrayPopAC);
}

// 20.1.3.6 Object.prototype.toString and 20.1.3.7 Object.prototype.valueOf
inline void initObjectPrototypeCoercion(ECMAGraph *G) {
  NodeUID objProto = PKRGlobalState::getGOOBJ_Object_prototype();

  // toString: the result is always a String, but step 14 reads %toStringTag%,
  // which may be a user accessor, so the read has to happen for its effects.
  NodeUID toStringFn = PKRGlobalState::ReserveNodeUID();
  auto toStringAC = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    ASSERT(args.A.size() == 2);
    auto thisSlots = decodeArgRanges(args.L, args.A[0]);
    if (!thisSlots.empty()) {
      std::vector<NodeUID> discarded;
      std::vector<ECMAGraph> branches;
      for (NodeUID recv : thisSlots[0]) {
        auto getActs =
            G->getPointees(recv, PKRGlobalState::EdgeIntern(PKR_Get));

        if (getActs.empty())
          continue;
        Karma(G, getActs, {NULL, {recv, recv}, {PKR_SYMBOL_LABEL(toStringTag)}},
              discarded, branches);
      }
      KarmaJoin(G, branches);
    }
    return {{PKRGlobalState::getSTRING()}};
  });
  AllocClosure(G, toStringFn, toStringAC, PKRGlobalState::getTRUE(),
               PKRGlobalState::getGFOBJ_Function_prototype());

  // valueOf: ToObject(this), which for an object receiver is the receiver.
  NodeUID valueOfFn = PKRGlobalState::ReserveNodeUID();
  auto valueOfAC = DEFINE_ACTION() {
    const ECMAGraph *G = args.G;
    ASSERT(args.A.size() == 2);
    auto thisSlots = decodeArgRanges(args.L, args.A[0]);
    if (thisSlots.empty() || thisSlots[0].empty())
      return {{PKRGlobalState::getUNDEF()}};

    std::set<NodeUID> out;
    for (NodeUID recv : thisSlots[0]) {
      if (!isObjectNode(G, recv))
        // ToObject would box the primitive into a wrapper object, which is not
        // modelled. Refuse rather than answer with the primitive, which is a
        // different value.
        throw std::runtime_error("PKR: Object.prototype.valueOf on a primitive "
                                 "receiver needs ToObject, unmodelled");
      out.insert(recv);
    }
    return {{out.begin(), out.end()}};
  });
  AllocClosure(G, valueOfFn, valueOfAC, PKRGlobalState::getTRUE(),
               PKRGlobalState::getGFOBJ_Function_prototype());

  // 20.1.3.x: both are writable, non-enumerable, configurable.
  auto define = [&](const char *name, NodeUID fn) {
    auto fd = std::make_shared<TempFieldDescriptor>();
    fd->addValue(fn);
    fd->addWritable(PKRGlobalState::getTRUE());
    fd->addEnumerable(PKRGlobalState::getFALSE());
    fd->addConfigurable(PKRGlobalState::getTRUE());
    // PRECISION: every Object.prototype carries these, so a lookup that reaches
    // here stops instead of walking on to null and contributing undefined.
    fd->addDefinite(PKRGlobalState::getTRUE());
    KarmaJoin(G, Karma(G,
                       G->getPointees(objProto, PKRGlobalState::EdgeIntern(
                                                    PKR_DefineOwnProperty)),
                       {NULL, {objProto}, {name}, {fd}}));
  };
  define("toString", toStringFn);
  define("valueOf", valueOfFn);
}

// 22.1.3.32 String.prototype.valueOf and 22.1.3.29 toString both return the
// receiver's [[StringData]]. Left as stubs they would shadow Object.prototype's
// real ones for a String object, so coercing `new String(x)` would refuse where
// it used to answer.
inline void initStringPrototypeCoercion(ECMAGraph *G) {
  NodeUID strProto = PKRGlobalState::getGOOBJ_String_prototype();

  auto define = [&](const char *name) {
    // ThisStringValue: the receiver is a String primitive or a String object,
    // and either way the answer is a String.
    //
    // A fresh closure per method: ActionClosureMap is a bimap, so binding one
    // closure object to a second node silently fails and leaves that function
    // object uncallable - which OrdinaryToPrimitive then skips, falling through
    // to Object.prototype's.
    auto thisStringValueAC = DEFINE_ACTION() {
      return {{PKRGlobalState::getSTRING()}};
    });
    NodeUID fn = PKRGlobalState::ReserveNodeUID();
    AllocClosure(G, fn, thisStringValueAC, PKRGlobalState::getTRUE(),
                 PKRGlobalState::getGFOBJ_Function_prototype());
    auto fd = std::make_shared<TempFieldDescriptor>();
    fd->addValue(fn);
    fd->addWritable(PKRGlobalState::getTRUE());
    fd->addEnumerable(PKRGlobalState::getFALSE());
    fd->addConfigurable(PKRGlobalState::getTRUE());
    // PRECISION: every String.prototype carries these, so a lookup stops here
    // rather than walking on to Object.prototype.
    fd->addDefinite(PKRGlobalState::getTRUE());
    KarmaJoin(G, Karma(G,
                       G->getPointees(strProto, PKRGlobalState::EdgeIntern(
                                                    PKR_DefineOwnProperty)),
                       {NULL, {strProto}, {name}, {fd}}));
  };
  define("valueOf");
  define("toString");
}

// 20.2.3.6 Function.prototype[@@hasInstance].
inline void initFunctionHasInstance(ECMAGraph *G) {
  NodeUID fn = PKRGlobalState::ReserveNodeUID();
  auto ac = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    ASSERT(args.A.size() == 2);

    auto thisSlots = decodeArgRanges(args.L, args.A[0]);
    auto argSlots = decodeArgRanges(args.L, args.A[1]);

    if (thisSlots.empty() || argSlots.empty())
      return {{PKRGlobalState::getFALSE()}};
    const std::set<NodeUID> &ctors = thisSlots[0];
    const std::set<NodeUID> &instances = argSlots[0];

    NodeUID ohi =
        PKRGlobalState::getActionNode(PKRGlobalState::NAC_OrdinaryHasInstance);

    std::set<NodeUID> res;
    std::vector<ECMAGraph> branches;
    for (NodeUID ctor : ctors) {
      for (NodeUID inst : instances) {
        ECMAGraph H = G->clone();
        auto r = invokeAction(ohi, {&H, {ctor, inst}});
        res.insert(r.L.begin(), r.L.end());
        branches.push_back(std::move(H));
      }
    }
    KarmaJoin(G, branches);
    ASSERT(!res.empty());
    return {{res.begin(), res.end()}};
  });
  AllocClosure(G, fn, ac, PKRGlobalState::getTRUE(),
               PKRGlobalState::getGFOBJ_Function_prototype());

  auto fd = std::make_shared<TempFieldDescriptor>();
  fd->addValue(fn);
  fd->addWritable(PKRGlobalState::getFALSE());
  fd->addEnumerable(PKRGlobalState::getFALSE());
  fd->addConfigurable(PKRGlobalState::getFALSE());
  // PRECISION: every concrete Function.prototype carries this, so a lookup that
  // reaches here stops instead of walking on to Object.prototype.
  fd->addDefinite(PKRGlobalState::getTRUE());

  NodeUID funProto = PKRGlobalState::getGFOBJ_Function_prototype();
  KarmaJoin(
      G, Karma(G,
               G->getPointees(
                   funProto, PKRGlobalState::EdgeIntern(PKR_DefineOwnProperty)),
               {NULL, {funProto}, {PKR_SYMBOL_LABEL(hasInstance)}, {fd}}));
}

// B.2.2.1: __proto__
inline void initObjectPrototypeProtoAccessor(ECMAGraph *G) {
  NodeUID getter = PKRGlobalState::ReserveNodeUID();
  auto get = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    NodeUID O = args.L[0];
    auto acts =
        G->getPointees(O, PKRGlobalState::EdgeIntern(PKR_GetPrototypeOf));
    std::vector<NodeUID> protos;
    std::vector<ECMAGraph> branches;
    Karma(G, acts, {NULL, {O}}, protos, branches);
    KarmaJoin(G, branches);
    if (protos.empty()) // primitive receiver; ToObject wrappers not modeled
      return {{PKRGlobalState::getUNDEF()}};
    return {{protos.begin(), protos.end()}};
  });
  AllocClosure(G, getter, get, PKRGlobalState::getTRUE(),
               PKRGlobalState::getGFOBJ_Function_prototype());

  NodeUID setter = PKRGlobalState::ReserveNodeUID();
  auto set = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    NodeUID O = args.L[0], proto = args.L[1];
    if (proto != PKRGlobalState::getNULL() && !isObjectNode(G, proto))
      return {};
    auto acts =
        G->getPointees(O, PKRGlobalState::EdgeIntern(PKR_SetPrototypeOf));
    KarmaJoin(G, Karma(G, acts, {NULL, {O, proto}}));
    return {};
  });
  AllocClosure(G, setter, set, PKRGlobalState::getTRUE(),
               PKRGlobalState::getGFOBJ_Function_prototype());

  auto fd = std::make_shared<TempFieldDescriptor>();
  fd->addGet(getter);
  fd->addSet(setter);
  fd->addEnumerable(PKRGlobalState::getFALSE());
  fd->addConfigurable(PKRGlobalState::getTRUE());
  NodeUID objProto = PKRGlobalState::getGOOBJ_Object_prototype();
  KarmaJoin(G, Karma(G,
                     G->getPointees(objProto, PKRGlobalState::EdgeIntern(
                                                  PKR_DefineOwnProperty)),
                     {NULL, {objProto}, {"__proto__"}, {fd}}));
}

inline void initFunctionConstructor(ECMAGraph *G) {
  ALLOC_CTR(PKRGlobalState::getGFOBJ_Function(), PKR_STUB_FUN,
            PKRGlobalState::getGFOBJ_Function_prototype());
  linkPrototypeConstructor(G, PKRGlobalState::getGFOBJ_Function(),
                           PKRGlobalState::getGFOBJ_Function_prototype());
  defineStubMethods(G, PKRGlobalState::getGFOBJ_Function_prototype(),
                    "Function.prototype.", {"toString"});
}

inline void initECMAEnvironment(ECMAGraph *G) {
  AllocOrdinaryObject(G, PKRGlobalState::getGOOBJ_Object_prototype(),
                      PKRGlobalState::getTRUE(), PKRGlobalState::getNULL());
  initFunctionPrototype(G);
  initFunctionHasInstance(G);
  initObjectPrototypeProtoAccessor(G);

  defineStubIntrinsic(
      G, PKRGlobalState::getGFOBJ_Object(),
      PKRGlobalState::getGOOBJ_Object_prototype(), PKRGlobalState::getNULL(),
      {"assign",
       "create",
       "defineProperties",
       "defineProperty",
       "entries",
       "freeze",
       "fromEntries",
       "getOwnPropertyDescriptor",
       "getOwnPropertyDescriptors",
       "getOwnPropertyNames",
       "getOwnPropertySymbols",
       "getPrototypeOf",
       "hasOwn",
       "is",
       "isExtensible",
       "isFrozen",
       "isSealed",
       "keys",
       "preventExtensions",
       "seal",
       "setPrototypeOf",
       "values"},
      {"hasOwnProperty", "isPrototypeOf", "propertyIsEnumerable",
       "toLocaleString"},
      "Object", PKR_STUB_FUN);
  initObjectPrototypeCoercion(G);

  defineStubIntrinsic(G, PKRGlobalState::getGFOBJ_Boolean(),
                      PKRGlobalState::getGOOBJ_Boolean_prototype(),
                      PKRGlobalState::getGOOBJ_Object_prototype(), {},
                      {"toString", "valueOf"}, "Boolean",
                      PKR_STUB_FUN);

  defineStubIntrinsic(G, PKRGlobalState::getGFOBJ_Symbol(),
                      PKRGlobalState::getGOOBJ_Symbol_prototype(),
                      PKRGlobalState::getGOOBJ_Object_prototype(),
                      {"for", "keyFor"}, {"toString", "valueOf"}, "Symbol",
                      PKR_STUB_FUN);
// ECMA defines these as non-configurable, also making them definite is a good
// idea as there are two parent protos above, so this might be useful too
#define AS_SYM_PROP(name)                                                      \
  {                                                                            \
    auto symFD = std::make_shared<TempFieldDescriptor>();                      \
    symFD->addValue(PKRGlobalState::getSYMBOL_##name());                       \
    symFD->addWritable(PKRGlobalState::getFALSE());                            \
    symFD->addEnumerable(PKRGlobalState::getFALSE());                          \
    symFD->addConfigurable(PKRGlobalState::getFALSE());                        \
    symFD->addDefinite(PKRGlobalState::getTRUE());                             \
    KarmaJoin(G, Karma(G,                                                      \
                       G->getPointees(                                         \
                           PKRGlobalState::getGFOBJ_Symbol(),                  \
                           PKRGlobalState::EdgeIntern(PKR_DefineOwnProperty)), \
                       {                                                       \
                         NULL, {PKRGlobalState::getGFOBJ_Symbol()}, {#name}, { \
                           symFD                                               \
                         }                                                     \
                       }));                                                    \
  }
  DEF_WELL_KNOWN_SYMBOLS(AS_SYM_PROP)
#undef AS_SYM_PROP

  defineStubIntrinsic(G, PKRGlobalState::getGFOBJ_Error(),
                      PKRGlobalState::getGOOBJ_Error_prototype(),
                      PKRGlobalState::getGOOBJ_Object_prototype(), {},
                      {"toString"}, "Error", PKR_STUB_FUN);

  defineStubIntrinsic(G, PKRGlobalState::getGFOBJ_Array(),
                      PKRGlobalState::getGOOBJ_Array_prototype(),
                      PKRGlobalState::getGOOBJ_Object_prototype(),
                      {"from", "isArray", "of"},
                      {"at",        "concat",   "copyWithin",     "entries",
                       "every",     "fill",     "filter",         "find",
                       "findIndex", "findLast", "findLastIndex",  "flat",
                       "flatMap",   "forEach",  "includes",       "indexOf",
                       "join",      "keys",     "lastIndexOf",    "map",
                       "reduce",    "reduceRight",
                       "reverse",   "shift",    "slice",          "some",
                       "sort",      "splice",   "toLocaleString", "toString",
                       "unshift"},
                      "Array", PKR_STUB_FUN);
  initArrayPrototype(G);

  // 22.1.1.1 String(value). Called as a function it answers with a String
  // primitive; ToString on the argument is what may run user code, via
  // ToPrimitive, so it happens for its effects.
  //
  // `new String(x)` is not modelled faithfully: forge's ConstructorCall
  // allocates an ordinary object with String.prototype, not a String exotic
  // object, so it has no length and no index properties. Returning a primitive
  // here means the receiver is what `new` yields, which is the same
  // approximation every other constructor gets.
  auto stringCtorAC = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    ASSERT(args.A.size() == 2);

    // Step 1: String() with no argument is the empty string.
    auto argSlots = decodeArgRanges(args.L, args.A[1]);
    if (!argSlots.empty()) {
      NodeUID tsAct =
          PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToString);
      // Each value is an alternative, so each coercion reads the same incoming
      // state rather than observing the previous one's effects.
      std::vector<ECMAGraph> branches;
      for (NodeUID v : argSlots[0]) {
        ECMAGraph H = G->clone();
        // A Symbol argument takes 22.1.1.1 step 2b, SymbolDescriptiveString,
        // rather than ToString - both answer with a String here.
        invokeAction(tsAct, {&H, {v}});
        branches.push_back(std::move(H));
      }
      KarmaJoin(G, branches);
    }
    return {{PKRGlobalState::getSTRING()}};
  });

  defineStubIntrinsic(
      G, PKRGlobalState::getGFOBJ_String(),
      PKRGlobalState::getGOOBJ_String_prototype(),
      PKRGlobalState::getGOOBJ_Object_prototype(),
      {"fromCharCode", "fromCodePoint", "raw"},
      {"at",          "charAt",      "charCodeAt",     "codePointAt",
       "concat",      "endsWith",    "includes",       "indexOf",
       "lastIndexOf", "localeCompare", "match",        "matchAll",
       "normalize",   "padEnd",      "padStart",       "repeat",
       "replace",     "replaceAll",  "search",         "slice",
       "split",       "startsWith",  "substring",      "toLocaleLowerCase",
       "toLocaleUpperCase", "toLowerCase", "toUpperCase",
       "trim",        "trimEnd",     "trimStart"},
      "String", stringCtorAC);
  initStringPrototypeCoercion(G);

  initFunctionConstructor(G);
}

} // namespace Prakriti

namespace Prakriti {

inline void initECMAModuleEnvironment(ECMAGraph *G) { initECMAEnvironment(G); }

inline const GlobalInitTable &ecmaModuleGlobalInitializers() {
  return ecmaGlobalInitializers();
}

} // namespace Prakriti

namespace Prakriti {

inline void initECMAScriptEnvironment(ECMAGraph *G) { initECMAEnvironment(G); }

inline const GlobalInitTable &ecmaScriptGlobalInitializers() {
  return ecmaGlobalInitializers();
}

} // namespace Prakriti

namespace Prakriti {

inline void initGSTK_console(ECMAGraph *G) {
  NodeUID ref = PKRGlobalState::getGlobal(GSTK_console);
  AllocStackObject(G, ref);
  KarmaJoin(G, Karma(G,
                     G->getPointees(ref, PKRGlobalState::EdgeIntern(PKR_Set)),
                     {NULL, {ref, PKRGlobalState::getGOOBJ_console()}}));
}

inline const GlobalInitTable &consoleGlobalInitializers() {
  static const GlobalInitTable table = {{GSTK_console, initGSTK_console}};
  return table;
}

// A plain object, not a stub: log/warn/error accept any arguments, return
// undefined, no graph side effects.
inline void initConsole(ECMAGraph *G) {
  NodeUID consoleID = PKRGlobalState::getGOOBJ_console();
  AllocOrdinaryObject(G, consoleID, PKRGlobalState::getTRUE(),
                      PKRGlobalState::getGOOBJ_Object_prototype());
  defineNoopMethods(G, consoleID, {"log", "warn", "error"});
}

} // namespace Prakriti

namespace Prakriti {

inline void initNodeModuleEnvironment(ECMAGraph *G) {
  initECMAEnvironment(G);
  initConsole(G);
}

inline const GlobalInitTable &nodeModuleGlobalInitializers() {
  static const GlobalInitTable table = composeGlobalInitializers(
      {&ecmaGlobalInitializers(), &consoleGlobalInitializers()});
  return table;
}

} // namespace Prakriti

namespace Prakriti {

inline void initNodeScriptEnvironment(ECMAGraph *G) {
  initECMAEnvironment(G);
  initConsole(G);
}

inline const GlobalInitTable &nodeScriptGlobalInitializers() {
  static const GlobalInitTable table = composeGlobalInitializers(
      {&ecmaGlobalInitializers(), &consoleGlobalInitializers()});
  return table;
}

} // namespace Prakriti

namespace Prakriti {

inline void initQJSModuleEnvironment(ECMAGraph *G) {
  initECMAEnvironment(G);
  initConsole(G);
}

inline const GlobalInitTable &qjsModuleGlobalInitializers() {
  static const GlobalInitTable table = composeGlobalInitializers(
      {&ecmaGlobalInitializers(), &consoleGlobalInitializers()});
  return table;
}

} // namespace Prakriti

namespace Prakriti {

inline void initQJSScriptEnvironment(ECMAGraph *G) {
  initECMAEnvironment(G);
  initConsole(G);
}

inline const GlobalInitTable &qjsScriptGlobalInitializers() {
  static const GlobalInitTable table = composeGlobalInitializers(
      {&ecmaGlobalInitializers(), &consoleGlobalInitializers()});
  return table;
}

} // namespace Prakriti

namespace Prakriti {

inline void initLeaves(ECMAGraph *G) {
  G->addNode(PKRGlobalState::getINF(), TAG::INF_VAL);
  G->addNode(PKRGlobalState::getNAN(), TAG::NAN_VAL);
  G->addNode(PKRGlobalState::getUNDEF(), TAG::UNDEF_VAL);
  G->addNode(PKRGlobalState::getNULL(), TAG::NULL_VAL);
  G->addNode(PKRGlobalState::getTRUE(), TAG::TRUE_VAL);
  G->addNode(PKRGlobalState::getFALSE(), TAG::FALSE_VAL);
  G->addNode(PKRGlobalState::getNUMBER(), TAG::NUMBER_VAL);
  G->addNode(PKRGlobalState::getSTRING(), TAG::STRING_VAL);
  G->addNode(PKRGlobalState::getBIGINT(), TAG::BIGINT_VAL);
#define AS_SYM_NODE(name)                                                      \
  G->addNode(PKRGlobalState::getSYMBOL_##name(),                               \
             TAG::PKR_SYMBOL_TAG(name));
  DEF_WELL_KNOWN_SYMBOLS(AS_SYM_NODE)
#undef AS_SYM_NODE
}

#define DEF_JSFILE_EVAL(NACName, envInitFn)                                    \
  PKRGlobalState::NACName = DEFINE_ACTION() {                                  \
    ECMAGraph *G = args.G;                                                     \
    auto &L = args.L;                                                          \
                                                                               \
    initLeaves(G);                                                             \
    envInitFn(G);                                                              \
                                                                               \
    ASSERT(L.size() == 1);                                                     \
    NodeUID ctx = args.L[0];                                                   \
                                                                               \
    /* Call Registered Action Closure */                                       \
    invokeAction(ctx, {G});                                                    \
                                                                               \
    /* After expanding the node, delete it */                                  \
    G->removeNode(ctx);                                                        \
                                                                               \
    return {};                                                                 \
  });

inline bool initJSFileNodes = []() {
  DEF_JSFILE_EVAL(NAC_ECMASCRIPT_Eval, initECMAScriptEnvironment)
  DEF_JSFILE_EVAL(NAC_ECMAMODULE_Eval, initECMAModuleEnvironment)
  DEF_JSFILE_EVAL(NAC_NODESCRIPT_Eval, initNodeScriptEnvironment)
  DEF_JSFILE_EVAL(NAC_NODEMODULE_Eval, initNodeModuleEnvironment)
  DEF_JSFILE_EVAL(NAC_QJSSCRIPT_Eval, initQJSScriptEnvironment)
  DEF_JSFILE_EVAL(NAC_QJSMODULE_Eval, initQJSModuleEnvironment)

  return true;
}();

#undef DEF_JSFILE_EVAL

namespace detail {

#define DEF_ALLOC_JSFILE(FnName, NACName)                                      \
  inline void FnName(ECMAGraph *G, NodeUID id, ECMAGraph::ActionClosure ac) {  \
    G->addNode(id, TAG::JSFILE);                                               \
    PKRGlobalState::associateActionClosure(id, ac);                            \
                                                                               \
    NodeUID evalNode = PKRGlobalState::getActionNode(PKRGlobalState::NACName); \
    G->addNode(evalNode, TAG::ACT);                                            \
    G->addEdge(id, evalNode, PKRGlobalState::EdgeIntern(PKR_Eval));            \
  }

DEF_ALLOC_JSFILE(allocECMAScriptFile, NAC_ECMASCRIPT_Eval)
DEF_ALLOC_JSFILE(allocECMAModuleFile, NAC_ECMAMODULE_Eval)
DEF_ALLOC_JSFILE(allocNodeScriptFile, NAC_NODESCRIPT_Eval)
DEF_ALLOC_JSFILE(allocNodeModuleFile, NAC_NODEMODULE_Eval)
DEF_ALLOC_JSFILE(allocQJSScriptFile, NAC_QJSSCRIPT_Eval)
DEF_ALLOC_JSFILE(allocQJSModuleFile, NAC_QJSMODULE_Eval)

#undef DEF_ALLOC_JSFILE

} // namespace detail

// A neat wrapper trick to prevent mismatched nodes and global registries
struct JSFileAllocator {
  void (*allocFile)(ECMAGraph *, NodeUID, ECMAGraph::ActionClosure);
  const GlobalInitTable &globals;

  // Declaring here is what keeps the two in sync: no file exists without its
  // environment's bindings reserved.
  void alloc(ECMAGraph *G, NodeUID id, ECMAGraph::ActionClosure ac) const {
    declareGlobals(globals);
    allocFile(G, id, ac);
  }
};

inline JSFileAllocator ECMAScriptFile() {
  return {detail::allocECMAScriptFile, ecmaScriptGlobalInitializers()};
}
inline JSFileAllocator ECMAModuleFile() {
  return {detail::allocECMAModuleFile, ecmaModuleGlobalInitializers()};
}
inline JSFileAllocator NodeScriptFile() {
  return {detail::allocNodeScriptFile, nodeScriptGlobalInitializers()};
}
inline JSFileAllocator NodeModuleFile() {
  return {detail::allocNodeModuleFile, nodeModuleGlobalInitializers()};
}
inline JSFileAllocator QJSScriptFile() {
  return {detail::allocQJSScriptFile, qjsScriptGlobalInitializers()};
}
inline JSFileAllocator QJSModuleFile() {
  return {detail::allocQJSModuleFile, qjsModuleGlobalInitializers()};
}

} // namespace Prakriti

namespace Prakriti {

// 10.4.3 String exotic object, the thing `new String(x)` produces. Ordinary
// internal methods throughout - what makes it exotic is the state below, the
// same way AllocArrayObject differs from AllocOrdinaryObject.
//
// The wrapped primitive is the abstract STRING node, so its length is unknown.
// That means the index properties cannot be enumerated, so they live in the
// may-alias bucket rather than at literal indices - the same shape a
// dynamically written array ends up with. Reading any index answers String, and
// length answers Number; faithful in shape, capped in value.
inline void AllocStringObject(ECMAGraph *G, NodeUID id) {
  G->addNode(id, TAG::STROBJ);

  NodeUID temp;
  SET_AC(id, NAC_OOBJ_GetPrototypeOf, PKR_GetPrototypeOf);
  SET_AC(id, NAC_OOBJ_SetPrototypeOf, PKR_SetPrototypeOf);
  SET_AC(id, NAC_OOBJ_IsExtensible, PKR_IsExtensible);
  SET_AC(id, NAC_OOBJ_PreventExtensions, PKR_PreventExtensions);
  SET_AC(id, NAC_OOBJ_GetOwnProperty, PKR_GetOwnProperty);
  SET_AC(id, NAC_OOBJ_DefineOwnProperty, PKR_DefineOwnProperty);
  SET_AC(id, NAC_OOBJ_HasProperty, PKR_HasProperty);
  SET_AC(id, NAC_OOBJ_Get, PKR_Get);
  SET_AC(id, NAC_OOBJ_Set, PKR_Set);
  SET_AC(id, NAC_OOBJ_Delete, PKR_Delete);
  SET_AC(id, NAC_OOBJ_OwnPropertyKeys, PKR_OwnPropertyKeys);

  G->addEdge(id, PKRGlobalState::getTRUE(),
             PKRGlobalState::EdgeIntern(PKR_EXTENSIBLE));
  G->addEdge(id, PKRGlobalState::getGOOBJ_String_prototype(),
             PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
  G->addEdge(id, PKRGlobalState::getSTRING(),
             PKRGlobalState::EdgeIntern(PKR_STRING_DATA));

  // Insensitive from the start: the index properties are in the bucket, so a
  // named read has to consult it.
  G->addEdge(id, PKRGlobalState::getFALSE(),
             PKRGlobalState::EdgeIntern(PKR_SENSITIVE));

  // 10.4.3: length is { [[Writable]]: false, [[Enumerable]]: false,
  // [[Configurable]]: false }. [[Definite]] because every String object has it,
  // which is what keeps a `.length` read off the prototype chain.
  NodeUID lengthFP = PKRGlobalState::generateSentinel(
      id, PKRGlobalState::EdgeIntern("length"));
  AllocFieldProxyObject(G, lengthFP);
  G->addEdge(id, lengthFP, PKRGlobalState::EdgeIntern("length"));
  G->addEdge(lengthFP, PKRGlobalState::getNUMBER(),
             PKRGlobalState::EdgeIntern(PKR_VALUE));
  G->addEdge(lengthFP, PKRGlobalState::getFALSE(),
             PKRGlobalState::EdgeIntern(PKR_WRITABLE));
  G->addEdge(lengthFP, PKRGlobalState::getFALSE(),
             PKRGlobalState::EdgeIntern(PKR_ENUMERABLE));
  G->addEdge(lengthFP, PKRGlobalState::getFALSE(),
             PKRGlobalState::EdgeIntern(PKR_CONFIGURABLE));
  G->addEdge(lengthFP, PKRGlobalState::getTRUE(),
             PKRGlobalState::EdgeIntern(PKR_DEFINITE));

  // The index properties. StringGetOwnProperty makes each one
  // { [[Writable]]: false, [[Enumerable]]: true, [[Configurable]]: false }, and
  // every one of them reads as a String. No [[Definite]]: which indices exist
  // depends on the length, which is unknown.
  NodeUID idxFP = PKRGlobalState::generateSentinel(
      id, PKRGlobalState::EdgeIntern(PKR_UNKNOWN_FIELD));
  AllocFieldProxyObject(G, idxFP);
  G->addEdge(id, idxFP, PKRGlobalState::EdgeIntern(PKR_UNKNOWN_FIELD));
  G->addEdge(idxFP, PKRGlobalState::getSTRING(),
             PKRGlobalState::EdgeIntern(PKR_VALUE));
  G->addEdge(idxFP, PKRGlobalState::getFALSE(),
             PKRGlobalState::EdgeIntern(PKR_WRITABLE));
  G->addEdge(idxFP, PKRGlobalState::getTRUE(),
             PKRGlobalState::EdgeIntern(PKR_ENUMERABLE));
  G->addEdge(idxFP, PKRGlobalState::getFALSE(),
             PKRGlobalState::EdgeIntern(PKR_CONFIGURABLE));
}

} // namespace Prakriti

namespace Prakriti {

inline bool initTSOBJ = []() {
  PKRGlobalState::NAC_TSOBJ_Set = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() >= 2);
    NodeUID ctx = L[0];

    // PRECISION: strong only inside the window the owning frame opened. A mixed
    // set is the join of a branch that closed the window with one that did not,
    // and falls to weak, which is the safe direction.
    auto transience =
        G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_TRANSIENCE));
    if (transience.size() == 1 && transience[0] == PKRGlobalState::getTRUE())
      G->removeAllOutgoingEdgesByLabel(ctx, PKRGlobalState::EdgeIntern(PKR_STK));

    for (auto i = 1; i < L.size(); i++) {
      G->addEdge(ctx, L[i], PKRGlobalState::EdgeIntern(PKR_STK));
    }

    return {};
  });

  PKRGlobalState::NAC_TSOBJ_Get = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID ctx = args.L[0];
    return {G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_STK))};
  });

  return true;
}();

inline void AllocTransientStackObject(ECMAGraph *G, NodeUID id) {
  G->addNode(id, TAG::TSTKOBJ);
  NodeUID temp;
  SET_AC(id, NAC_TSOBJ_Set, PKR_Set);
  SET_AC(id, NAC_TSOBJ_Get, PKR_Get);
  G->addEdge(id, PKRGlobalState::getFALSE(),
             PKRGlobalState::EdgeIntern(PKR_TRANSIENCE));
}

} // namespace Prakriti

namespace Prakriti {

inline bool initWIPSOBJ = []() {
  PKRGlobalState::NAC_WIPSTKOBJ_Set = DEFINE_ACTION() {
    throw std::runtime_error("[Prakriti] Not implemented: WIPStackObject");
  });

  PKRGlobalState::NAC_WIPSTKOBJ_Get = DEFINE_ACTION() {
    throw std::runtime_error("[Prakriti] Not implemented: WIPStackObject");
  });

  return true;
}();

inline void AllocWIPStackObject(ECMAGraph *G, NodeUID id) {
  G->addNode(id, TAG::WIPSTKOBJ);
  NodeUID temp;
  SET_AC(id, NAC_WIPSTKOBJ_Set, PKR_Set);
  SET_AC(id, NAC_WIPSTKOBJ_Get, PKR_Get);
}

} // namespace Prakriti

#endif
