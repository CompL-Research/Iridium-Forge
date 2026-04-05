#ifndef PRAKRITI_COMBINED_HPP
#define PRAKRITI_COMBINED_HPP

#include <string>
#include <vector>

#include <string>
#include <stdexcept>

namespace Graph {

// Custom exception for when a node doesn't exist and needs to be removed
class MissingNodeRemovalError : public std::runtime_error {
public:
    explicit MissingNodeRemovalError(const std::string& nodeName)
        : std::runtime_error("Missing node removal error: Node '" + nodeName + "' does not exist") {}
};

// Custom exception for when a node doesn't exist and needs to be removed
class NodeNotFoundError : public std::runtime_error {
public:
    explicit NodeNotFoundError(const std::string& nodeName)
        : std::runtime_error("Missing node: Node '" + nodeName + "' does not exist") {}
};

// Custom exception for when a node referenced in an edge doesn't exist
class MissingNodeInEdgeError : public std::runtime_error {
public:
    MissingNodeInEdgeError(const std::string& source, const std::string& target)
        : std::runtime_error("Missing node in edge error: Node '" + source + "' or '" + target + "' does not exist") {}
};

// Custom exception for when an edge to be removed doesn't exist
class NonExistantEdgeRemovalError : public std::runtime_error {
public:
    NonExistantEdgeRemovalError(const std::string& source, const std::string& target, const std::string& name)
        : std::runtime_error("Non-existent edge removal error: Edge ('" + source + "', '" + target + "', '" + name + "') does not exist") {}
};

} // namespace Graph

#include <string>
#include <stdexcept>

namespace JSGraph
{

    class ActionKeyNotFoundError : public std::runtime_error
    {
    public:
        explicit ActionKeyNotFoundError(const std::string &message)
            : std::runtime_error(message) {}
    };

    class ECMANodeNotFound : public std::runtime_error
    {
    public:
        explicit ECMANodeNotFound(const std::string &message)
            : std::runtime_error(message) {}
    };

    class JSSL_STK_SET_Error : public std::runtime_error
    {
    public:
        explicit JSSL_STK_SET_Error(const std::string &message)
            : std::runtime_error(message) {}
    };

    class JSSL_STK_GET_Error : public std::runtime_error
    {
    public:
        explicit JSSL_STK_GET_Error(const std::string &message)
            : std::runtime_error(message) {}
    };

    class JS_OO_CTX_INVALID : public std::runtime_error
    {
    public:
        explicit JS_OO_CTX_INVALID(const std::string &message)
            : std::runtime_error(message) {}
    };

    class JS_OO_FIELD_TO_STACK_PTR : public std::runtime_error
    {
    public:
        explicit JS_OO_FIELD_TO_STACK_PTR(const std::string &message)
            : std::runtime_error(message) {}
    };

    class JS_OO_FIELD_TO_OO : public std::runtime_error
    {
    public:
        explicit JS_OO_FIELD_TO_OO(const std::string &message)
            : std::runtime_error(message) {}
    };

    class JS_IMPLEMENTATION_STUB : public std::runtime_error
    {
    public:
        explicit JS_IMPLEMENTATION_STUB(const std::string &message)
            : std::runtime_error(message) {}
    };

    class JS_FP_CTX_INVALID : public std::runtime_error
    {
    public:
        explicit JS_FP_CTX_INVALID(const std::string &message)
            : std::runtime_error(message) {}
    };

    
} // namespace JSGraph

#include <string>
#include <unordered_set>
#include <vector>
#include <functional>
#include <optional>

namespace Graph {

/// @brief Represents an edge in a graph with source and target nodes and a label.
template<typename NodeID>
struct Edge {
    /// @brief The source node of the edge.
    NodeID source;

    /// @brief The target node of the edge.
    NodeID target;

    /// @brief A string label associated with the edge.
    std::string label;

    /// @brief Compares two edges for equality.
    /// @param other The other edge to compare against.
    /// @return True if the edges are equal, false otherwise.
    bool operator==(const Edge& other) const {
        return source == other.source &&
               target == other.target &&
               label == other.label;
    }
};

/**
 * @brief Hash functor for Edge<NodeID>.
 *
 * Combines the hashes of source, target, and label using the
 * boost-style seed-xor pattern. Requires std::hash<NodeID> to exist.
 *
 * @tparam NodeID The node identifier type.
 */
template<typename NodeID>
struct EdgeHash {
    size_t operator()(const Edge<NodeID>& e) const noexcept {
        size_t seed = std::hash<NodeID>{}(e.source);
        seed ^= std::hash<NodeID>{}(e.target)  + 0x9e3779b9u + (seed << 6) + (seed >> 2);
        seed ^= std::hash<std::string>{}(e.label) + 0x9e3779b9u + (seed << 6) + (seed >> 2);
        return seed;
    }
};

/**
 * @brief Abstract base class for graph storage implementations.
 *
 * This class defines the interface for storing and manipulating graphs,
 * including nodes, edges, and their relationships. Implementations of this
 * class must provide concrete behavior for all virtual functions.
 *
 * @tparam Node The type representing a node in the graph.
 * @tparam NodeID A unique identifier type for nodes.
 */
template <typename Node, typename NodeID>
class GraphStorage {
public:
    /**
     * @brief Virtual destructor for proper inheritance cleanup.
     */
    virtual ~GraphStorage() = default;

