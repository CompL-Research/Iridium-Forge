#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"

ConstantsAtStmt ConstantsAtStmt::transfer(const std::shared_ptr<BBSEXP> &bb) const
{
  return iter(bb, [&](size_t idx, ConstantsAtStmt val) {});
}

// ConstantsAtStmt ConstantsAtStmt::iter(const std::shared_ptr<BBSEXP> & bb, std::function<void(size_t, ConstantsAtStmt)> callback) const
// {
//   auto next = this->clone();

//   size_t idx = 0;
//   for (const auto &stmt : bb->args)
//   {
//     callback(idx++, next);
//     auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt);
//     if (!envWrite)
//       continue;

//     auto lVal = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getLValTarget());
//     if (!lVal)
//       continue;

//     // Compute lattice value for the RHS
//     ConstantLatticeValue latticeVal;

//     auto innerWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal());
//     if (innerWrite)
//     {
//       latticeVal = ConstantLatticeValue::generate(innerWrite->getRVal());
//       next.dfv.store[lVal] = latticeVal;
//       next.dfv.store[innerWrite->getLValTarget()] = latticeVal;
//     }
//     else
//     {
//       latticeVal = ConstantLatticeValue::generate(envWrite->getRVal());
//       next.dfv.store[lVal] = latticeVal;
//     }
//   }

//   return next;
// }

// #include "Iridium/Analysis/Domains/ConstantsAtStmt.h"

ConstantsAtStmt ConstantsAtStmt::iter(const std::shared_ptr<BBSEXP> & bb, std::function<void(size_t, ConstantsAtStmt)> callback) const
{
  auto next = this->clone();

  size_t idx = 0;
  for (const auto &stmt : bb->args)
  {
    auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt);
    if (!envWrite)
    {
      // non-write stmt: nothing changes, but still let caller observe the state
      callback(idx++, next);
      continue;
    }

    auto lVal = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getLValTarget());
    if (!lVal)
    {
      // LHS not a binding we care about
      callback(idx++, next);
      continue;
    }

    // Compute lattice value for the RHS.
    ConstantLatticeValue latticeVal;

    // Case 1: RHS itself is an EnvWrite (e.g. x = (y = 3)).
    if (auto innerWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
    {
      // evaluate inner RHS
      latticeVal = ConstantLatticeValue::generate(innerWrite->getRVal());

      // update the inner LHS if it's a binding
      if (auto innerLVal = std::dynamic_pointer_cast<EnvBindingSEXP>(innerWrite->getLValTarget()))
      {
        next.dfv.store[innerLVal] = latticeVal;
      }

      // outer LHS gets the same value
      next.dfv.store[lVal] = latticeVal;
    }
    else
    {
      // Case 2: RHS is a direct binding reference (x = y) -> forward y's lattice if present
      if (auto rhsBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getRVal()))
      {
        auto it = next.dfv.store.find(rhsBinding);
        if (it != next.dfv.store.end())
        {
          latticeVal = it->second; // propagate existing lattice for y
        }
        else
        {
          // fallback to generating from the expression (literal, call, etc.)
          latticeVal = ConstantLatticeValue::generate(envWrite->getRVal());
        }
      }
      else
      {
        // generic expression on RHS (literal, op, call, ...).
        latticeVal = ConstantLatticeValue::generate(envWrite->getRVal());
      }

      next.dfv.store[lVal] = latticeVal;
    }

    // Observe the state *after* the statement (important).
    callback(idx++, next);
  }

  return next;
}