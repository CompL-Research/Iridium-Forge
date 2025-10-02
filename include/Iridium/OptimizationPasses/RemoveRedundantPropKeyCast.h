#pragma once
#include "Iridium/Analysis/Domains/SetSafePropKeyAccesses.h"

class RemoveRedundantPropKeyCast
{

  static void patchExpr(IRISEXP curr, SetSafePropKeyAccesses &flowVal)
  {
    if (auto computedFieldRead = std::dynamic_pointer_cast<JSComputedFieldReadSEXP>(curr))
    {
      if (PropKeyLatticeValue::isSafe(computedFieldRead->getField(), flowVal.dfv))
      {
        // std::cout << "JSComputedFieldReadSEXP - removed field cast" << std::endl;
        computedFieldRead->setSAFE();
      }
      else
      {
        computedFieldRead->unsetSAFE();
      }
    }

    if (auto computedFieldWrite = std::dynamic_pointer_cast<JSComputedFieldWriteSEXP>(curr))
    {
      if (PropKeyLatticeValue::isSafe(computedFieldWrite->getField(), flowVal.dfv))
      {
        // std::cout << "JSComputedFieldWriteSEXP - removed field cast" << std::endl;
        computedFieldWrite->setSAFE();
      }
      else
      {
        computedFieldWrite->unsetSAFE();
      }
    }
    for (auto &e : curr->args)
      patchExpr(e, flowVal);
  }

public:
  static void Transform(std::shared_ptr<BBSEXP> &bb, SetSafePropKeyAccesses inData)
  {
    inData.iter(bb, [&](size_t idx, SetSafePropKeyAccesses val)
                { patchExpr(bb->args.at(idx), val); });
  }
};