    /**
     * @brief Adds a node to the graph.
     *
     * @param nodeName The identifier of the node to add.
     */
    virtual void addNode(const Node& nodeName) = 0;

    /**
     * @brief Retrieves the descriptor for a node by its ID.
     *
     * @param nodeName The ID of the node whose descriptor is requested.
     * @return The node descriptor.
     */
    virtual Node GetNodeDescriptor(const NodeID& nodeName) = 0;

    /**
     * @brief Replaces the descriptor for an existing node.
     *
     * The NodeID derived from the new descriptor must equal id; changing
     * node identity is not supported and will throw std::invalid_argument.
     *
     * @param id   The ID of the node to update.
     * @param node The new descriptor value.
     * @throws NodeNotFoundError      if id does not exist.
     * @throws std::invalid_argument  if static_cast<NodeID>(node) != id.
     */
    virtual void SetNodeDescriptor(const NodeID& id, const Node& node) = 0;

    /**
     * @brief Removes a node from the graph.
     *
     * @param nodeName The ID of the node to remove.
     * @return True if the node was successfully removed, false otherwise.
     */
    virtual bool removeNode(const NodeID& nodeName) = 0;

    /**
     * @brief Checks whether a node exists in the graph.
     *
     * @param nodeName The ID of the node to check for existence.
     * @return True if the node exists, false otherwise.
     */
    virtual bool hasNode(const NodeID& nodeName) const = 0;

    /**
     * @brief Retrieves all nodes in the graph.
     *
     * @return A set containing identifiers of all nodes.
     */
    virtual std::unordered_set<NodeID> getAllNodes() const = 0;

    /**
     * @brief Adds an edge to the graph.
     *
     * @param edge The edge to add, defined by source and target node IDs and a label.
     */
    virtual void addEdge(const Edge<NodeID>& edge) = 0;

    /**
     * @brief Removes an edge between two nodes from the graph.
     *
     * @param source The ID of the source node.
     * @param target The ID of the target node.
     * @return True if the edge was successfully removed, false otherwise.
     */
    virtual bool removeEdge(const NodeID& source, const NodeID& target) = 0;

    /**
     * @brief Checks whether an edge exists between two nodes.
     *
     * @param source The ID of the source node.
     * @param target The ID of the target node.
     * @return True if the edge exists, false otherwise.
     */
    virtual bool hasEdge(const NodeID& source, const NodeID& target) const = 0;
    
    /**
     * @brief Retrieves all edges in the graph.
     *
     * @return A set containing all edges in the graph.
     */
    virtual std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>> getAllEdges() const = 0;

    /**
     * @brief Gets all outgoing edges from a given node.
     *
     * @param nodeName The ID of the source node.
     * @return A set of outgoing edges from the specified node.
     */
    virtual std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>> getAllOutgoingEdges(const NodeID& nodeName) const = 0;

    /**
     * @brief Gets labels of all outgoing edges from a given node.
     *
     * @param id The ID of the source node.
     * @param prefix Optional prefix to filter edge labels.
     * @return A set containing labels of outgoing edges matching the criteria.
     */
    virtual std::unordered_set<std::string> GetOutgoingEdgeLabels(const NodeID& id, const std::optional<std::string>& prefix) = 0;

    /**
     * @brief Removes all outgoing edges from a node with a specific label.
     *
     * @param id The ID of the source node.
     * @param label The edge label to remove.
     */
    virtual void RemoveAllOutgoingEdgesByLabel(const NodeID& id, const std::string& label) = 0;

    /**
     * @brief Retrieves all pointees (target nodes) for a given node and edge label.
     *
     * @param id The ID of the source node.
     * @param label The edge label to filter by.
     * @return A set containing IDs of target nodes that match the criteria.
     */
    virtual std::unordered_set<NodeID> GetPointeesByName(const NodeID& id, const std::string& label) = 0;

    /**
     * @brief Checks equality with another graph storage instance.
     *
     * @param otherGraph The other graph to compare against.
     * @return True if graphs are equal, false otherwise.
     */
    virtual bool operator==(const GraphStorage & otherGraph) const = 0;

    /**
     * @brief Checks inequality with another graph storage instance.
     *
     * @param otherGraph The other graph to compare against.
     * @return True if graphs are not equal, false otherwise.
     */
    virtual bool operator!=(const GraphStorage & otherGraph) const = 0;

    /**
     * @brief Creates a copy of this graph storage instance.
     *
     * @param otherGraph The other graph to clone from (for context).
     * @return A pointer to the cloned graph storage object.
     */
    virtual GraphStorage* clone(const GraphStorage & otherGraph) const = 0;
};

}

#include <unordered_map>
#include <unordered_set>

