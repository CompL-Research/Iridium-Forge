#pragma once

#include <set>
#include <memory>
#include "Iridium/Analysis/Domains/Liveness.h"

class EnvBindingSEXP;
class CFGManager;

struct RegisterAllocator
{
    virtual ~RegisterAllocator() = default;

    // Called once before feeding data
    virtual void begin(
        const std::vector<std::shared_ptr<EnvBindingSEXP>> &bindings,
        const std::set<std::shared_ptr<EnvBindingSEXP>> &captured) = 0;

    // Feed liveness info for one program point
    virtual void observe(
        const std::set<std::shared_ptr<EnvBindingSEXP>> &live) = 0;

    // Finalize allocation and assign REFIDX
    virtual void allocate() = 0;

    // Reserve a new stack/register slot
    virtual size_t allocateExtra() = 0;

    // Max register / stack index used
    virtual size_t getMaxIndex() const = 0;
};

struct RAGC : public RegisterAllocator
{
    std::set<std::shared_ptr<EnvBindingSEXP>> nodes;
    std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::set<std::shared_ptr<EnvBindingSEXP>>> edges;

    std::unordered_map<std::shared_ptr<EnvBindingSEXP>, size_t> allocation;

    size_t K = 4;
    size_t spillIdx = 4;
    size_t maxIdx = 0;

    virtual void begin(
        const std::vector<std::shared_ptr<EnvBindingSEXP>> &bindings,
        const std::set<std::shared_ptr<EnvBindingSEXP>> &captured) override;

    virtual void observe(
        const std::set<std::shared_ptr<EnvBindingSEXP>> &liveSet) override;

    virtual void allocate() override;

    virtual size_t allocateExtra() override;

    virtual size_t getMaxIndex() const override;
};

struct ScopeBasedRegisterAllocator : public RegisterAllocator
{
    size_t nextIdx = 0;
    size_t maxIdx = 0;

    std::vector<std::shared_ptr<EnvBindingSEXP>> ordered;

    virtual void begin(
        const std::vector<std::shared_ptr<EnvBindingSEXP>> &bindings,
        const std::set<std::shared_ptr<EnvBindingSEXP>> &captured) override;

    virtual void observe(
        const std::set<std::shared_ptr<EnvBindingSEXP>> &liveSet) override;

    virtual void allocate() override;

    virtual size_t allocateExtra() override;

    virtual size_t getMaxIndex() const override;
};
