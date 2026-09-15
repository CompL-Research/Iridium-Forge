#ifndef PRAKRITI_COMBINED_HPP
#define PRAKRITI_COMBINED_HPP

#include <string>
#include <vector>

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
  V(BIGINT_VAL)                                                                \
  V(SYMBOL_TOPRIMITIVE_VAL)

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
};

inline const char *dumpPKRTagToString(TAG tag) {
  switch (tag) {
#define NODE_TYPES(name)                                                       \
  case TAG::name:                                                              \
    return #name;
    DEF_NODE_TYPES(NODE_TYPES)
#undef NODE_TYPES
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
  V(NAC_ToPrimitive)                                                          \
  V(NAC_ToString)                                                             \
  V(NAC_ToNumeric)                                                            \
  V(NAC_HandleBinop)                                                         \
  V(NAC_HandleRelop)

// Global object identities. PKRGlobalState (ECMAGraph.hpp) turns each of
// these into a NodeUID field + getter + Init() reservation.
#define DEF_GLOBAL_IDENTITIES(V)                                              \
  V(GFOBJ_Object)                                                             \
  V(GFOBJ_Function)                                                           \
  V(GFOBJ_Boolean)                                                            \
  V(GFOBJ_Symbol)                                                             \
  V(GFOBJ_Error)                                                              \
  V(GFOBJ_Function_prototype)                                                 \
  V(GOOBJ_Object_prototype)                                                   \
  V(GOOBJ_Boolean_prototype)                                                  \
  V(GOOBJ_Symbol_prototype)                                                   \
  V(GOOBJ_Error_prototype)                                                    \
  V(GOOBJ_Array_prototype)                                                    \
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
  inline static NodeUID SYMBOL_TOPRIMITIVE_VAL = 0;

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

  static NodeUID getSYMBOL_TOPRIMITIVE() {
    ASSERT(isInitialized);
    return SYMBOL_TOPRIMITIVE_VAL;
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
    SYMBOL_TOPRIMITIVE_VAL = reserveNodeUID();

#define AS_GLOBAL_INIT(name) name = reserveNodeUID();
    DEF_GLOBAL_IDENTITIES(AS_GLOBAL_INIT)
#undef AS_GLOBAL_INIT

    globalStackBindings[edgeIntern(GSTK_globalThis)] = reserveNodeUID();
    globalStackBindings[edgeIntern(GSTK_Infinity)] = reserveNodeUID();
    globalStackBindings[edgeIntern(GSTK_NaN)] = reserveNodeUID();
    globalStackBindings[edgeIntern(GSTK_undefined)] = reserveNodeUID();
    globalStackBindings[edgeIntern(GSTK_Function)] = reserveNodeUID();
    globalStackBindings[edgeIntern(GSTK_Boolean)] = reserveNodeUID();
    globalStackBindings[edgeIntern(GSTK_Symbol)] = reserveNodeUID();
    globalStackBindings[edgeIntern(GSTK_Error)] = reserveNodeUID();
    globalStackBindings[edgeIntern(GSTK_Object)] = reserveNodeUID();

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
    if (uid == SYMBOL_TOPRIMITIVE_VAL)
      return "Symbol.toPrimitive";

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

// Generic RAII span for tracing a named call (e.g. Karma/KarmaBindu) that
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
#define PKR_SYM_toPrimitive "[[Symbol.toPrimitive]]"

#include <algorithm>
#include <set>
#include <vector>

#define SET_AC(src, ac, edge)                                                  \
  temp = PKRGlobalState::getActionNode(PKRGlobalState::ac);                    \
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

inline std::set<NodeUID> KarmaBindu(ECMAGraph *G,
                                    const std::vector<NodeUID> &acts,
                                    const ECMAGraph::PJSSL_ARG &args) {
  std::string enterExtra;
  if (isTraceEnabled())
    enterExtra = "acts=" + formatNodeVec(acts);
  TraceSpan span(std::nullopt, "KarmaBindu", args.L, args.A, enterExtra);

  std::vector<ECMAGraph> sources;
  std::set<NodeUID> res;
  auto kResults = Karma(G, acts, args);
  for (auto &r : kResults) {
    sources.push_back(r.clonedG);
    for (NodeUID b : r.ret.L)
      res.insert(b);
  }

  if (!sources.empty()) {
    ECMAGraph merged = std::move(sources.front());
    if (sources.size() > 1) {
      merged.mutateMergeUnion(
          std::vector<ECMAGraph>(sources.begin() + 1, sources.end()));
    }
    *G = std::move(merged);
  }

  if (isTraceEnabled())
    span.setExitExtra("merged=" + formatNodeVec(std::vector<NodeUID>(
                                      res.begin(), res.end())));
  return res;
}

template <typename T> inline bool isOnlyFalse(const T &nodes) {
  ASSERT(!nodes.empty());
  return std::ranges::all_of(
      nodes, [](const auto &tgt) { return tgt == PKRGlobalState::getFALSE(); });
}

[[nodiscard]] inline bool isOnlyFalse(ECMAGraph *G, NodeUID id, EdgeUID label) {
  return isOnlyFalse(G->getPointees(id, label));
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
};

//
// FDUnion
//

inline void FDUnionTT(TempFieldDescriptor *, TempFieldDescriptor *);
inline void FDUnionTF(TempFieldDescriptor *, FieldDescriptor *);
inline void FDUnionFT(const FieldDescriptor *self,
                      const TempFieldDescriptor *other);

inline void FDUnion(FieldDescriptor *self, FieldDescriptor *other) {
  if (auto *t = dynamic_cast<TempFieldDescriptor *>(self)) {
    if (auto *o = dynamic_cast<TempFieldDescriptor *>(other)) {
      return FDUnionTT(t, o);
    }

    return FDUnionTF(t, other);
  }

  if (const auto *o = dynamic_cast<TempFieldDescriptor *>(other)) {
    return FDUnionFT(self, o);
  }

  throw std::runtime_error("::TODO:: FieldDescriptor X FieldDescriptor");
}

inline void FDUnionTT(TempFieldDescriptor *, TempFieldDescriptor *) {

  throw std::runtime_error(
      "::TODO:: TempFieldDescriptor X TempFieldDescriptor");
}

inline void FDUnionTF(TempFieldDescriptor *, FieldDescriptor *) {

  throw std::runtime_error("::TODO:: TempFieldDescriptor X FieldDescriptor");
}

inline void replaceFieldIfSpecified(const FieldDescriptor *self,
                                    const std::vector<NodeUID> &vals,
                                    const char *label) {
  if (vals.empty())
    return;
  EdgeUID edgeLabel = PKRGlobalState::EdgeIntern(label);
  self->getGraph()->removeAllOutgoingEdgesByLabel(self->getID(), edgeLabel);
  for (NodeUID tgt : vals)
    self->getGraph()->addEdge(self->getID(), tgt, edgeLabel);
}

inline void FDUnionFT(const FieldDescriptor *self,
                      const TempFieldDescriptor *other) {
  if (!self->isLinked())
    throw std::runtime_error("FDUnion called on unlinked FieldDescriptor");

  replaceFieldIfSpecified(self, other->getValue(), PKR_VALUE);
  replaceFieldIfSpecified(self, other->getWritable(), PKR_WRITABLE);
  replaceFieldIfSpecified(self, other->getEnumerable(), PKR_ENUMERABLE);
  replaceFieldIfSpecified(self, other->getConfigurable(), PKR_CONFIGURABLE);
  replaceFieldIfSpecified(self, other->getGet(), PKR_Get);
  replaceFieldIfSpecified(self, other->getSet(), PKR_Set);
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

  return setters.empty() && getters.empty();
}

inline bool IsAccessorDescriptor(const TempFieldDescriptor *self) {
  return self->getGet().empty() && self->getSet().empty();
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
#include <unordered_set>

namespace Prakriti {

namespace OOHelpers {
//
// Local Helpers
//
inline static bool isExtensible(ECMAGraph *G, NodeUID ctx) {
  return isOnlyFalse(G, ctx, PKRGlobalState::EdgeIntern(PKR_EXTENSIBLE)) ? false
                                                                         : true;
}

inline static NodeUID getOwnProperty(ECMAGraph *G, NodeUID ctx,
                                     std::string &field) {
  auto acts =
      G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_GetOwnProperty));
  auto result = KarmaBindu(G, acts, {NULL, {ctx}, {field}});
  ASSERT(result.size() == 1);
  return *result.begin();
}

inline static std::set<NodeUID> getPrototypes(ECMAGraph *G, NodeUID ctx) {
  auto acts =
      G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_GetPrototypeOf));
  auto result = KarmaBindu(G, acts, {NULL, {ctx}});
  return result;
}

inline static bool hasProperty(ECMAGraph *G, NodeUID ctx, std::string &field) {
  auto acts = G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_HasProperty));
  auto result = KarmaBindu(G, acts, {NULL, {ctx}, {field}});
  ASSERT(result.size() == 1);
  return *result.begin() != PKRGlobalState::getUNDEF();
}