namespace Graph {

template <typename Node, typename NodeID>
class InMemoryGraphStorage : public GraphStorage<Node, NodeID> {
private:
    std::unordered_map<NodeID, Node> nodes;
    std::unordered_map<NodeID, std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>>> outgoingEdges;

public:
    void addNode(const Node& nodeName) override {
        nodes[nodeName] = nodeName;
    }

    Node GetNodeDescriptor(const NodeID& nodeName) override {
        return nodes.at(nodeName);
    }

    void SetNodeDescriptor(const NodeID& id, const Node& node) override {
        NodeID newId = static_cast<NodeID>(node);
        if (newId != id) {
            throw std::invalid_argument("SetNodeDescriptor: NodeID mismatch");
        }
        auto it = nodes.find(id);
        if (it == nodes.end()) {
            throw NodeNotFoundError(std::to_string(static_cast<long long>(id)));
        }
        it->second = node;
    }

    bool removeNode(const NodeID& nodeName) override {
        if (nodes.erase(nodeName) > 0) {
            outgoingEdges.erase(nodeName);
            return true;
        }
        return false;
    }

    bool hasNode(const NodeID& nodeName) const override {
        return nodes.find(nodeName) != nodes.end();
    }

    std::unordered_set<NodeID> getAllNodes() const override {
        std::unordered_set<NodeID> result;
        for (const auto& pair : nodes) {
            result.insert(pair.first);
        }
        return result;
    }

    void addEdge(const Edge<NodeID>& edge) override {
        outgoingEdges[edge.source].insert(edge);
    }

    bool removeEdge(const NodeID& source, const NodeID& target) override {
        auto it = outgoingEdges.find(source);
        if (it != outgoingEdges.end()) {
            for (auto edgeIt = it->second.begin(); edgeIt != it->second.end(); ++edgeIt) {
                if (edgeIt->target == target) {
                    it->second.erase(edgeIt);
                    return true;
                }
            }
        }
        return false;
    }

    bool hasEdge(const NodeID& source, const NodeID& target) const override {
        auto it = outgoingEdges.find(source);
        if (it != outgoingEdges.end()) {
            for (const auto& edge : it->second) {
                if (edge.target == target) {
                    return true;
                }
            }
        }
        return false;
    }

    std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>> getAllEdges() const override {
        std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>> result;
        for (const auto& pair : outgoingEdges) {
            result.insert(pair.second.begin(), pair.second.end());
        }
        return result;
    }

    std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>> getAllOutgoingEdges(const NodeID& nodeName) const override {
        auto it = outgoingEdges.find(nodeName);
        if (it != outgoingEdges.end()) {
            return it->second;
        }
        return {};
    }

    std::unordered_set<std::string> GetOutgoingEdgeLabels(const NodeID& id, const std::optional<std::string>& prefix) override {
        std::unordered_set<std::string> result;
        auto it = outgoingEdges.find(id);
        if (it != outgoingEdges.end()) {
            for (const auto& edge : it->second) {
                if (!prefix || edge.label.substr(0, prefix.value().length()) == prefix.value()) {
                    result.insert(edge.label);
                }
            }
        }
        return result;
    }

    void RemoveAllOutgoingEdgesByLabel(const NodeID& id, const std::string& label) override {
        auto it = outgoingEdges.find(id);
        if (it != outgoingEdges.end()) {
            for (auto edgeIt = it->second.begin(); edgeIt != it->second.end();) {
                if (edgeIt->label == label) {
                    edgeIt = it->second.erase(edgeIt);
                } else {
                    ++edgeIt;
                }
            }
        }
    }

    std::unordered_set<NodeID> GetPointeesByName(const NodeID& id, const std::string& label) override {
        std::unordered_set<NodeID> result;
        auto it = outgoingEdges.find(id);
        if (it != outgoingEdges.end()) {
            for (const auto& edge : it->second) {
                if (edge.label == label) {
                    result.insert(edge.target);
                }
            }
        }
        return result;
    }

    bool operator==(const GraphStorage<Node, NodeID>& otherGraph) const override {
        // Implementation would compare node and edge sets
        // This is a simplified version - in practice you'd want full comparison
        return false;
    }

    bool operator!=(const GraphStorage<Node, NodeID>& otherGraph) const override {
        return !(*this == otherGraph);
    }

    GraphStorage<Node, NodeID>* clone(const GraphStorage<Node, NodeID>& /*otherGraph*/) const override {
        return new InMemoryGraphStorage<Node, NodeID>(*this);
    }
};

}

