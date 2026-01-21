#include "Iridium/Analysis/Domains/Liveness.h"

std::set<std::shared_ptr<EnvBindingSEXP>> Liveness::blacklist;

Liveness Liveness::transfer(const std::shared_ptr<BBSEXP> &bb) const
{
  return iter(bb, [&](size_t idx, Liveness val) {});
}

static void populateUsesAndDefs(IRISEXP currSEXP, std::set<std::shared_ptr<EnvBindingSEXP>> &uses, std::set<std::shared_ptr<EnvBindingSEXP>> &defs)
{
  if (auto envWriteStmt = std::dynamic_pointer_cast<EnvWriteSEXP>(currSEXP))
  { // Unsafe writes to EnvBindingSEXPs create a read.
    if (auto envBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(envWriteStmt->getLValTarget()))
    {
      if (envWriteStmt->getTHISINIT()) {
        uses.insert(envBinding);
      } 
      else if (envWriteStmt->getSAFE())
      {
        defs.insert(envBinding);
      }
      else
      {
        uses.insert(envBinding);
      }
    }

    populateUsesAndDefs(envWriteStmt->getRVal(), uses, defs);
    return;
  }

  if (auto stackReject = std::dynamic_pointer_cast<StackRejectSEXP>(currSEXP))
  { // Reads to effect less safe bindings are trivially true TDZ checks, we early return...
    if (auto envRead = std::dynamic_pointer_cast<EnvReadSEXP>(stackReject->args.at(0)))
    {
      if (envRead->hasSAFE())
      {
        return;
      }
    }
  }

  // if (auto stackToHeap = std::dynamic_pointer_cast<StackToHeapSEXP>(currSEXP))
  // {
  //   for (auto & bindingSEXP : stackToHeap->args) {
  //     if (auto binding = std::dynamic_pointer_cast<EnvBindingSEXP>(bindingSEXP))
  //     {
  //       uses.insert(binding);
  //     } else throw std::runtime_error("expected EnvBinding inside StackToHeap Node");
  //   }
  // }

  if (auto implicitDecl = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(currSEXP))
  {
    if (auto envBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(implicitDecl->getStore()))
    {
      if (implicitDecl->getSAFE())
        defs.insert(envBinding);
      else
        uses.insert(envBinding);
    }

    for (auto b : implicitDecl->getArgs()->args) {
      if (auto pArg = std::dynamic_pointer_cast<EnvBindingSEXP>(b))
      {
        uses.insert(pArg);
      }
    }

    return;
  }

  if (auto binding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(currSEXP))
    return;

  if (auto binding = std::dynamic_pointer_cast<EnvBindingSEXP>(currSEXP))
    uses.insert(binding);

  for (auto e : currSEXP->args)
    populateUsesAndDefs(e, uses, defs);
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
    // Callback with the *real* statement index
    callback(i, next);

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
    updated.dfv.insert(blacklist.begin(), blacklist.end());

    next = std::move(updated);
  }

  return next;
}

Liveness Liveness::iterAlt(
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
    updated.dfv.insert(blacklist.begin(), blacklist.end());

    next = std::move(updated);

    // Callback with the *real* statement index
    callback(i, next);
  }

  return next;
}