inline static void searchFPNodes(ECMAGraph *G, NodeUID ctx, std::string &field,
                                 std::vector<NodeUID> &res) {
  if (ctx == PKRGlobalState::getNULL())
    return;
  auto r = G->getPointees(ctx, PKRGlobalState::EdgeIntern(field.c_str()));
  if (r.size() == 0) {
    auto pp = G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
    for (auto p : pp) {
      searchFPNodes(G, p, field, res);
    }
  } else {
    ASSERT(r.size() == 1);
    res.push_back(*r.begin());
  }
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
    G->removeAllOutgoingEdgesByLabel(ctx,
                                     PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
    G->addEdge(ctx, V, PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
    return {{PKRGlobalState::getTRUE()}};
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
    if (!OOHelpers::isExtensible(G, ctx))
      return {};
    G->removeAllOutgoingEdgesByLabel(
        ctx, PKRGlobalState::EdgeIntern(PKR_EXTENSIBLE));
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
    return {fps};
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
    NodeUID currentFP = OOHelpers::getOwnProperty(G, ctx, field);
    auto isExtensible = OOHelpers::isExtensible(G, ctx);

    if (currentFP == PKRGlobalState::getUNDEF()) {
      if (!isExtensible)
        return {{PKRGlobalState::getFALSE()}};

      // Defining a brand new own property (data or accessor -- FDUnion
      // doesn't care, it just copies over whatever fields rhsPtr set, same
      // as NAC_OOBJ_Set's own "create new field proxy" path).
      NodeUID id = PKRGlobalState::generateSentinel(
          ctx, PKRGlobalState::EdgeIntern(field));
      AllocFieldProxyObject(G, id);
      G->addEdge(ctx, id, PKRGlobalState::EdgeIntern(field));

      FieldDescriptor newFD(G, id);
      FDUnion(&newFD, rhsPtr.get());
      return {{PKRGlobalState::getTRUE()}};
    }

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
    FDUnion(currentFD.get(), rhsPtr.get());
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
    NodeUID currentFP = OOHelpers::getOwnProperty(G, ctx, field);
    if (currentFP != PKRGlobalState::getUNDEF())
      return {{PKRGlobalState::getTRUE()}};
    for (auto p : OOHelpers::getPrototypes(G, ctx)) {
      if (p == PKRGlobalState::getNULL())
        continue;
      if (OOHelpers::hasProperty(G, p, field))
        return {{PKRGlobalState::getTRUE()}};
    }
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
    NodeUID currentFP = OOHelpers::getOwnProperty(G, ctx, field);
    std::vector<NodeUID> finRes;
    if (currentFP == PKRGlobalState::getUNDEF()) {
      std::unordered_set<NodeUID> res;
      for (auto &p : OOHelpers::getPrototypes(G, ctx)) {
        if (p == PKRGlobalState::getNULL()) {
          res.insert(PKRGlobalState::getUNDEF());
        } else {
          auto acts = G->getPointees(p, PKRGlobalState::EdgeIntern(PKR_Get));
          auto current_ = KarmaBindu(G, acts, {NULL, {p, rcvr}, {field}});
          for (auto &tgt : current_)
            res.insert(tgt);
        }
      }
      ASSERT(res.size() > 0);
      return {{res.begin(), res.end()}};
    } else {
      finRes = G->getPointees(currentFP, PKRGlobalState::EdgeIntern(PKR_VALUE));
    }
    auto acts = G->getPointees(currentFP, PKRGlobalState::EdgeIntern(PKR_Get));
    auto res = KarmaBindu(G, acts, {NULL, {rcvr}});
    for (auto r : res)
      finRes.push_back(r);
    return {{finRes.begin(), finRes.end()}};
  });

  PKRGlobalState::NAC_OOBJ_Set = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;
    ASSERT(args.L.size() == 2 && args.A.size() == 1);

    NodeUID ctx = L[0];
    NodeUID valToSet = L[1];
    std::string field = A[0];

    std::vector<NodeUID> fpNodes;
    OOHelpers::searchFPNodes(G, ctx, field, fpNodes);
    std::vector<KarmaResult> kResults;

    for (auto &fp : fpNodes) {
      FieldDescriptor f(G, fp);
      if (GetAllSetters(&f).size() > 0) {
        auto acts = G->getPointees(fp, PKRGlobalState::EdgeIntern(PKR_Set));
        auto rs = Karma(G, acts, {NULL, {ctx, valToSet}});
        for (auto &r : rs)
          kResults.push_back(std::move(r));
      }
    }

    // an accessor along the chain handled the write, dont also create a data property
    if (!kResults.empty()) {
      ECMAGraph merged = std::move(kResults.front().clonedG);
      if (kResults.size() > 1) {
        std::vector<ECMAGraph> rest;
        for (size_t i = 1; i < kResults.size(); i++) {
          rest.push_back(std::move(kResults[i].clonedG));
        }
        merged.mutateMergeUnion(rest);
      }
      *G = std::move(merged);
      return {{}};
    }

    auto currentFP = OOHelpers::getOwnProperty(G, ctx, field);
    auto tmp = std::make_shared<TempFieldDescriptor>();
    tmp->addValue(valToSet);
    if (currentFP == PKRGlobalState::getUNDEF()) {
      tmp->addWritable(PKRGlobalState::getTRUE());
      tmp->addEnumerable(PKRGlobalState::getTRUE());
      tmp->addConfigurable(PKRGlobalState::getTRUE());
    } else {
      auto currentFD = FieldDescriptor(G, currentFP);
      tmp->addWritable(!IsNotWritable(&currentFD) ? PKRGlobalState::getTRUE()
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
    KarmaBindu(G, acts, {NULL, {ctx}, {field}, {tmp}});
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

    NodeUID currentFP = OOHelpers::getOwnProperty(G, ctx, field);
    if (currentFP == PKRGlobalState::getUNDEF())
      return {{PKRGlobalState::getTRUE()}};

    FieldDescriptor currentFD(G, currentFP);
    if (IsNotConfigurable(&currentFD))
      return {{PKRGlobalState::getFALSE()}};
    G->removeAllOutgoingEdgesByLabel(ctx, PKRGlobalState::EdgeIntern(field));
    return {{PKRGlobalState::getTRUE()}};
  });

  PKRGlobalState::NAC_OOBJ_OwnPropertyKeys = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    auto &A = args.A;
    auto &X = args.X;
    ASSERT(args.L.size() == 1);

    NodeUID ctx = args.L[0];

    std::vector<std::string> res;

    for (auto &[_, __, lab] : G->getAllOutgoingEdges(ctx)) {
      auto lblStr = PKRGlobalState::EdgeGet(lab);
      if (lblStr.rfind("::", 0) == 0) {
        res.push_back(std::string(lblStr));
      }
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
    NodeUID stackCell = targets[0];
    auto acts = G->getPointees(stackCell, PKRGlobalState::EdgeIntern(PKR_Get));
    auto res = KarmaBindu(G, acts, {NULL, {stackCell}});
    return {{res.begin(), res.end()}};
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
    NodeUID stackCell = targets[0];
    auto acts = G->getPointees(stackCell, PKRGlobalState::EdgeIntern(PKR_Set));
    KarmaBindu(G, acts, {NULL, {stackCell, val}});
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

  NodeUID lengthFP =
      PKRGlobalState::generateSentinel(id, PKRGlobalState::EdgeIntern("length"));
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
    //   KarmaBindu(G,
    //              G->getPointees(st.target,
    //              PKRGlobalState::EdgeIntern(PKR_Set)), {NULL, L_});
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
      return {{PKRGlobalState::getNUMBER()}};
    }
  });

  // ECMA-262 7.1.1.1 OrdinaryToPrimitive
  PKRGlobalState::NAC_OrdinaryToPrimitive = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() == 1);
    NodeUID ctx = L[0];

    auto getActs = G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_Get));
    auto funs1 = KarmaBindu(G, getActs, {NULL, {ctx, ctx}, {"toString"}});
    auto funs2 = KarmaBindu(G, getActs, {NULL, {ctx, ctx}, {"valueOf"}});

    std::vector<NodeUID> toStringFns, valueOfFns;
    for (NodeUID f : funs1)
      if (PKRGlobalState::nodeHasActionClosure(f))
        toStringFns.push_back(f);
    for (NodeUID f : funs2)
      if (PKRGlobalState::nodeHasActionClosure(f))
        valueOfFns.push_back(f);

    // I am unsure about that to do here, for a later day ~ Meetesh
    ASSERT(!toStringFns.empty() || !valueOfFns.empty());

    // Its sound only under all execution ordered,
    // in the spec its under a loop, so some ordering exists,
    // cannot Karma and forget here...
    ECMAGraph GA = G->clone();
    auto resA1 = KarmaBindu(&GA, toStringFns, {NULL, {ctx}});
    auto resA2 = KarmaBindu(&GA, valueOfFns, {NULL, {ctx}});

    ECMAGraph GB = G->clone();
    auto resB1 = KarmaBindu(&GB, valueOfFns, {NULL, {ctx}});
    auto resB2 = KarmaBindu(&GB, toStringFns, {NULL, {ctx}});

    GA.mutateMergeUnion({GB});
    *G = std::move(GA);

    std::set<NodeUID> allResults;
    for (auto *s : {&resA1, &resA2, &resB1, &resB2})
      allResults.insert(s->begin(), s->end());

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
    auto funcs =
        KarmaBindu(G, getActs, {NULL, {val, val}, {PKR_SYM_toPrimitive}});

    bool sawUndefined = false;
    std::vector<NodeUID> callable;
    for (NodeUID f : funcs) {
      if (f == PKRGlobalState::getUNDEF())
        sawUndefined = true;
      else if (PKRGlobalState::nodeHasActionClosure(f))
        callable.push_back(f);
      else
        ASSERT(
            false); // non-callable [Symbol.toPrimitive]: TypeError, unmodeled
    }

    // Do I really wanna throw an error with undefined or just move on... its a
    // corner case I hope... ~ Meetesh
    ASSERT(!(sawUndefined && !callable.empty()));

    if (!callable.empty()) {
      auto results = KarmaBindu(G, callable, {NULL, {val}});
      return {{results.begin(), results.end()}};
    }

    NodeUID otpAct =
        PKRGlobalState::getActionNode(PKRGlobalState::NAC_OrdinaryToPrimitive);
    return invokeAction(otpAct, {G, {val}});
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

    if (op == "+") {
      NodeUID tpAct =
          PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToPrimitive);
      auto vv1Ret = invokeAction(tpAct, {G, {lval}});
      std::set<NodeUID> vv1(vv1Ret.L.begin(), vv1Ret.L.end());
      auto vv2Ret = invokeAction(tpAct, {G, {rval}});
      std::set<NodeUID> vv2(vv2Ret.L.begin(), vv2Ret.L.end());

      bool anyString = false;
      for (NodeUID v : vv1)
        anyString |= (G->getNodeTAG(v) == TAG::STRING_VAL);
      for (NodeUID v : vv2)
        anyString |= (G->getNodeTAG(v) == TAG::STRING_VAL);

      if (anyString) {
        NodeUID tsAct =
            PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToString);
        for (NodeUID v : vv1)
          invokeAction(tsAct, {G, {v}}); // side effects only
        for (NodeUID v : vv2)
          invokeAction(tsAct, {G, {v}}); // side effects only
        return {{PKRGlobalState::getSTRING()}};
      }

      lvalSet = vv1;
      rvalSet = vv2;
    }

    NodeUID tnumAct =
        PKRGlobalState::getActionNode(PKRGlobalState::NAC_ToNumeric);
    std::set<NodeUID> v1, v2;
    for (NodeUID v : lvalSet) {
      auto r = invokeAction(tnumAct, {G, {v}});
      v1.insert(r.L.begin(), r.L.end());
    }
    for (NodeUID v : rvalSet) {
      auto r = invokeAction(tnumAct, {G, {v}});
      v2.insert(r.L.begin(), r.L.end());
    }

    if (v1.size() == 1 && v2.size() == 1) {
      TAG t1 = G->getNodeTAG(*v1.begin());
      TAG t2 = G->getNodeTAG(*v2.begin());
      if (t1 == TAG::BIGINT_VAL && t2 == TAG::BIGINT_VAL)
        return {{PKRGlobalState::getBIGINT()}};
      if (t1 == TAG::NUMBER_VAL && t2 == TAG::NUMBER_VAL)
        return {{PKRGlobalState::getNUMBER()}};
    }

    // Over-approximated
    return {{PKRGlobalState::getNUMBER(), PKRGlobalState::getBIGINT()}};
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

  return true;
}();

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
}

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
  KarmaBindu(G, G->getPointees(id, PKRGlobalState::EdgeIntern(PKR_Set)),       \
             {NULL, {id, protoField}, {"prototype"}});

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
  KarmaBindu(G, G->getPointees(target, PKRGlobalState::EdgeIntern(PKR_Set)),
             {NULL, {target, methodID}, {propName}});
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
  KarmaBindu(G, G->getPointees(target, PKRGlobalState::EdgeIntern(PKR_Set)),
             {NULL, {target, methodID}, {propName}});
}