/*
 * COWGraphStorage — Copy-On-Write graph storage for pointer analysis.
 *
 * DESIGN RATIONALE
 * ----------------
 * Pointer analysis repeatedly clones a graph at each program point, then
 * applies a small number of edge mutations (typically 2-5 nodes touched per
 * statement).  Deep-cloning an adjacency list is O(N+E) every time.
 *
 * COW turns clone into O(N) shared_ptr copies (no data duplicated), and
 * lazily deep-copies only the NodeEntry that a mutation actually touches.
 * For k mutations after a clone, the total cost is O(N + k * avg_degree)
 * rather than O(N + E).
 *
 * DATA LAYOUT
 * -----------
 *   nodeMap_  :  NodeID  →  shared_ptr<NodeEntry>
 *
 *   NodeEntry
 *     node       — the Node descriptor
 *     byLabel    — label → unordered_set<NodeID targets>     hot PTA paths
 *     byTarget   — target → string label                     at-most-one-edge
 *
 * SINGLE-EDGE CONSTRAINT
 * ----------------------
 * At most one edge is allowed between any ordered pair (source, target).
 * byTarget therefore maps each target to exactly one label (a plain string,
 * not a set).  Calling addEdge with an existing (src, tgt) pair but a
 * different label replaces the old label.
 *
 * Both indexes are kept in sync on every mutation.
 *
 * This class does NOT maintain an incoming-edge index.  Consequently,
 * the reverse-lookup APIs (getAllIncomingEdges, getIncomingSources,
 * getIncomingEdgeLabels) are O(N+E) — they scan the whole graph.
 * For workloads that need fast reverse traversals, use COWGraphStorageV2.
 *
 * COMPLEXITY SUMMARY  (amortised, avg-case for hash containers)
 * --------------------------------------------------------------
 *   addNode                          O(1)
 *   GetNodeDescriptor                O(1)
 *   removeNode                       O(1)  — outgoing only; incoming NOT cleaned
 *   hasNode                          O(1)
 *   getAllNodes                       O(N)
 *
 *   addEdge                          O(1)  + possible COW copy of 1 entry
 *   removeEdge(src, tgt)             O(1)  — single label per pair
 *   removeEdgeByLabel(src, tgt, l)   O(1)  + possible COW copy of 1 entry
 *   hasEdge(src, tgt)                O(1)
 *   hasEdgeByLabel(src, tgt, l)      O(1)
 *   getAllEdges                       O(N + E)
 *   getAllOutgoingEdges(v)            O(d_out(v))
 *   getAllIncomingEdges(v)            O(N + E)  — no reverse index in v1
 *
 *   GetOutgoingEdgeLabels(v, prefix) O(L)   L = distinct labels on v
 *   RemoveAllOutgoingEdgesByLabel    O(k)   + COW copy of 1 entry
 *   GetPointeesByName(v, label)      O(1)
 *   getIncomingSources(v, label?)    O(N+E) — no reverse index in v1
 *   getIncomingEdgeLabels(v, src?)   O(N+E) — no reverse index in v1
 *
 *   clone()                          O(N)  — shared_ptr copies only
 *   operator==                       O(N + E)  — node descriptor + edges
 *   assertConsistent()               O(E)  — debug; verifies byLabel↔byTarget
 *
 * CONSTRAINT
 * ----------
 * Node must be statically castable to NodeID (i.e. Node is implicitly
 * convertible to NodeID) so that addNode can extract the key from the
 * node descriptor.  For JSGraph, Bindu<BinduId> provides operator BinduId().
 *
 * operator== requires Node to have operator== and operator!=.
 *
 * OPTIONAL PERFORMANCE UPGRADE
 * -----------------------------
 * Replace std::unordered_map / std::unordered_set with
 * robin_hood::unordered_map / robin_hood::unordered_flat_set from
 * https://github.com/martinus/robin-hood-hashing  (single header, ~2–4× faster).
 * The rest of the code is identical; just swap the type aliases below.
 */

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <string>
#include <cassert>

namespace Graph {

template<typename Node, typename NodeID>
class COWGraphStorage : public GraphStorage<Node, NodeID> {

    // ------------------------------------------------------------------ //
    //  Internal node entry — one per node, shared across graph versions.  //
    // ------------------------------------------------------------------ //
    struct NodeEntry {
        Node node;

        /// Primary index: label → set of target NodeIDs.
        /// Hot for GetPointeesByName and RemoveAllOutgoingEdgesByLabel.
        std::unordered_map<std::string, std::unordered_set<NodeID>> byLabel;

        /// Secondary index: target → single label.
        /// O(1) for hasEdge and removeEdge.
        /// A plain string because at most one edge is allowed per (src, tgt) pair.
        std::unordered_map<NodeID, std::string> byTarget;

        explicit NodeEntry(Node n) : node(std::move(n)) {}
        NodeEntry(const NodeEntry&)            = default;
        NodeEntry& operator=(const NodeEntry&) = default;
    };

    using EntryPtr = std::shared_ptr<NodeEntry>;

    /// Top-level map; copying it shares all NodeEntry objects (COW).
    std::unordered_map<NodeID, EntryPtr> nodeMap_;

    // ------------------------------------------------------------------ //
    //  COW helper                                                          //
    //  O(1) when already private; O(deg(id)) when a copy is needed.       //
    // ------------------------------------------------------------------ //
    NodeEntry& cowEntry(const NodeID& id) {
        auto it = nodeMap_.find(id);
        if (it == nodeMap_.end()) {
            throw NodeNotFoundError(std::to_string(static_cast<long long>(id)));
        }
        EntryPtr& ptr = it->second;
        if (ptr.use_count() > 1) {
            ptr = std::make_shared<NodeEntry>(*ptr);
        }
        return *ptr;
    }

public:

