#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"

ConstantsAtStmt ConstantsAtStmt::transfer(const std::shared_ptr<BBSEXP> &bb) const
{
  return iter(bb, [&](size_t idx, ConstantsAtStmt val) {});
}

ConstantsAtStmt ConstantsAtStmt::iter(const std::shared_ptr<BBSEXP> & bb, std::function<void(size_t, ConstantsAtStmt)> callback) const
{
  auto next = this->clone();

  size_t idx = 0;
  for (const auto &stmt : bb->args)
  {
    callback(idx++, next);
    auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt);
    if (!envWrite)
      continue;

    auto lVal = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getLValTarget());
    if (!lVal)
      continue;

    // Compute lattice value for the RHS
    ConstantLatticeValue latticeVal;

    auto innerWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal());
    if (innerWrite)
    {
      latticeVal = ConstantLatticeValue::generate(innerWrite->getRVal());
      next.dfv.store[lVal] = latticeVal;
      next.dfv.store[innerWrite->getLValTarget()] = latticeVal;
    }
    else
    {
      latticeVal = ConstantLatticeValue::generate(envWrite->getRVal());
      next.dfv.store[lVal] = latticeVal;
    }
  }

  return next;
}