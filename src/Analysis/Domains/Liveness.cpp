#include "Iridium/Analysis/Domains/Liveness.h"

Liveness Liveness::transfer(const std::shared_ptr<BBSEXP> &bb) const
{
  return iter(bb, [&](size_t idx, Liveness val) {});
}

static void populateUsesAndDefs(IRISEXP currSEXP, std::set<std::shared_ptr<EnvBindingSEXP>> &uses, std::set<std::shared_ptr<EnvBindingSEXP>> &defs)
{
  if (auto envRead = std::dynamic_pointer_cast<EnvReadSEXP>(currSEXP))
  {
    if (auto binding = std::dynamic_pointer_cast<EnvBindingSEXP>(envRead->getObj()))
      uses.insert(binding);
  }
  else if (auto envBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(currSEXP))
  {
    defs.insert(envBinding);
  }
  else
  {
    for (auto e : currSEXP->args)
      populateUsesAndDefs(e, uses, defs);
  }
}

Liveness Liveness::iter(
    const std::shared_ptr<BBSEXP> &bb,
    std::function<void(size_t, Liveness)> callback) const
{
  auto next = this->clone();

  // Total number of statements
  size_t n = bb->args.size();

  // Walk statements in reverse (backward analysis)
  for (size_t i = n; i-- > 0;)
  {
    const auto &stmt = bb->args[i];

    std::set<std::shared_ptr<EnvBindingSEXP>> uses;
    std::set<std::shared_ptr<EnvBindingSEXP>> defs;
    populateUsesAndDefs(stmt, uses, defs);

    // Apply transfer: IN = USE ∪ (OUT − DEF)
    Liveness updated;
    updated.dfv = next.dfv;

    for (auto &d : defs)
      updated.dfv.erase(d);

    updated.dfv.insert(uses.begin(), uses.end());

    next = std::move(updated);

    // Callback with the *real* statement index
    callback(i, next);
  }

  return next;
}