    // ================================================================== //
    //  Lifecycle                                                           //
    // ================================================================== //

    COWGraphStorage() = default;

    /// O(N) — copies N shared_ptrs. No NodeEntry data is duplicated.
    COWGraphStorage(const COWGraphStorage&)            = default;
    COWGraphStorage& operator=(const COWGraphStorage&) = default;

    COWGraphStorage(COWGraphStorage&&)            = default;
    COWGraphStorage& operator=(COWGraphStorage&&) = default;

    ~COWGraphStorage() override = default;

    // ================================================================== //
    //  Node operations                                                     //
    // ================================================================== //

    /// O(1) average.
    /// No-op if a node with the same NodeID already exists.
    void addNode(const Node& node) override {
        NodeID id = static_cast<NodeID>(node);
        if (!nodeMap_.count(id)) {
            nodeMap_[id] = std::make_shared<NodeEntry>(node);
        }
    }

    /// O(1) average.
    /// @throws NodeNotFoundError
    Node GetNodeDescriptor(const NodeID& id) override {
        auto it = nodeMap_.find(id);
        if (it == nodeMap_.end()) {
            throw NodeNotFoundError(std::to_string(static_cast<long long>(id)));
        }
        return it->second->node;
    }

    /// O(1) average. Triggers COW on the entry.
    /// @throws NodeNotFoundError      if id does not exist.
    /// @throws std::invalid_argument  if static_cast<NodeID>(node) != id.
    void SetNodeDescriptor(const NodeID& id, const Node& node) override {
        NodeID newId = static_cast<NodeID>(node);
        if (newId != id) {
            throw std::invalid_argument("SetNodeDescriptor: NodeID mismatch");
        }
        NodeEntry& entry = cowEntry(id);
        entry.node = node;
    }

    /// O(1) average.
    /// WARNING: does NOT clean up incoming edges from other nodes.
    /// Edges from other nodes that targeted this id become dangling.
    /// Use COWGraphStorageV2::removeNode for full detach (O(deg(id))).
    bool removeNode(const NodeID& id) override {
        return nodeMap_.erase(id) > 0;
    }

    /// O(1) average.
    bool hasNode(const NodeID& id) const override {
        return nodeMap_.count(id) > 0;
    }

    /// O(N).
    std::unordered_set<NodeID> getAllNodes() const override {
        std::unordered_set<NodeID> result;
        result.reserve(nodeMap_.size());
        for (const auto& kv : nodeMap_) {
            result.insert(kv.first);
        }
        return result;
    }

    // ================================================================== //
    //  Edge operations                                                     //
    // ================================================================== //

    /// O(1) average. May trigger a COW copy of the source entry.
    ///
    /// Both source and target must exist.
    /// At most one edge per (source, target) pair is enforced:
    ///   - Same (src, tgt, label):  no-op.
    ///   - Same (src, tgt), different label: replaces the old label.
    ///
    /// @throws MissingNodeInEdgeError if source or target does not exist.
    void addEdge(const Edge<NodeID>& edge) override {
        if (!nodeMap_.count(edge.source) || !nodeMap_.count(edge.target)) {
            throw MissingNodeInEdgeError(
                std::to_string(static_cast<long long>(edge.source)),
                std::to_string(static_cast<long long>(edge.target)));
        }

        // Check if (src, tgt) already has an edge.
        const auto& existing = nodeMap_[edge.source]->byTarget;
        auto exIt = existing.find(edge.target);
        if (exIt != existing.end()) {
            if (exIt->second == edge.label) return;   // exact duplicate: no-op

            // Different label: remove old label from byLabel, then fall through.
            NodeEntry& entry = cowEntry(edge.source);
            const std::string& oldLabel = entry.byTarget[edge.target];
            entry.byLabel[oldLabel].erase(edge.target);
            if (entry.byLabel[oldLabel].empty()) entry.byLabel.erase(oldLabel);
            entry.byTarget.erase(edge.target);
        }

        NodeEntry& entry = cowEntry(edge.source);
        entry.byLabel[edge.label].insert(edge.target);
        entry.byTarget[edge.target] = edge.label;
    }

    /// O(1) average. May trigger a COW copy of the source entry.
    /// Removes the single edge from source to target (regardless of label).
    /// Returns true if an edge existed and was removed.
    bool removeEdge(const NodeID& source, const NodeID& target) override {
        auto it = nodeMap_.find(source);
        if (it == nodeMap_.end()) return false;
        auto tgtIt = it->second->byTarget.find(target);
        if (tgtIt == it->second->byTarget.end()) return false;

        NodeEntry& entry = cowEntry(source);
        const std::string& label = entry.byTarget[target];
        entry.byLabel[label].erase(target);
        if (entry.byLabel[label].empty()) entry.byLabel.erase(label);
        entry.byTarget.erase(target);
        return true;
    }

