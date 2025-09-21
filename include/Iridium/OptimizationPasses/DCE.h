#pragma once
#include "Iridium/Analysis/Domains/Liveness.h"

class DCE
{

  static bool maybeSideEffect(IRISEXP rVAL)
  {
    if (
      std::dynamic_pointer_cast<EnvReadSEXP>(rVAL) ||
      std::dynamic_pointer_cast<BooleanSEXP>(rVAL) ||
      std::dynamic_pointer_cast<LambdaSEXP>(rVAL) ||
      std::dynamic_pointer_cast<NullSEXP>(rVAL) ||
      std::dynamic_pointer_cast<NumberSEXP>(rVAL) ||
      std::dynamic_pointer_cast<RegExpSEXP>(rVAL) ||
      std::dynamic_pointer_cast<StringSEXP>(rVAL) ||
      std::dynamic_pointer_cast<BitIntSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSNUBDSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSObjectSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSPrivateSEXP>(rVAL)
    ) return false;
    return true;
  }

  static IRISEXP patchExpr(IRISEXP curr, const Liveness &val)
  {
    // TODO
    // If noSideEffect(RVAL): StackReject[1](RVAL) => NOP
    // 

    if (auto implicitBindingDecl = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(curr))
    {
      if (auto LVAL = std::dynamic_pointer_cast<EnvBindingSEXP>(implicitBindingDecl->getStore()))
      {
        if (val.dfv.count(LVAL) == 0)
        {
          return std::make_shared<NOPSEXP>();
        }
      }
    }

    if (auto envWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(curr))
    {

      // // LVAL = (ILVAL = RVAL) => LVAL = (RVAL)
      // if (auto innerEnvWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(envWriteSEXP->getRVal()))
      // {
      //   if (innerEnvWriteSEXP->getSAFE())
      //   {
      //     if (auto LVAL = std::dynamic_pointer_cast<EnvBindingSEXP>(innerEnvWriteSEXP->getLValTarget()))
      //     {
      //       if (val.dfv.count(LVAL) == 0)
      //       {
      //         // std::cout << "KILLING (inner): "; 
      //         // curr->prettyPrint(std::cout);
      //         // std::cout << std::endl;
  
      //         envWriteSEXP->setRVal(innerEnvWriteSEXP->getRVal());
              
      //         // std::cout << "AFTER KILLING (inner): "; 
      //         // curr->prettyPrint(std::cout);
      //         // std::cout << std::endl;
      //       }
      //     }
      //   }
      // }

      if (!envWriteSEXP->getSAFE()) return curr;


      // LVAL = [RVAL] => StackReject[1]([RVAL])
      if (auto LVAL = std::dynamic_pointer_cast<EnvBindingSEXP>(envWriteSEXP->getLValTarget()))
      {
        if (val.dfv.count(LVAL) == 0)
        {
          // std::cout << "KILLING (outer): "; 
          // curr->prettyPrint(std::cout);
          // std::cout << std::endl;

          if (auto innerEnvWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(envWriteSEXP->getRVal()))
          {
            return envWriteSEXP->getRVal();
          }

          if (!maybeSideEffect(envWriteSEXP->getRVal()))
          {
            return std::make_shared<NOPSEXP>();
          }

          auto res = std::make_shared<StackRejectSEXP>(1);
          res->args.push_back(envWriteSEXP->getRVal());

          // std::cout << "AFTER KILLING (outer): "; 
          // res->prettyPrint(std::cout);
          // std::cout << std::endl;

          return res;
        }
      }
    }

    return curr;
  }

public:
  static void Transform(std::shared_ptr<BBSEXP> &bb, Liveness inData)
  {
    inData.iter(bb, [&](size_t idx, const Liveness &val)
                { bb->args.at(idx) = patchExpr(bb->args.at(idx), val); });
  }
};