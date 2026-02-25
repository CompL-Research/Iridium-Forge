#include "Iridium/OptimizationPasses/ReduceComputedFieldOpsPass.h"
#include <cmath>

std::string ReduceComputedFieldOpsPass::name() const
{
  return "ReduceComputedFieldOps";
}

static bool hasFractionalPart(double value)
{
  return std::trunc(value) != value;
}

bool ReduceComputedFieldOpsPass::patchExpr(IRISEXP curr)
{
  bool changed = false;

  for (size_t i = 0; i < curr->args.size(); i++)
  {
    auto &child = curr->args[i];

    // -------- READ --------
    if (auto read = std::dynamic_pointer_cast<JSComputedFieldReadSEXP>(child))
    {
      auto field = read->getField();

      std::shared_ptr<StringSEXP> key;

      if (auto b = std::dynamic_pointer_cast<BooleanSEXP>(field))
        key = std::make_shared<StringSEXP>(b->getIridiumPrimitive() ? "true" : "false");

      else if (std::dynamic_pointer_cast<NullSEXP>(field))
        key = std::make_shared<StringSEXP>("null");

      else if (auto s = std::dynamic_pointer_cast<StringSEXP>(field))
        key = std::make_shared<StringSEXP>(s->getIridiumPrimitive());

      if (key)
      {
        child = std::make_shared<FieldReadSEXP>(
            read->getObj(), key);
        changed = true;
      }
    }

    // -------- WRITE --------
    if (auto write = std::dynamic_pointer_cast<JSComputedFieldWriteSEXP>(child))
    {
      auto field = write->getField();

      std::shared_ptr<StringSEXP> key;

      if (auto b = std::dynamic_pointer_cast<BooleanSEXP>(field))
        key = std::make_shared<StringSEXP>(b->getIridiumPrimitive() ? "true" : "false");

      else if (std::dynamic_pointer_cast<NullSEXP>(field))
        key = std::make_shared<StringSEXP>("null");

      else if (auto s = std::dynamic_pointer_cast<StringSEXP>(field))
        key = std::make_shared<StringSEXP>(s->getIridiumPrimitive());

      if (key)
      {
        child = std::make_shared<FieldWriteSEXP>(
            write->getObj(), key, write->getValue());
        changed = true;
      }
    }

    // Recurse
    changed |= patchExpr(child);
  }

  return changed;
}

bool ReduceComputedFieldOpsPass::runOnContainer(BBContainerView &bb)
{
  bool changed = false;

  bb.cfgManager.traverseCFG(
      [&](Vertex v, std::shared_ptr<BBSEXP> block)
      {
        for (auto &stmt : block->args)
          changed |= patchExpr(stmt);
      });

  return changed;
}

bool ReduceComputedFieldOpsPass::run(BBContainerView &bb,
                                     FileView &,
                                     AnalysisManager &)
{
  return runOnContainer(bb);
}