    /// O(1) average. May trigger a COW copy of the source entry.
    /// Removes the edge only if it matches the given label exactly.
    /// Returns true if the edge existed with that label and was removed.
    bool removeEdgeByLabel(const NodeID& source, const NodeID& target,
                           const std::string& label)
    {
        auto it = nodeMap_.find(source);
        if (it == nodeMap_.end()) return false;
        auto tgtIt = it->second->byTarget.find(target);
        if (tgtIt == it->second->byTarget.end()) return false;
        if (tgtIt->second != label) return false;   // label mismatch

        NodeEntry& entry = cowEntry(source);
        entry.byLabel[label].erase(target);
        if (entry.byLabel[label].empty()) entry.byLabel.erase(label);
        entry.byTarget.erase(target);
        return true;
    }

    /// O(1) average.
    bool hasEdge(const NodeID& source, const NodeID& target) const override {
        auto it = nodeMap_.find(source);
        if (it == nodeMap_.end()) return false;
        return it->second->byTarget.count(target) > 0;
    }

    /// O(1) average. True only if the edge exists with exactly that label.
    bool hasEdgeByLabel(const NodeID& source, const NodeID& target,
                        const std::string& label) const
    {
        auto it = nodeMap_.find(source);
        if (it == nodeMap_.end()) return false;
        auto tgtIt = it->second->byTarget.find(target);
        if (tgtIt == it->second->byTarget.end()) return false;
        return tgtIt->second == label;
    }

    /// O(N + E) — iterates every node and its outgoing edges.
    std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>> getAllEdges() const override {
        std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>> result;
        for (const auto& [srcId, entryPtr] : nodeMap_) {
            for (const auto& [tgt, label] : entryPtr->byTarget) {
                result.insert({srcId, tgt, label});
            }
        }
        return result;
    }

    /// O(d_out(id)).
    std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>>
    getAllOutgoingEdges(const NodeID& id) const override {
        std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>> result;
        auto it = nodeMap_.find(id);
        if (it == nodeMap_.end()) return result;
        for (const auto& [tgt, label] : it->second->byTarget) {
            result.insert({id, tgt, label});
        }
        return result;
    }

    /// O(N + E) — no reverse index; must scan all nodes.
    /// Returns all edges whose target is `id`.
    /// Prefer COWGraphStorageV2 if this is called frequently.
    std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>>
    getAllIncomingEdges(const NodeID& id) const {
        std::unordered_set<Edge<NodeID>, EdgeHash<NodeID>> result;
        for (const auto& [srcId, entryPtr] : nodeMap_) {
            auto tgtIt = entryPtr->byTarget.find(id);
            if (tgtIt != entryPtr->byTarget.end()) {
                result.insert({srcId, id, tgtIt->second});
            }
        }
        return result;
    }

    // ================================================================== //
    //  PTA hot paths                                                       //
    // ================================================================== //

    /// O(L) where L = number of distinct labels on id.
    /// When prefix is set, only labels that start with that prefix are returned.
    std::unordered_set<std::string> GetOutgoingEdgeLabels(
        const NodeID& id,
        const std::optional<std::string>& prefix) override
    {
        std::unordered_set<std::string> result;
        auto it = nodeMap_.find(id);
        if (it == nodeMap_.end()) return result;
        for (const auto& [label, _] : it->second->byLabel) {
            if (!prefix ||
                (label.size() >= prefix->size() &&
                 label.compare(0, prefix->size(), *prefix) == 0))
            {
                result.insert(label);
            }
        }
        return result;
    }

    /// O(k) where k = number of edges with that label from id.
    /// Hot mutation in JSSL (e.g. STACK_set_strong).
    void RemoveAllOutgoingEdgesByLabel(const NodeID& id,
                                       const std::string& label) override
    {
        auto it = nodeMap_.find(id);
        if (it == nodeMap_.end()) return;
        if (!it->second->byLabel.count(label)) return;   // fast exit, no COW

        NodeEntry& entry = cowEntry(id);
        auto labelIt = entry.byLabel.find(label);
        if (labelIt == entry.byLabel.end()) return;

        for (const NodeID& tgt : labelIt->second) {
            entry.byTarget.erase(tgt);
        }
        entry.byLabel.erase(labelIt);
    }

    /// O(1) average — direct byLabel lookup.
    /// Returns a snapshot copy of the target set.
    std::unordered_set<NodeID> GetPointeesByName(
        const NodeID& id, const std::string& label) override
    {
        auto it = nodeMap_.find(id);
        if (it == nodeMap_.end()) return {};
        auto labelIt = it->second->byLabel.find(label);
        if (labelIt == it->second->byLabel.end()) return {};
        return labelIt->second;
    }

    // ================================================================== //
    //  Reverse-index queries                                               //
    //  No incoming index in v1 — all of these are O(N+E).                 //
    //  Use COWGraphStorageV2 if these are on the hot path.                 //
    // ================================================================== //