inline void defineNoopMethods(ECMAGraph *G, NodeUID target,
                              std::initializer_list<const char *> names) {
  for (const char *name : names)
    defineNoopMethod(G, target, name);
}

inline void linkPrototypeConstructor(ECMAGraph *G, NodeUID ctorID,
                                     NodeUID protoID) {
  KarmaBindu(G, G->getPointees(protoID, PKRGlobalState::EdgeIntern(PKR_Set)),
             {NULL, {protoID, ctorID}, {"constructor"}});
}

inline void
defineStubIntrinsic(ECMAGraph *G, NodeUID ctorID, NodeUID protoID,
                    NodeUID protoParent,
                    std::initializer_list<const char *> staticMethodNames,
                    std::initializer_list<const char *> protoMethodNames,
                    const std::string &name) {
  AllocOrdinaryObject(G, protoID, PKRGlobalState::getTRUE(), protoParent);
  ALLOC_CTR(ctorID, PKR_STUB_FUN, protoID);
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

#define ALLOC_STKN(name) AllocStackObject(G, PKRGlobalState::getGlobal(name))
#define GSTK_BIND(src, dest)                                                   \
  KarmaBindu(G,                                                                \
             G->getPointees(PKRGlobalState::getGlobal(src),                    \
                            PKRGlobalState::EdgeIntern(PKR_Set)),              \
             {NULL, {PKRGlobalState::getGlobal(src), dest}});

namespace Prakriti {

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

// Weird special case from ECMA
inline void initFunctionPrototype(ECMAGraph *G) {
  NodeUID protoID = PKRGlobalState::getGFOBJ_Function_prototype();
  auto ac = DEFINE_ACTION() { return {{PKRGlobalState::getUNDEF()}}; });
  AllocClosure(G, protoID, ac, PKRGlobalState::getTRUE(),
               PKRGlobalState::getGOOBJ_Object_prototype());
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
       "toLocaleString", "toString", "valueOf"},
      "Object");

  defineStubIntrinsic(G, PKRGlobalState::getGFOBJ_Boolean(),
                      PKRGlobalState::getGOOBJ_Boolean_prototype(),
                      PKRGlobalState::getGOOBJ_Object_prototype(), {},
                      {"toString", "valueOf"}, "Boolean");

  defineStubIntrinsic(G, PKRGlobalState::getGFOBJ_Symbol(),
                      PKRGlobalState::getGOOBJ_Symbol_prototype(),
                      PKRGlobalState::getGOOBJ_Object_prototype(),
                      {"for", "keyFor"}, {"toString", "valueOf"}, "Symbol");
  // Add Symbol.toPrimitive -> SYMBOL_TOPRIMITIVE_VAL
  KarmaBindu(G,
             G->getPointees(PKRGlobalState::getGFOBJ_Symbol(),
                            PKRGlobalState::EdgeIntern(PKR_Set)),
             {NULL,
              {PKRGlobalState::getGFOBJ_Symbol(),
               PKRGlobalState::getSYMBOL_TOPRIMITIVE()},
              {"toPrimitive"}});

  defineStubIntrinsic(G, PKRGlobalState::getGFOBJ_Error(),
                      PKRGlobalState::getGOOBJ_Error_prototype(),
                      PKRGlobalState::getGOOBJ_Object_prototype(), {},
                      {"toString"}, "Error");

  AllocOrdinaryObject(G, PKRGlobalState::getGOOBJ_Array_prototype(),
                      PKRGlobalState::getTRUE(),
                      PKRGlobalState::getGOOBJ_Object_prototype());

  initFunctionConstructor(G);
}

} // namespace Prakriti

