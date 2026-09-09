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

// #define GFOBJ_Object "~Object"
// #define GFOBJ_Function "~Function"
// #define GFOBJ_Boolean "~Boolean"
// #define GFOBJ_Symbol "~Symbol"
// #define GFOBJ_Error "~Error"
//
// #define GOOBJ_Object_prototype "~Object.prototype"
// #define GFOBJ_Function_prototype "~Function.prototype"
// #define GOOBJ_Boolean_prototype "~Boolean.prototype"
// #define GOOBJ_Symbol_prototype "~Symbol.prototype"
// #define GOOBJ_Error_prototype "~Error.prototype"

#define DEF_NODE_EVAL(V) V(JSFILE)

#define DEF_NODE_DYNAMIC(V)                                                    \
  V(STKOBJ)                                                                    \
  V(OOBJ)                                                                      \
  V(FOX)                                                                       \
  V(FOBJ)                                                                      \
  V(ACT)                                                                       \
  V(AWAIT)

#define DEF_NODE_STATIC(V)                                                     \
  V(UNDEF_VAL)                                                                 \
  V(NAN_VAL)                                                                   \
  V(INF_VAL)                                                                   \
  V(NULL_VAL)                                                                  \
  V(TRUE_VAL)                                                                  \
  V(FALSE_VAL)                                                                 \
  V(STATE_VAL)

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
  V(NAC_ECMASCRIPT_Eval)                                                       \
  V(NAC_ECMAMODULE_Eval)                                                       \
  V(NAC_Await_Eval)

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

  inline static NodeUID GFOBJ_Object = 0;
  inline static NodeUID GFOBJ_Function = 0;
  inline static NodeUID GFOBJ_Boolean = 0;
  inline static NodeUID GFOBJ_Symbol = 0;
  inline static NodeUID GFOBJ_Error = 0;

  inline static NodeUID GOOBJ_Object_prototype = 0;
  inline static NodeUID GFOBJ_Function_prototype = 0;
  inline static NodeUID GOOBJ_Boolean_prototype = 0;
  inline static NodeUID GOOBJ_Symbol_prototype = 0;
  inline static NodeUID GOOBJ_Error_prototype = 0;

  inline static std::unordered_map<EdgeUID, NodeUID> globalStackBindings;
  inline static bool isInitialized = false;
  inline static std::map<std::pair<NodeUID, EdgeUID>, NodeUID> sentinelMap_;