    /// O(N + E).
    /// Returns all NodeIDs that have at least one edge pointing to id.
    /// If label is given, returns only sources connected via that exact label.
    std::unordered_set<NodeID> getIncomingSources(
        const NodeID& id,
        const std::optional<std::string>& label = std::nullopt) const
    {
        std::unordered_set<NodeID> result;
        for (const auto& [srcId, entryPtr] : nodeMap_) {
            auto tgtIt = entryPtr->byTarget.find(id);
            if (tgtIt == entryPtr->byTarget.end()) continue;
            if (!label || tgtIt->second == *label) result.insert(srcId);
        }
        return result;
    }

    /// O(N + E).
    /// Returns the label of the incoming edge to id from each source.
    /// If fromSource is given, returns only the label(s) from that source.
    std::unordered_set<std::string> getIncomingEdgeLabels(
        const NodeID& id,
        const std::optional<NodeID>& fromSource = std::nullopt) const
    {
        std::unordered_set<std::string> result;
        for (const auto& [srcId, entryPtr] : nodeMap_) {
            if (fromSource && srcId != *fromSource) continue;
            auto tgtIt = entryPtr->byTarget.find(id);
            if (tgtIt != entryPtr->byTarget.end()) result.insert(tgtIt->second);
        }
        return result;
    }

    // ================================================================== //
    //  Equality & cloning                                                  //
    // ================================================================== //

    /// O(N + E) — compares node descriptors and edge structure for every node.
    ///
    /// Two graphs are equal iff:
    ///   1. They have the same node set.
    ///   2. Every node has an equal descriptor (requires Node::operator==).
    ///   3. Every node has an identical byTarget map (same edges and labels).
    ///
    /// Returns false when compared against a non-COWGraphStorage instance.
    bool operator==(const GraphStorage<Node, NodeID>& other) const override {
        const auto* o = dynamic_cast<const COWGraphStorage*>(&other);
        if (!o) return false;
        if (nodeMap_.size() != o->nodeMap_.size()) return false;
        for (const auto& [id, entryPtr] : nodeMap_) {
            auto oit = o->nodeMap_.find(id);
            if (oit == o->nodeMap_.end()) return false;
            if (entryPtr->node    != oit->second->node)    return false;
            if (entryPtr->byTarget != oit->second->byTarget) return false;
        }
        return true;
    }

    bool operator!=(const GraphStorage<Node, NodeID>& other) const override {
        return !(*this == other);
    }

    /// O(N) — copies N shared_ptrs. No NodeEntry data is duplicated.
    /// @param otherGraph Ignored (kept for interface compatibility).
    GraphStorage<Node, NodeID>* clone(
        const GraphStorage<Node, NodeID>& /*otherGraph*/) const override
    {
        return new COWGraphStorage(*this);
    }

    // ================================================================== //
    //  Debug invariant check                                               //
    // ================================================================== //

    /// O(E) — verifies that byLabel and byTarget are mutually consistent.
    ///
    /// Assertions:
    ///   A) for every (label, tgt) in byLabel: byTarget[tgt] == label
    ///   B) for every (tgt, label) in byTarget: byLabel[label].count(tgt)
    ///   C) each target appears in exactly one label's set in byLabel
    ///      (enforced by the single-edge constraint — checked implicitly
    ///       because byTarget is a flat map, not a multi-map)
    ///
    /// Terminates the program (assert) on any inconsistency.
    void assertConsistent() const {
        for (const auto& [srcId, srcPtr] : nodeMap_) {
            const NodeEntry& src = *srcPtr;

            // A: byLabel → byTarget
            for (const auto& [label, targets] : src.byLabel) {
                for (const NodeID& tgt : targets) {
                    auto tgtIt = src.byTarget.find(tgt);
                    assert(tgtIt != src.byTarget.end()
                           && "byLabel target missing from byTarget");
                    assert(tgtIt->second == label
                           && "byTarget label does not match byLabel label");
                }
            }

            // B: byTarget → byLabel
            for (const auto& [tgt, label] : src.byTarget) {
                auto lblIt = src.byLabel.find(label);
                assert(lblIt != src.byLabel.end()
                       && "byTarget label missing from byLabel");
                assert(lblIt->second.count(tgt)
                       && "target missing in byLabel entry");
            }
        }
    }
};

} // namespace Graph

#include <set>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>
#include <cassert>
#include <stdexcept>

namespace JSGraph
{
  // ------------------------------------------------------------------ //
  //  1. Primitive typedefs                                              //
  // ------------------------------------------------------------------ //

  typedef int BinduId;

  // ------------------------------------------------------------------ //
  //  2. Enumerations                                                    //
  // ------------------------------------------------------------------ //

  enum class TAG
  {
    STACK,
    ORDINARY_OBJECT,
    FIELD_PROXY,
    UKNOWN,
  };

  // Extend JS node actions by adding entries to this X-macro.
  #define DEF_ACTION_TAG(V) \
  V(STACK_set_strong)       \
  V(STACK_set_weak)

  enum class ActionTag
  {
  #define AS_ENUM(name) name,
    DEF_ACTION_TAG(AS_ENUM)
  #undef AS_ENUM
  };

  // ------------------------------------------------------------------ //
  //  3. FieldDescriptor (needed by PJSSL_ARG below)                    //
  // ------------------------------------------------------------------ //

