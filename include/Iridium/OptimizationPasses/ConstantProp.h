#pragma once
#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"

class ConstantProp
{
  static std::shared_ptr<IridiumSEXP> makeConstant(const ConstantLatticeValue &val)
  {
    switch (val.kind)
    {
      case ConstantLatticeValue::Null:
        return std::make_shared<NullSEXP>(true); // vestigial

      case ConstantLatticeValue::Boolean: {
        auto booleanVal = std::dynamic_pointer_cast<BooleanSEXP>(val.value);
        assert(booleanVal);
        return std::make_shared<BooleanSEXP>(booleanVal->getIridiumPrimitive());
      }

      case ConstantLatticeValue::Number: {
        auto numberVal = std::dynamic_pointer_cast<NumberSEXP>(val.value);
        assert(numberVal);
        return std::make_shared<NumberSEXP>(numberVal->getIridiumPrimitive());
      }

      case ConstantLatticeValue::String: {
        auto stringVal = std::dynamic_pointer_cast<StringSEXP>(val.value);
        assert(stringVal);
        return std::make_shared<StringSEXP>(stringVal->getIridiumPrimitive());
      }

      case ConstantLatticeValue::JSBigInt: {
        auto bigIntVal = std::dynamic_pointer_cast<BitIntSEXP>(val.value);
        assert(bigIntVal);
        return std::make_shared<BitIntSEXP>(bigIntVal->getIridiumPrimitive());
      }

      case ConstantLatticeValue::NAC:
      case ConstantLatticeValue::NUBD:
      case ConstantLatticeValue::BOTTOM:
        throw std::runtime_error("Unreachable makeConstant");
        return nullptr; // not a constant we can propagate
    }
    throw std::runtime_error("Unreachable makeConstant");
    return nullptr; // defensive
  }

  static void patchExpr(IRISEXP expr, IRISEXP binding, const ConstantLatticeValue &val)
  {
    for (size_t i = 0; i < expr->args.size(); i++)
    {
      auto &curr = expr->args[i];
      if (auto envReadNode = std::dynamic_pointer_cast<EnvReadSEXP>(curr))
      {
        if (envReadNode->getObj() == binding)
        {
          if (auto replacement = makeConstant(val))
            expr->args[i] = replacement;
        }
      }
      patchExpr(expr->args[i], binding, val);
    }
  }

public:
  static void Transform(std::shared_ptr<BBSEXP> &bb, ConstantsAtStmt inData)
  {
    inData.iter(bb, [&](size_t idx, ConstantsAtStmt val)
    {
      for (auto &[binding, latticeVal] : val.dfv.store)
      {
        if (latticeVal.kind == ConstantLatticeValue::NAC ||
            latticeVal.kind == ConstantLatticeValue::NUBD)
          continue;

        assert(latticeVal.kind != ConstantLatticeValue::BOTTOM);

        // Recursively patch AST for the current statement
        patchExpr(bb->args.at(idx), binding, latticeVal);
      }
    });
  }
};