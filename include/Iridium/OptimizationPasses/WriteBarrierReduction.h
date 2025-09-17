#pragma once
#include "Iridium/Analysis/Domains/TDZA.h"

class WriteBarrierReduction
{
  static void patchExpr(IRISEXP curr, IRISEXP binding)
  {
    if (auto envWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(curr))
    {
      if (!envWriteSEXP->getSAFE() && envWriteSEXP->getLValTarget() == binding)
      {
        envWriteSEXP->setSAFE(true);
      }
      if (std::dynamic_pointer_cast<EnvWriteSEXP>(envWriteSEXP->getRVal())) patchExpr(envWriteSEXP->getRVal(), binding);
    }
  }

public:
  static void Transform(std::shared_ptr<BBSEXP> &bb, TDZA inData)
  {
    inData.iter(bb, [&](size_t idx, TDZA val)
    {
      for (auto &[binding, latticeVal] : val.dfv.store)
      {
        // assert(latticeVal.kind != TDZLattice::BOTTOM);
        if (latticeVal.kind == TDZLattice::SAFE)
          patchExpr(bb->args.at(idx), binding);
          // continue;

        // Recursively patch AST for the current statement
      }
    });
  }
};