namespace Prakriti {

inline void initECMAModuleEnvironment(ECMAGraph *G) { initECMAEnvironment(G); }

} // namespace Prakriti

namespace Prakriti {

inline void initECMAScriptEnvironment(ECMAGraph *G) { initECMAEnvironment(G); }

} // namespace Prakriti

namespace Prakriti {

inline void initGSTK_console(ECMAGraph *G) {
  NodeUID ref = PKRGlobalState::getGlobal(GSTK_console);
  AllocStackObject(G, ref);
  KarmaBindu(G, G->getPointees(ref, PKRGlobalState::EdgeIntern(PKR_Set)),
             {NULL, {ref, PKRGlobalState::getGOOBJ_console()}});
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

} // namespace Prakriti

namespace Prakriti {

inline void initNodeScriptEnvironment(ECMAGraph *G) {
  initECMAEnvironment(G);
  initConsole(G);
}

} // namespace Prakriti

namespace Prakriti {

inline void initQJSModuleEnvironment(ECMAGraph *G) {
  initECMAEnvironment(G);
  initConsole(G);
}

} // namespace Prakriti

namespace Prakriti {

inline void initQJSScriptEnvironment(ECMAGraph *G) {
  initECMAEnvironment(G);
  initConsole(G);
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
  G->addNode(PKRGlobalState::getSYMBOL_TOPRIMITIVE(),
             TAG::SYMBOL_TOPRIMITIVE_VAL);
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

#define DEF_ALLOC_JSFILE(FnName, NACName)                                      \
  inline void FnName(ECMAGraph *G, NodeUID id, ECMAGraph::ActionClosure ac) {  \
    G->addNode(id, TAG::JSFILE);                                               \
    PKRGlobalState::associateActionClosure(id, ac);                            \
                                                                               \
    NodeUID evalNode = PKRGlobalState::getActionNode(PKRGlobalState::NACName); \
    G->addNode(evalNode, TAG::ACT);                                            \
    G->addEdge(id, evalNode, PKRGlobalState::EdgeIntern(PKR_Eval));            \
  }

DEF_ALLOC_JSFILE(AllocECMAScriptFile, NAC_ECMASCRIPT_Eval)
DEF_ALLOC_JSFILE(AllocECMAModuleFile, NAC_ECMAMODULE_Eval)
DEF_ALLOC_JSFILE(AllocNodeScriptFile, NAC_NODESCRIPT_Eval)
DEF_ALLOC_JSFILE(AllocNodeModuleFile, NAC_NODEMODULE_Eval)
DEF_ALLOC_JSFILE(AllocQJSScriptFile, NAC_QJSSCRIPT_Eval)
DEF_ALLOC_JSFILE(AllocQJSModuleFile, NAC_QJSMODULE_Eval)

#undef DEF_ALLOC_JSFILE

} // namespace Prakriti

namespace Prakriti {

inline bool initTSOBJ = []() {
  PKRGlobalState::NAC_TSOBJ_Set = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    ASSERT(L.size() >= 2);
    NodeUID ctx = L[0];

    auto transience =
        G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_TRANSIENCE));
    bool isStrong =
        transience.size() == 1 && transience[0] == PKRGlobalState::getTRUE();

    if (isStrong) {
      G->removeAllOutgoingEdgesByLabel(ctx,
                                       PKRGlobalState::EdgeIntern(PKR_STK));
    }
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
