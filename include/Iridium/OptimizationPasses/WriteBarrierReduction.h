#pragma once
#include "Iridium/Analysis/Domains/TDZA.h"
#include "Iridium/Structure/FileView.h"

class WriteBarrierReduction
{
  static bool predicateLambdaSEXP(const IRISEXP &ele)
  {
    if (std::dynamic_pointer_cast<LambdaSEXP>(ele)) return true;
    return false;
  }

  static void markCapturedBindings(IRISEXP curr, TDZA& inData)
  {
    if (auto lambdaSEXP = std::dynamic_pointer_cast<LambdaSEXP>(curr))
    {
      currFileView->updateSafelyCapturedBindingsSet(lambdaSEXP->getStartBBIDX(), inData);
    }

    for (auto e : curr->args)
      markCapturedBindings(e, inData);
  }

  static void patchExpr(IRISEXP curr, IRISEXP binding, TDZLattice & latticeVal)
  {
    if (auto envWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(curr))
    {
      if (latticeVal.kind == TDZLattice::SAFE && !envWriteSEXP->getSAFE() && envWriteSEXP->getLValTarget() == binding)
      {
        envWriteSEXP->setSAFE(true);
      }

      if (auto remEnvBindingWrite =  std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(envWriteSEXP->getLValTarget()))
      {
        if (!envWriteSEXP->getSAFE() && currFileView->safelyCapturedBindings.count(remEnvBindingWrite) > 0)
        {
          envWriteSEXP->setSAFE(true);
        }
      }

    }
    if (auto envReadSEXP = std::dynamic_pointer_cast<EnvReadSEXP>(curr))
    {
      if (auto o = std::dynamic_pointer_cast<EnvBindingSEXP>(envReadSEXP->getObj()))
      {
        if (latticeVal.kind == TDZLattice::SAFE && o == binding)
        {
          envReadSEXP->setSAFE();
        }
      }

      // Set safety for reads to remote bindings which are known to be trivially true i.e. bindings captured outside the temporal dead zone
      if (auto o = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(envReadSEXP->getObj()))
      {
        if (currFileView->safelyCapturedBindings.count(o) > 0)
        {
          envReadSEXP->setSAFE();
        }
      }
    }

    for (auto e : curr->args)
      patchExpr(e, binding, latticeVal);
  }

public:
  static FileView * currFileView;
  
  static void Transform(std::shared_ptr<BBSEXP> &bb, TDZA inData)
  {
    inData.iter(bb, [&](size_t idx, TDZA val)
                {
      for (auto &[binding, latticeVal] : val.dfv.store)
      {
        patchExpr(bb->args.at(idx), binding, latticeVal);
      } 

      if (currFileView && hasNode(bb->args.at(idx), predicateLambdaSEXP))
      {
        markCapturedBindings(bb->args.at(idx), val);
      }

    });
  }
};