  class FieldDescriptor
  {
  private:
    std::optional<BinduId> id;

  public:
    void FDUnion(const FieldDescriptor& /*fd*/)
    {
      throw std::runtime_error("TODO// STUB");
    }

    void GetAllSetters()
    {
      throw std::runtime_error("TODO// STUB");
    }

    bool IsNotConfigurable()
    {
      throw std::runtime_error("TODO// STUB");
    }
  };

  // ------------------------------------------------------------------ //
  //  4. ECMAGraph forward declaration (needed by PJSSL_ARG/RET)         //
  // ------------------------------------------------------------------ //

  class ECMAGraph;   // full definition at bottom of this file

  // ------------------------------------------------------------------ //
  //  5. Action argument/return types + ActionClosure type alias         //
  //  (PJSSL_ARG/RET must be complete before ActionClosure is used as   //
  //   a template argument inside Bindu member containers)              //
  // ------------------------------------------------------------------ //

  struct PJSSL_ARG
  {
    ECMAGraph* G;
    std::vector<BinduId> L;
    std::optional<std::vector<std::string>> A;
    std::optional<std::vector<FieldDescriptor>> X;
  };

  struct PJSSL_RET
  {
    ECMAGraph* G;
    std::vector<BinduId> L;
    std::optional<std::vector<std::string>> A;
  };

  /// A JS semantic action: takes a graph + context, returns an updated graph.
  using ActionClosure = std::function<PJSSL_RET(PJSSL_ARG)>;

  // ------------------------------------------------------------------ //
  //  6. Bindu — a node in the ECMAScript pointer-analysis graph         //
  // ------------------------------------------------------------------ //

  /**
   * @brief Stores a unique integer ID, a TAG, and per-ActionTag closures
   *        that implement the JS semantics for that node type.
   *
   * operator ID() allows Bindu<BinduId> to be used as a map key wherever
   * BinduId is expected (required by COWGraphStorage::addNode).
   */
  template <typename ID>
  class Bindu
  {
  private:
    ID id;
    TAG tag;
    std::set<ActionTag> availableActions;
    std::unordered_map<ActionTag, std::vector<ActionClosure>> actionMapping;

  public:
    explicit Bindu(ID id_, TAG tag_ = TAG::UKNOWN) : id(id_), tag(tag_) {}

    /// Implicit conversion to NodeID — required by GraphStorage::addNode.
    operator ID() const { return id; }

    ID getID() const { return id; }

    void SetTag(const TAG& newTag) { tag = newTag; }

    TAG GetTag() const { return tag; }

    std::vector<ActionClosure> GetActions(ActionTag t)
    {
      assert(availableActions.count(t) > 0);
      auto it = actionMapping.find(t);
      assert(it != actionMapping.end() && !it->second.empty());
      return it->second;
    }

    bool HasAction(ActionTag t) const
    {
      if (!availableActions.count(t)) return false;
      auto it = actionMapping.find(t);
      return it != actionMapping.end() && !it->second.empty();
    }

    void RegisterAction(ActionTag t, ActionClosure closure)
    {
      availableActions.insert(t);
      actionMapping[t].push_back(std::move(closure));
    }
  };

  // ------------------------------------------------------------------ //
  //  7. ECMAGraph — the full definition                                 //
  // ------------------------------------------------------------------ //

  /**
   * @brief The ECMAScript pointer-analysis graph.
   *
   * Extends COWGraphStorage<Bindu<BinduId>, BinduId> so that clone()
   * is O(N) shared_ptr copies rather than a full O(N+E) deep copy.
   * No additional private state — all graph logic lives in COWGraphStorage.
   */
  class ECMAGraph : public Graph::COWGraphStorage<Bindu<BinduId>, BinduId>
  {
  };

} // namespace JSGraph

#include <cassert>

// TAG, ActionTag, PJSSL_ARG, PJSSL_RET, ActionClosure, Bindu, ECMAGraph
// are all defined in JSGraph.hpp.

namespace JSGraph
{
  // ------------------------------------------------------------------ //
  //  STACK node actions                                                 //
  // ------------------------------------------------------------------ //

  /// Strong-reference assignment: removes all "stk" edges from ctx, then
  /// adds a new "stk" edge from ctx to target.
  /// L[0] = ctx node, L[1] = target node.
  inline ActionClosure AC_STACK_set_strong = [](PJSSL_ARG args) -> PJSSL_RET
  {
    ECMAGraph* G  = args.G;
    auto&      L  = args.L;

    assert(L.size() == 2);

    BinduId ctx    = L[0];
    BinduId target = L[1];

    G->RemoveAllOutgoingEdgesByLabel(ctx, "stk");
    G->addEdge({ ctx, target, "stk" });

    return { G, L, std::nullopt };
  };

  /// Weak-reference assignment (stub).
  inline ActionClosure AC_STACK_set_weak = [](PJSSL_ARG /*args*/) -> PJSSL_RET
  {
    throw std::runtime_error("STACK_set_weak: TODO // STUB");
  };

} // namespace JSGraph

#endif