#include "Iridium/Structure/RegisterAllocator.h"

void RAGC::begin(
    const std::vector<std::shared_ptr<EnvBindingSEXP>> &bindings,
    const std::set<std::shared_ptr<EnvBindingSEXP>> &captured)
{
    nodes.clear();
    edges.clear();
    allocation.clear();
    spillIdx = K;
    maxIdx = 0;
}

void RAGC::observe(
    const std::set<std::shared_ptr<EnvBindingSEXP>> &live)
{
    for (auto it1 = live.begin(); it1 != live.end(); it1++)
    {
        // TODO: Find out what JSARG and JSRESTARG mean
        // JSARG means it's a function argument
        // JSRESTARG means it's a rest argument (a pointer to an array)
        if ((*it1)->hasJSARG() || (*it1)->hasJSRESTARG())
        {
            continue;
        }

        nodes.insert(*it1);

        for (auto it2 = std::next(it1); it2 != live.end(); it2++)
        {
            if ((*it2)->hasJSARG() || (*it2)->hasJSRESTARG())
            {
                continue;
            }

            nodes.insert(*it2);
            edges[*it1].insert(*it2);
            edges[*it2].insert(*it1);
        }
    }
}

void RAGC::allocate()
{
    std::vector<std::shared_ptr<EnvBindingSEXP>> stack;

    // --- Simplify phase ---
    std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::set<std::shared_ptr<EnvBindingSEXP>>> graph = edges;
    std::set<std::shared_ptr<EnvBindingSEXP>> remaining(nodes.begin(), nodes.end());

    while (!remaining.empty())
    {
        bool removed = false;
        for (auto it = remaining.begin(); it != remaining.end();)
        {
            auto node = *it;
            size_t degree = graph[node].size();

            if (degree < K)
            {
                stack.push_back(node);
                // remove node from graph
                for (auto &nbr : graph[node])
                {
                    graph[nbr].erase(node);
                }
                graph.erase(node);
                it = remaining.erase(it);
                removed = true;
                break;
            }
            else
            {
                ++it;
            }
        }

        // If no node had degree < K, spill candidate
        if (!removed)
        {
            auto node = *remaining.begin();
            stack.push_back(node); // force push
            for (auto &nbr : graph[node])
            {
                graph[nbr].erase(node);
            }
            graph.erase(node);
            remaining.erase(node);
        }
    }

    // --- Select phase ---
    while (!stack.empty())
    {
        // TODO: Remove random, maybe better heuristic
        auto node = stack.back();
        stack.pop_back();

        // Collect neighbor allocations
        std::set<size_t> used;
        for (auto &nbr : edges[node])
        {
            if (allocation.count(nbr))
            {
                used.insert(allocation[nbr]);
            }
        }

        // Assign a free register if available
        size_t assigned = K; // sentinel
        for (size_t r = 0; r < K; ++r)
        {
            if (!used.count(r))
            {
                assigned = r;
                break;
            }
        }

        if (assigned == K)
        {
            // Spill
            assigned = spillIdx++;
        }

        if (assigned > maxIdx)
            maxIdx = assigned;
        allocation[node] = assigned;
        node->setREFIDX(assigned); // assumes EnvBindingSEXP has setIDX
    }
}

size_t RAGC::allocateExtra()
{
    return ++maxIdx;
}

size_t RAGC::getMaxIndex() const
{
    return maxIdx;
}

void ScopeBasedRegisterAllocator::begin(
    const std::vector<std::shared_ptr<EnvBindingSEXP>> &bindings,
    const std::set<std::shared_ptr<EnvBindingSEXP>> &captured)
{
    ordered.clear();
    nextIdx = 0;
    maxIdx = 0;

    // Walk bindings in scope order
    for (auto &b : bindings)
    {
        if (!captured.count(b) &&
            !b->hasJSARG() &&
            !b->hasJSRESTARG())
        {
            ordered.push_back(b);
        }
    }
}

void ScopeBasedRegisterAllocator::observe(const std::set<std::shared_ptr<EnvBindingSEXP>> &)
{
}

void ScopeBasedRegisterAllocator::allocate()
{
    for (auto &b : ordered)
    {
        b->setREFIDX(nextIdx++);
    }
    maxIdx = nextIdx - 1;
}

size_t ScopeBasedRegisterAllocator::allocateExtra()
{
    return ++maxIdx;
}

size_t ScopeBasedRegisterAllocator::getMaxIndex() const
{
    return maxIdx;
}
