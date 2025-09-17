#include "Iridium/Analysis/Domains/TDZA.h"

TDZA TDZA::transfer(const std::shared_ptr<BBSEXP> &bb) const
{
  return iter(bb, [&](size_t idx, TDZA val) {});
}

TDZA TDZA::iter(const std::shared_ptr<BBSEXP> &bb, std::function<void(size_t, TDZA)> callback) const
{
  auto next = this->clone();

  size_t idx = 0;

  for (const auto &stmt : bb->args)
  {
    callback(idx++, next);
    if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
    {
      TDZLattice RVAL;
      if (auto innerEnvWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
      {
        RVAL = TDZLattice::generate(innerEnvWrite->getRVal());
      }
      else
      {
        RVAL = TDZLattice::generate(envWrite->getRVal());
      }

      if (auto outerWriteTarget = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getLValTarget()))
      {
        if (envWrite->getSAFE())
        {
          if (next.dfv.store.count(outerWriteTarget) == 0)
            continue;
          next.dfv.store[outerWriteTarget] = RVAL;
        }
      }

      if (auto innerEnvWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
      {
        if (auto innerWriteTarget = std::dynamic_pointer_cast<EnvBindingSEXP>(innerEnvWrite->getLValTarget()))
        {
          if (innerEnvWrite->getSAFE())
          {
            if (next.dfv.store.count(innerWriteTarget) == 0)
              continue;
            next.dfv.store[innerWriteTarget] = RVAL;
          }
        }
      }
    }
  }
  return next;
}

// TDZA TDZA::transfer(const std::shared_ptr<BBSEXP> &bb) const
// {
//   auto next = this->clone();

//   for (const auto &stmt : bb->args)
//   {

//     if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
//     {
//       TDZLattice RVAL;
//       if (auto innerEnvWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
//       {
//         RVAL = TDZLattice::generate(innerEnvWrite->getRVal());
//       }
//       else
//       {
//         RVAL = TDZLattice::generate(envWrite->getRVal());
//       }

//       if (auto outerWriteTarget = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getLValTarget()))
//       {
//         if (envWrite->getSAFE())
//         {
//           if (next.dfv.store.count(outerWriteTarget) == 0)
//             continue;
//           next.dfv.store[outerWriteTarget] = RVAL;
//         }
//       }

//       if (auto innerEnvWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
//       {
//         if (auto innerWriteTarget = std::dynamic_pointer_cast<EnvBindingSEXP>(innerEnvWrite->getLValTarget()))
//         {
//           if (innerEnvWrite->getSAFE())
//           {
//             if (next.dfv.store.count(innerWriteTarget) == 0)
//               continue;
//             next.dfv.store[innerWriteTarget] = RVAL;
//           }
//         }
//       }
//     }
//   }
//   return next;
// }

// TDZA TDZA::transferDump(const std::shared_ptr<BBSEXP> &bb, std::ostringstream &oss) const
// {
//   auto next = this->clone();

//   for (const auto &stmt : bb->args)
//   {
//     oss << std::endl << "DFV: " << std::endl;
//     next.dump(oss);
//     oss << std::endl;
//     oss << "STMT: " << std::endl;
//     stmt->prettyPrint(oss);

//     if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
//     {
//       TDZLattice RVAL;
//       if (auto innerEnvWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
//       {
//         RVAL = TDZLattice::generate(innerEnvWrite->getRVal());
//       }
//       else
//       {
//         RVAL = TDZLattice::generate(envWrite->getRVal());
//       }

//       if (auto outerWriteTarget = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getLValTarget()))
//       {
//         if (envWrite->getSAFE())
//         {
//           if (next.dfv.store.count(outerWriteTarget) == 0) continue;
//           next.dfv.store[outerWriteTarget] = RVAL;
//         }
//       }

//       if (auto innerEnvWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
//       {
//         if (auto innerWriteTarget = std::dynamic_pointer_cast<EnvBindingSEXP>(innerEnvWrite->getLValTarget()))
//         {
//           if (innerEnvWrite->getSAFE())
//           {
//             if (next.dfv.store.count(innerWriteTarget) == 0) continue;
//             next.dfv.store[innerWriteTarget] = RVAL;
//           }
//         }
//       }
//     }
//   }
//   oss << std::endl << "DFV: " << std::endl;
//   next.dump(oss);
//   oss << std::endl << std::endl;
//   return next;
// }