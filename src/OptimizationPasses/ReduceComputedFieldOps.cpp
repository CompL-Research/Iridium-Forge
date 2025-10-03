#include "Iridium/OptimizationPasses/ReduceComputedFieldOps.h"
#include <cmath>

bool hasFractionalPart(double value) {
    return std::trunc(value) != value;
}
//
// Reduce JSComputedField operations into Field operations if possible
//

void patchExpr(IRISEXP currSEXP)
{
  for (size_t i = 0; i < currSEXP->args.size(); i++) {
    auto curr = currSEXP->args.at(i);
    if (auto computedFieldRead = std::dynamic_pointer_cast<JSComputedFieldReadSEXP>(curr))
    {
      // BooleanSEXP
      // | NullSEXP
      // | NumberSEXP
      // | StringSEXP
      auto currField = computedFieldRead->getField();
      if (auto booleanField = std::dynamic_pointer_cast<BooleanSEXP>(currField))
      {
        // std::cout << "Reduced Computed Field Read -> Boolean" << std::endl;
        currSEXP->args.at(i) = std::make_shared<FieldReadSEXP>(
          computedFieldRead->getObj(), 
          std::make_shared<StringSEXP>(booleanField->getIridiumPrimitive() ? "true" : "false")
        );
      }
      else if (auto nullObj = std::dynamic_pointer_cast<NullSEXP>(currField))
      {
        // std::cout << "Reduced Computed Field Read -> Null" << std::endl;
        currSEXP->args.at(i) = std::make_shared<FieldReadSEXP>(
          computedFieldRead->getObj(), 
          std::make_shared<StringSEXP>("null")
        );
      }
      else if (auto numberObj = std::dynamic_pointer_cast<NumberSEXP>(currField))
      {
        double numberStored = numberObj->getIridiumPrimitive();
        if (!hasFractionalPart(numberStored))
        {
          // std::cout << "Reduced Computed Field Read -> Number (non fractional)" << std::endl;
          currSEXP->args.at(i) = std::make_shared<FieldReadSEXP>(
            computedFieldRead->getObj(), 
            std::make_shared<StringSEXP>(std::to_string(static_cast<int>(std::trunc(numberStored))))
          );
        }
        else
        {
          // std::cout << "Reduced Computed Field Read -> Number (fractional)" << std::endl;
          currSEXP->args.at(i) = std::make_shared<FieldReadSEXP>(
            computedFieldRead->getObj(), 
            std::make_shared<StringSEXP>(std::to_string(numberStored))
          );
        }
      }
      else if (auto stringObj = std::dynamic_pointer_cast<StringSEXP>(currField))
      {
        // std::cout << "Reduced Computed Field Read -> String" << std::endl;
        currSEXP->args.at(i) = std::make_shared<FieldReadSEXP>(
          computedFieldRead->getObj(), 
          std::make_shared<StringSEXP>(stringObj->getIridiumPrimitive())
        );
      }
    }

    if (auto computedFieldWrite = std::dynamic_pointer_cast<JSComputedFieldWriteSEXP>(curr))
    {
      // BooleanSEXP
      // | NullSEXP
      // | NumberSEXP
      // | StringSEXP
      auto currField = computedFieldWrite->getField();
      if (auto booleanField = std::dynamic_pointer_cast<BooleanSEXP>(currField))
      {
        // std::cout << "Reduced Computed Field Write -> Boolean" << std::endl;
        currSEXP->args.at(i) = std::make_shared<FieldWriteSEXP>(
          computedFieldWrite->getObj(), 
          std::make_shared<StringSEXP>(booleanField->getIridiumPrimitive() ? "true" : "false"),
          computedFieldWrite->getValue()
        );
      }
      else if (auto nullObj = std::dynamic_pointer_cast<NullSEXP>(currField))
      {
        // std::cout << "Reduced Computed Field Write -> Null" << std::endl;
        currSEXP->args.at(i) = std::make_shared<FieldWriteSEXP>(
          computedFieldWrite->getObj(), 
          std::make_shared<StringSEXP>("null"),
          computedFieldWrite->getValue()
        );
      }
      else if (auto numberObj = std::dynamic_pointer_cast<NumberSEXP>(currField))
      {
        double numberStored = numberObj->getIridiumPrimitive();
        if (!hasFractionalPart(numberStored))
        {
          // std::cout << "Reduced Computed Field Write -> Number (non fractional)" << std::endl;
          currSEXP->args.at(i) = std::make_shared<FieldWriteSEXP>(
            computedFieldWrite->getObj(), 
            std::make_shared<StringSEXP>(std::to_string(static_cast<int>(std::trunc(numberStored)))),
          computedFieldWrite->getValue()
          );
        }
        else
        {
          // std::cout << "Reduced Computed Field Write -> Number (fractional)" << std::endl;
          currSEXP->args.at(i) = std::make_shared<FieldWriteSEXP>(
            computedFieldWrite->getObj(), 
            std::make_shared<StringSEXP>(std::to_string(numberStored)),
          computedFieldWrite->getValue()
          );
        }
      }
      else if (auto stringObj = std::dynamic_pointer_cast<StringSEXP>(currField))
      {
        // std::cout << "Reduced Computed Field Write -> String" << std::endl;
        currSEXP->args.at(i) = std::make_shared<FieldWriteSEXP>(
          computedFieldWrite->getObj(), 
          std::make_shared<StringSEXP>(stringObj->getIridiumPrimitive()),
          computedFieldWrite->getValue()
        );
      }
    }

    // Speedup here by skipping cases...
    return patchExpr(curr);
  }
}

void reduceComputedFieldOps(BBContainerView &bbContView)
{
  bbContView.cfgManager.traverseCFG(
  [&](Vertex v, std::shared_ptr<BBSEXP> bb)
  {
    for (auto & stmt : bb->args)
    {
      patchExpr(stmt);
    }
  });
}