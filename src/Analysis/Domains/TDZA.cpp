#include "Iridium/Analysis/Domains/TDZA.h"

TDZA TDZA::transfer(const std::shared_ptr<BBSEXP> &bb) const
{
  auto next = this->clone();

  for (const auto &stmt : bb->args)
  {

    if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
    {
      IRISEXP RVAL;
      if (auto innerEnvWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
      {
        RVAL = innerEnvWrite->getRVal();
      }
      else
      {
        RVAL = envWrite->getRVal();
      }

      if (auto outerWriteTarget = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getLValTarget()))
      {
        if (envWrite->getSAFE())
        {
          if (next.dfv.store.count(outerWriteTarget) == 0) continue;
          next.dfv.store[outerWriteTarget] = TDZLattice::generate(RVAL);
        }
      }
      
      if (auto innerEnvWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
      {
        if (auto innerWriteTarget = std::dynamic_pointer_cast<EnvBindingSEXP>(innerEnvWrite->getLValTarget()))
        {
          if (innerEnvWrite->getSAFE())
          {
            if (next.dfv.store.count(innerWriteTarget) == 0) continue;
            next.dfv.store[innerWriteTarget] = TDZLattice::generate(RVAL);
          }
        }
      }
    }
  }
  return next;
}