public:
  inline static std::function<NodeUID()> ReserveNodeUID = nullptr;
  inline static std::function<EdgeUID(std::string_view)> EdgeIntern = nullptr;
  inline static std::function<std::string_view(EdgeUID)> EdgeGet = nullptr;
  inline static boost::bimap<NodeUID, ActionClosure> ActionClosureMap;
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

  static NodeUID getGFOBJ_Object() { return GFOBJ_Object; }
  static NodeUID getGFOBJ_Function() { return GFOBJ_Function; }
  static NodeUID getGFOBJ_Boolean() { return GFOBJ_Boolean; }
  static NodeUID getGFOBJ_Symbol() { return GFOBJ_Symbol; }
  static NodeUID getGFOBJ_Error() { return GFOBJ_Error; }

  static NodeUID getGOOBJ_Object_prototype() { return GOOBJ_Object_prototype; }
  static NodeUID getGFOBJ_Function_prototype() {
    return GFOBJ_Function_prototype;
  }
  static NodeUID getGOOBJ_Boolean_prototype() {
    return GOOBJ_Boolean_prototype;
  }
  static NodeUID getGOOBJ_Symbol_prototype() { return GOOBJ_Symbol_prototype; }
  static NodeUID getGOOBJ_Error_prototype() { return GOOBJ_Error_prototype; }

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

    GFOBJ_Object = reserveNodeUID();
    GFOBJ_Function = reserveNodeUID();
    GFOBJ_Boolean = reserveNodeUID();
    GFOBJ_Symbol = reserveNodeUID();
    GFOBJ_Error = reserveNodeUID();

    GFOBJ_Function_prototype = reserveNodeUID();
    GOOBJ_Boolean_prototype = reserveNodeUID();
    GOOBJ_Symbol_prototype = reserveNodeUID();
    GOOBJ_Error_prototype = reserveNodeUID();
    GOOBJ_Object_prototype = reserveNodeUID();

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

    os << "[Global Objects]\n";
    os << "  GFOBJ_Object:   " << GFOBJ_Object << "\n";
    os << "  GFOBJ_Function: " << GFOBJ_Function << "\n";
    os << "  GFOBJ_Boolean:  " << GFOBJ_Boolean << "\n";
    os << "  GFOBJ_Symbol:   " << GFOBJ_Symbol << "\n";
    os << "  GFOBJ_Error:    " << GFOBJ_Error << "\n\n";

    os << "[Prototypes]\n";
    os << "  GFOBJ_Function_prototype: " << GFOBJ_Function_prototype << "\n";
    os << "  GOOBJ_Boolean_prototype:  " << GOOBJ_Boolean_prototype << "\n";
    os << "  GOOBJ_Symbol_prototype:   " << GOOBJ_Symbol_prototype << "\n";
    os << "  GOOBJ_Error_prototype:    " << GOOBJ_Error_prototype << "\n";
    os << "  GOOBJ_Object_prototype:   " << GOOBJ_Object_prototype << "\n\n";

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

    // Global Objects
    if (uid == GFOBJ_Object)
      return "GFOBJ_Object";
    if (uid == GFOBJ_Function)
      return "GFOBJ_Function";
    if (uid == GFOBJ_Boolean)
      return "GFOBJ_Boolean";
    if (uid == GFOBJ_Symbol)
      return "GFOBJ_Symbol";
    if (uid == GFOBJ_Error)
      return "GFOBJ_Error";

    // Prototypes
    if (uid == GOOBJ_Object_prototype)
      return "GOOBJ_Object_prototype";
    if (uid == GFOBJ_Function_prototype)
      return "GFOBJ_Function_prototype";
    if (uid == GOOBJ_Boolean_prototype)
      return "GOOBJ_Boolean_prototype";
    if (uid == GOOBJ_Symbol_prototype)
      return "GOOBJ_Symbol_prototype";
    if (uid == GOOBJ_Error_prototype)
      return "GOOBJ_Error_prototype";

    // Global Stack Bindings
    for (const auto &[edgeUid, nodeUid] : globalStackBindings) {
      if (nodeUid == uid) {
        std::string_view binding = EdgeGet ? EdgeGet(edgeUid) : "bound_node";
        return std::string(binding);
      }
    }

    // Action Closures lookup via ActionClosureMap
    auto closureIt = ActionClosureMap.left.find(uid);
    if (closureIt != ActionClosureMap.left.end()) {
      ActionClosure clos = closureIt->second;
#define MATCH_CLOSURE(name)                                                    \
  if (clos && clos == PKRGlobalState::name)                                    \
    return #name;
      DEF_GRAPH_CLOSURES(MATCH_CLOSURE)
#undef MATCH_CLOSURE
      return "ActionClosure";
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

#include <algorithm>
#include <set>
#include <vector>

#define SET_AC(src, ac, edge)                                                  \
  temp = PKRGlobalState::getActionNode(PKRGlobalState::ac);                    \
  G->addNode(temp, TAG::ACT);                                                  \
  G->addEdge(src, temp, PKRGlobalState::EdgeIntern(edge))

#define DEFINE_ACTION()                                                            \
      std::make_shared<std::function<ECMAGraph::PJSSL_RET(ECMAGraph::PJSSL_ARG)>>(                   \
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

inline std::vector<KarmaResult> Karma(const ECMAGraph *G,
                                      const std::vector<NodeUID> &acts,
                                      const ECMAGraph::PJSSL_ARG &args) {
  std::vector<KarmaResult> res;
  for (const NodeUID aID : acts) {
    if (G->getNodeTAG(aID) == TAG::ACT) {
      auto G_ = G->clone();
      auto act = PKRGlobalState::getActionClosure(aID);
      auto ret = (*act)(ECMAGraph::PJSSL_ARG{&G_, args.L, args.A, args.X});
      res.push_back(KarmaResult{std::move(G_), ret});
    }
  }
  return res;
}

inline std::set<NodeUID> KarmaBindu(ECMAGraph *G,
                                    const std::vector<NodeUID> &acts,
                                    const ECMAGraph::PJSSL_ARG &args) {
  std::vector<ECMAGraph> sources;
  std::set<NodeUID> res;
  auto kResults = Karma(G, acts, args);
  for (auto &r : kResults) {
    sources.push_back(r.clonedG);
    for (NodeUID b : r.ret.L)
      res.insert(b);
  }

  // In place Mutate
  G->mutateMergeUnion(sources);
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

namespace Prakriti {

inline bool initAwait = []() {
  PKRGlobalState::NAC_Await_Eval = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    const auto &L = args.L;

    ASSERT(L.size() > 0);
    NodeUID ctx = L[0];

    auto storeTargets = G->getAllOutgoingEdgesByLabel(
        ctx, PKRGlobalState::EdgeIntern(PKR_StoreTarget));

    for (const auto &st : storeTargets) {
      std::vector<NodeUID> L_(L.begin(), L.end());
      L_[0] = st.target;

      KarmaBindu(G,
                 G->getPointees(st.target, PKRGlobalState::EdgeIntern(PKR_Set)),
                 {NULL, L_});
    }

    auto savedStateVec = G->getAllOutgoingEdgesByLabel(
        ctx, PKRGlobalState::EdgeIntern(PKR_State));
    ASSERT(savedStateVec.size() == 1);

    auto savedState = savedStateVec.at(0).target;
    ASSERT(G->getNodeTAG(savedState) == TAG::STATE_VAL);

    // Call Registered Action Closure
    (*PKRGlobalState::getActionClosure(ctx))({G, {savedState}});

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

inline void FDUnionFT(const FieldDescriptor *self,
                      const TempFieldDescriptor *other) {
  if (!self->isLinked())
    throw std::runtime_error("FDUnion called on unlinked FieldDescriptor");

  for (NodeUID tgt : other->getValue())
    self->getGraph()->addEdge(self->getID(), tgt,
                              PKRGlobalState::EdgeIntern(PKR_VALUE));

  for (NodeUID id : other->getWritable())
    self->getGraph()->addEdge(self->getID(), id,
                              PKRGlobalState::EdgeIntern(PKR_WRITABLE));

  for (NodeUID id : other->getEnumerable())
    self->getGraph()->addEdge(self->getID(), id,
                              PKRGlobalState::EdgeIntern(PKR_ENUMERABLE));

  for (NodeUID id : other->getConfigurable())
    self->getGraph()->addEdge(self->getID(), id,
                              PKRGlobalState::EdgeIntern(PKR_CONFIGURABLE));

  for (NodeUID id : other->getGet())
    self->getGraph()->addEdge(self->getID(), id,
                              PKRGlobalState::EdgeIntern(PKR_Get));

  for (NodeUID id : other->getSet())
    self->getGraph()->addEdge(self->getID(), id,
                              PKRGlobalState::EdgeIntern(PKR_Set));
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

#include <stdexcept>
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

      // When can this happen in the runtime? Find out...
      throw std::runtime_error("OOBJ_DefineOwnProperty->TODO");
      // if (Prakriti::isDataDescriptor(rhsValue)) {
      //     current = AllocFieldProxyObject(G, ECMAGraph::genNodeUID());
      // } else {
      //     throw std::runtime_error("Todo handle definition of accessor
      //     properties");
      // }
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
          auto acts = G->getPointees(ctx, PKRGlobalState::EdgeIntern(PKR_Get));
          auto current_ = KarmaBindu(G, acts, {G, {p, rcvr}, {field}});
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
    auto &X = args.X;
    ASSERT(args.L.size() == 2 && args.A.size() == 1);

    NodeUID ctx = L[0];
    NodeUID valToSet = L[1];
    std::string field = A[0];

    // TODO: WIP
    bool isNeverWritable = false;

    if (isNeverWritable == false) {
      auto currentFP = OOHelpers::getOwnProperty(G, ctx, field);
      if (currentFP == PKRGlobalState::getUNDEF()) {
        auto tmp = std::make_shared<TempFieldDescriptor>();
        tmp->addValue(valToSet);
        tmp->addWritable(PKRGlobalState::getTRUE());
        tmp->addEnumerable(PKRGlobalState::getTRUE());
        tmp->addConfigurable(PKRGlobalState::getTRUE());

        //
        // Incorrect:
        // a.f = 12 :: [[fp:f]]->X | a.f = 13 :: [[fp:f]]->!X
        //
        // Correct:
        // a.f = 12 :: [[fp:f]]->X | a.f = 13 :: [[fp:f]]->X
        //

        NodeUID id = PKRGlobalState::generateSentinel(
            ctx, PKRGlobalState::EdgeIntern(field));
        AllocFieldProxyObject(G, id);
        G->addEdge(ctx, id, PKRGlobalState::EdgeIntern(field));
        G->addEdge(id, PKRGlobalState::getTRUE(),
                   PKRGlobalState::EdgeIntern(PKR_WRITABLE));
        G->addEdge(id, PKRGlobalState::getTRUE(),
                   PKRGlobalState::EdgeIntern(PKR_ENUMERABLE));
        G->addEdge(id, PKRGlobalState::getTRUE(),
                   PKRGlobalState::EdgeIntern(PKR_CONFIGURABLE));

        auto acts = G->getPointees(
            ctx, PKRGlobalState::EdgeIntern(PKR_DefineOwnProperty));
        KarmaBindu(G, acts, {NULL, {ctx}, {field}, {tmp}});
      } else {
        auto currentFD = FieldDescriptor(G, currentFP);
        auto tmp = std::make_shared<TempFieldDescriptor>();
        tmp->addValue(valToSet);
        tmp->addWritable(!IsNotWritable(&currentFD)
                             ? PKRGlobalState::getTRUE()
                             : PKRGlobalState::getFALSE());
        tmp->addEnumerable(!IsNotEnumerable(&currentFD)
                               ? PKRGlobalState::getTRUE()
                               : PKRGlobalState::getFALSE());
        tmp->addConfigurable(!IsNotConfigurable(&currentFD)
                                 ? PKRGlobalState::getTRUE()
                                 : PKRGlobalState::getFALSE());

        auto acts = G->getPointees(
            ctx, PKRGlobalState::EdgeIntern(PKR_DefineOwnProperty));
        KarmaBindu(G, acts, {NULL, {ctx}, {field}, {tmp}});
      }
    }
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

    std::vector<ECMAGraph> graphsToMerge;
    for (auto &kr : kResults)
      graphsToMerge.push_back(kr.clonedG);

    G->mutateMergeUnion(graphsToMerge);
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
    if (currentFP != PKRGlobalState::getUNDEF())
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

#define PKR_STUB_FUN                                                           \
  DEFINE_ACTION() {                                                            \
    ASSERT(false);                                                             \
    return {};                                                                 \
  })

#define ALLOC_STKN(name) AllocStackObject(G, PKRGlobalState::getGlobal(name))
#define ALLOC_CTR(id, func, protoField)                                        \
  AllocClosure(G, id, func, PKRGlobalState::getTRUE(),                         \
               PKRGlobalState::getGFOBJ_Function_prototype());                 \
  KarmaBindu(G, G->getPointees(id, PKRGlobalState::EdgeIntern(PKR_Set)),       \
             {NULL, {id, protoField}, {"prototype"}});

#define GSTK_BIND(src, dest)                                                   \
  KarmaBindu(G,                                                                \
             G->getPointees(PKRGlobalState::getGlobal(src),                    \
                            PKRGlobalState::EdgeIntern(PKR_Set)),              \
             {NULL, {PKRGlobalState::getGlobal(src), dest}});

namespace Prakriti {

inline void initGlobalStackRefs(ECMAGraph *G) {
  ALLOC_STKN(GSTK_globalThis);
  ALLOC_STKN(GSTK_Infinity);
  ALLOC_STKN(GSTK_NaN);
  ALLOC_STKN(GSTK_undefined);
  ALLOC_STKN(GSTK_Function);
  ALLOC_STKN(GSTK_Boolean);
  ALLOC_STKN(GSTK_Symbol);
  ALLOC_STKN(GSTK_Error);
  ALLOC_STKN(GSTK_Object);
}

inline void initLeaves(ECMAGraph *G) {
  G->addNode(PKRGlobalState::getINF(), TAG::INF_VAL);
  G->addNode(PKRGlobalState::getNAN(), TAG::NAN_VAL);
  G->addNode(PKRGlobalState::getUNDEF(), TAG::UNDEF_VAL);
  G->addNode(PKRGlobalState::getNULL(), TAG::NULL_VAL);
  G->addNode(PKRGlobalState::getTRUE(), TAG::TRUE_VAL);
  G->addNode(PKRGlobalState::getFALSE(), TAG::FALSE_VAL);
}

// ECMA 19.1
inline void initGlobalValueProps(ECMAGraph *G) {
  G->addEdge(PKRGlobalState::getGlobal(GSTK_Infinity), PKRGlobalState::getINF(),
             PKRGlobalState::EdgeIntern(PKR_STK));
  G->addEdge(PKRGlobalState::getGlobal(GSTK_NaN), PKRGlobalState::getNAN(),
             PKRGlobalState::EdgeIntern(PKR_STK));
  G->addEdge(PKRGlobalState::getGlobal(GSTK_undefined),
             PKRGlobalState::getUNDEF(), PKRGlobalState::EdgeIntern(PKR_STK));
}

// ECMA 20.1.3
inline void initObjectPrototype(ECMAGraph *G) {
  // Object.prototype [OOBJ]
  //      [[Extensible]] -> TRUE_VAL
  //      [[Prototype]]  -> NULL_VAL
  NodeUID objectPrototypeID = PKRGlobalState::getGOOBJ_Object_prototype();
  AllocOrdinaryObject(G, objectPrototypeID, PKRGlobalState::getTRUE(),
                      PKRGlobalState::getNULL());
}

// ECMA 20.2.3
inline void initFunctionPrototype(ECMAGraph *G) {
  // Function.prototype [FOBJ]
  //      [[Extensible]] -> TRUE_VAL
  //      [[Prototype]]  -> Object.prototype
  NodeUID functionPrototypeID = PKRGlobalState::getGFOBJ_Function_prototype();
  NodeUID objectPrototypeID = PKRGlobalState::getGOOBJ_Object_prototype();

  // ECMA -> accepts any arguments and returns undefined when invoked
  auto ac = DEFINE_ACTION() { return {{PKRGlobalState::getUNDEF()}}; });

  AllocClosure(G, functionPrototypeID, ac, PKRGlobalState::getTRUE(),
               objectPrototypeID);
}

inline void initBooleanPrototype(ECMAGraph *G) {
  // Boolean.prototype [OOBJ]
  //      [[Extensible]] -> TRUE_VAL
  //      [[Prototype]]  -> Object.prototype
  NodeUID booleanPrototypeID = PKRGlobalState::getGOOBJ_Boolean_prototype();
  NodeUID objectPrototypeID = PKRGlobalState::getGOOBJ_Object_prototype();
  AllocOrdinaryObject(G, booleanPrototypeID, PKRGlobalState::getTRUE(),
                      objectPrototypeID);
}

inline void initSymbolPrototype(ECMAGraph *G) {
  // Symbol.prototype [OOBJ]
  //      [[Extensible]] -> TRUE_VAL
  //      [[Prototype]]  -> Object.prototype
  NodeUID symbolPrototypeID = PKRGlobalState::getGOOBJ_Symbol_prototype();
  NodeUID objectPrototypeID = PKRGlobalState::getGOOBJ_Object_prototype();
  AllocOrdinaryObject(G, symbolPrototypeID, PKRGlobalState::getTRUE(),
                      objectPrototypeID);
}

inline void initErrorPrototype(ECMAGraph *G) {
  // Error.prototype [OOBJ]
  //      [[Extensible]] -> TRUE_VAL
  //      [[Prototype]]  -> Object.prototype
  NodeUID errorPrototypeID = PKRGlobalState::getGOOBJ_Error_prototype();
  NodeUID objectPrototypeID = PKRGlobalState::getGOOBJ_Object_prototype();
  AllocOrdinaryObject(G, errorPrototypeID, PKRGlobalState::getTRUE(),
                      objectPrototypeID);
}

// ECMA 20.1.1
inline void initObjectConstructor(ECMAGraph *G) {
  // Object [[FOBJ]]
  //      [[Extensible]] -> TRUE_VAL
  //      [[Prototype]]  -> Function.prototype
  //      prototype      -> Object.prototype

  // auto ac = DEFINE_ACTION() {
  //   auto &L = args.L;
  //   auto &A = args.A;
  //
  //   ASSERT(L.size() >= 1);
  //
  //   auto &ctx = L[0];
  //
  //   if (A.size() > 0) {
  //     // TODO: CTR Case
  //     ASSERT(false);
  //   }
  //
  //   auto ooRes =
  //       PKRGlobalState::generateSentinel(ctx,
  //       PKRGlobalState::EdgeIntern("2"));
  //
  //   // TODO: ToObject
  //   ASSERT(false);
  //   return {};
  // });
  ALLOC_CTR(PKRGlobalState::getGFOBJ_Object(), PKR_STUB_FUN,
            PKRGlobalState::getGOOBJ_Object_prototype());

  GSTK_BIND(GSTK_Object, PKRGlobalState::getGFOBJ_Object());
}

inline void initFunctionConstructor(ECMAGraph *G) {
  // Function [[FOBJ]]
  //      [[Extensible]] -> TRUE_VAL
  //      [[Prototype]]  -> Function.prototype
  //      prototype      -> Function.prototype

  ALLOC_CTR(PKRGlobalState::getGFOBJ_Function(), PKR_STUB_FUN,
            PKRGlobalState::getGFOBJ_Function_prototype());

  GSTK_BIND(GSTK_Function, PKRGlobalState::getGFOBJ_Function());
}

inline void initBooleanConstructor(ECMAGraph *G) {
  // Boolean [[FOBJ]]
  //      [[Extensible]] -> TRUE_VAL
  //      [[Prototype]]  -> Function.prototype
  //      prototype      -> Boolean.prototype

  ALLOC_CTR(PKRGlobalState::getGFOBJ_Boolean(), PKR_STUB_FUN,
            PKRGlobalState::getGOOBJ_Boolean_prototype());

  GSTK_BIND(GSTK_Boolean, PKRGlobalState::getGFOBJ_Boolean());
}

inline void initSymbolConstructor(ECMAGraph *G) {
  // Symbol [[FOBJ]]
  //      [[Extensible]] -> TRUE_VAL
  //      [[Prototype]]  -> Function.prototype
  //      prototype      -> Symbol.prototype

  ALLOC_CTR(PKRGlobalState::getGFOBJ_Symbol(), PKR_STUB_FUN,
            PKRGlobalState::getGOOBJ_Symbol_prototype());

  GSTK_BIND(GSTK_Symbol, PKRGlobalState::getGFOBJ_Symbol());
}

inline void initErrorConstructor(ECMAGraph *G) {
  // Error [[FOBJ]]
  //      [[Extensible]] -> TRUE_VAL
  //      [[Prototype]]  -> Function.prototype
  //      prototype      -> Error.prototype

  ALLOC_CTR(PKRGlobalState::getGFOBJ_Error(), PKR_STUB_FUN,
            PKRGlobalState::getGOOBJ_Symbol_prototype());

  GSTK_BIND(GSTK_Error, PKRGlobalState::getGFOBJ_Error());
}

inline bool initJSFileNodes = []() {
  PKRGlobalState::NAC_ECMASCRIPT_Eval = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;

    initGlobalStackRefs(G);
    initLeaves(G);
    initGlobalValueProps(G);

    initObjectPrototype(G);
    initFunctionPrototype(G);
    initBooleanPrototype(G);
    initSymbolPrototype(G);
    initErrorPrototype(G);

    initObjectConstructor(G);
    initFunctionConstructor(G);
    initBooleanConstructor(G);
    initSymbolConstructor(G);
    initErrorConstructor(G);

    ASSERT(L.size() == 1);
    NodeUID ctx = args.L[0];

    // Call Registered Action Closure
    (*PKRGlobalState::getActionClosure(ctx))({G});

    // After expanding the node, delete it
    G->removeNode(ctx);

    return {};
  });

  PKRGlobalState::NAC_ECMAMODULE_Eval = DEFINE_ACTION() {
    ECMAGraph *G = args.G;
    auto &L = args.L;
    // TODO Add Module Global Nodes

    ASSERT(L.size() == 1);
    NodeUID ctx = args.L[0];

    // Call Registered Action Closure
    (*PKRGlobalState::getActionClosure(ctx))({G});

    // After expanding the node, delete it
    G->removeNode(ctx);

    return {};
  });

  return true;
}();

inline void AllocECMAScriptFile(ECMAGraph *G, NodeUID id,
                                ECMAGraph::ActionClosure clos) {
  // Declare ECMAScript Node
  G->addNode(id, TAG::JSFILE);
  PKRGlobalState::associateActionClosure(id, clos);

  // Associate Eval ACT
  NodeUID esEval =
      PKRGlobalState::getActionNode(PKRGlobalState::NAC_ECMASCRIPT_Eval);
  G->addNode(esEval, TAG::ACT);
  G->addEdge(id, esEval, PKRGlobalState::EdgeIntern(PKR_Eval));
}

inline void AllocECMAModuleFile(ECMAGraph *G, NodeUID id,
                                ECMAGraph::ActionClosure ac) {
  // Declare ECMAModule Node
  G->addNode(id, TAG::JSFILE);
  PKRGlobalState::associateActionClosure(id, ac);

  // Associate Eval ACT
  NodeUID esmEval =
      PKRGlobalState::getActionNode(PKRGlobalState::NAC_ECMAMODULE_Eval);
  G->addNode(esmEval, TAG::ACT);
  G->addEdge(id, esmEval, PKRGlobalState::EdgeIntern(PKR_Eval));
}
} // namespace Prakriti

#endif
