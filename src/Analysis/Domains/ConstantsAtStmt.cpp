#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"

std::set<std::shared_ptr<EnvBindingSEXP>> ConstantsAtStmt::blacklist;

ConstantsAtStmt ConstantsAtStmt::transfer(const std::shared_ptr<BBSEXP> &bb) const
{
  return iter(bb, [&](size_t idx, ConstantsAtStmt val) {});
}

ConstantsAtStmt ConstantsAtStmt::iter(const std::shared_ptr<BBSEXP> &bb, std::function<void(size_t, ConstantsAtStmt)> callback) const
{
  auto next = this->clone();

  size_t idx = 0;
  for (const auto &stmt : bb->args)
  {
    auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt);
    if (!envWrite)
    {
      for (const auto &blacklistedVar : blacklist)
      {
        if (next.dfv.store.count(blacklistedVar))
        {
          next.dfv.store[blacklistedVar] = ConstantLatticeValue{ConstantLatticeValue::NAC, NULL};
        }
      }
      callback(idx++, next);
      continue;
    }

    auto process_write = [&](const IRISEXP &lval_target, const IRISEXP &rval)
    {
      auto lVal = std::dynamic_pointer_cast<EnvBindingSEXP>(lval_target);
      if (!lVal) return;

      if (blacklist.count(lVal))
      {
        next.dfv.store[lVal] = ConstantLatticeValue{ConstantLatticeValue::NAC, NULL};
        return;
      }

      ConstantLatticeValue latticeVal;
      if (auto rhsBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(rval))
      {
        auto it = next.dfv.store.find(rhsBinding);
        latticeVal = (it != next.dfv.store.end()) ? it->second : ConstantLatticeValue::generate(rval);
      }
      else
      {
        latticeVal = ConstantLatticeValue::generate(rval);
      }
      next.dfv.store[lVal] = latticeVal;
    };

    if (auto innerWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
    {
      process_write(innerWrite->getLValTarget(), innerWrite->getRVal());
      process_write(envWrite->getLValTarget(), innerWrite->getRVal());
    }
    else
    {
      process_write(envWrite->getLValTarget(), envWrite->getRVal());
    }

    callback(idx++, next);
  }

  return next;
}
