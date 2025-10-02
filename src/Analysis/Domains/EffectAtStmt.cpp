#include "Iridium/Globals.h"
#include "Iridium/Analysis/Domains/EffectAtStmt.h"

std::set<std::shared_ptr<EnvBindingSEXP>> EffectAtStmt::blacklist;

EffectAtStmt EffectAtStmt::transfer(const std::shared_ptr<BBSEXP> &bb) const
{
  return iter(bb, [&](size_t idx, EffectAtStmt &val) {});
}

static bool noSideEffect(IRISEXP rVAL)
{
  return (
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
      std::dynamic_pointer_cast<JSPrivateSEXP>(rVAL));
}
static bool supportedTransferEffect(IRISEXP rVAL)
{
  return true;
  return (
      // // Calls
      // std::dynamic_pointer_cast<CallSiteSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSADDBRANDSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSCheckConstructorSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSForInNextSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSForInStartSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSForOfIteratorCloseSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSForOfNextSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSForOfStartSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSAppendSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSCopyDataPropertiesSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSDefineObjMethodSEXP>(rVAL) ||

      // possible direct reads and writes to the env
      std::dynamic_pointer_cast<JSComputedFieldReadSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSComputedFieldWriteSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSPrivateFieldReadSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSPrivateFieldWriteSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSSuperFieldReadSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSSuperFieldWriteSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<EnvWriteSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<BinopSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSBinopSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<UnopSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<JSUnopSEXP>(rVAL) ||
      std::dynamic_pointer_cast<FieldReadSEXP>(rVAL)
      // std::dynamic_pointer_cast<FieldWriteSEXP>(rVAL)

      // // Stack effect
      // std::dynamic_pointer_cast<AwaitSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<YieldSEXP>(rVAL) ||
      // std::dynamic_pointer_cast<StackPopSEXP>(rVAL) ||

      // // Branch
      // std::dynamic_pointer_cast<IfElseJumpSEXP>(rVAL)
  );
}
static bool maybeEffect(IRISEXP rVAL)
{
  return (
      // Calls
      std::dynamic_pointer_cast<CallSiteSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSADDBRANDSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSCheckConstructorSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSForInNextSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSForInStartSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSForOfIteratorCloseSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSForOfNextSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSForOfStartSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSAppendSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSCopyDataPropertiesSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSDefineObjMethodSEXP>(rVAL) ||

      // possible direct reads and writes to the env
      std::dynamic_pointer_cast<JSComputedFieldReadSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSComputedFieldWriteSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSPrivateFieldReadSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSPrivateFieldWriteSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSSuperFieldReadSEXP>(rVAL) ||
      std::dynamic_pointer_cast<JSSuperFieldWriteSEXP>(rVAL) ||
      std::dynamic_pointer_cast<EnvWriteSEXP>(rVAL) ||
      std::dynamic_pointer_cast<FieldReadSEXP>(rVAL) ||
      std::dynamic_pointer_cast<FieldWriteSEXP>(rVAL) ||

      // Stack effect
      std::dynamic_pointer_cast<AwaitSEXP>(rVAL) ||
      std::dynamic_pointer_cast<YieldSEXP>(rVAL) ||
      std::dynamic_pointer_cast<StackPopSEXP>(rVAL) ||

      // Branch
      std::dynamic_pointer_cast<IfElseJumpSEXP>(rVAL));
}

EffectAtStmt EffectAtStmt::iter(const std::shared_ptr<BBSEXP> &bb, std::function<void(size_t, EffectAtStmt &)> callback) const
{
  auto next = this->clone();
  next.bottom = false;

  auto anyReadsOrWritesToCapturedVariablesPredicate = [&](IRISEXP val)
  {
    if (auto b = std::dynamic_pointer_cast<EnvBindingSEXP>(val))
    {
      if (blacklist.count(b) > 0)
        return true;
    }
    return false;
  };

  size_t idx = 0;
  for (const auto &stmt : bb->args)
  {
    callback(idx++, next);

    // For effects stored in the form:
    // Case LVAL = RVAL:
    //    If typeof(LVAL) == LocalStackBinding && RVAL is a SupportedEffect
    //      - Update Lattice: LVAL = RVAL
    //    Else If { ValidEffect && (AnyWritesToBindingsUsedInEffect || AnyReadsToStore || AnyReadsOrWritesToCapturedVariables || RVALmaybeEffectful) }
    //      - Update Latice: KILL
    // Base:
    //    If {maybeEffectful || AnyReadsOrWritesToCapturedVariables}
    //      - Update Lattice: KILL

    if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
    {
      auto LVAL = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getLValTarget());
      if (LVAL && supportedTransferEffect(envWrite->getRVal()))
      {
        next.bottom = false;
        next.validEffect = true;
        next.store = LVAL;
        next.effect = envWrite->getRVal();
        next.stmt = stmt;
      }
      else if (next.validEffect)
      {
        auto bindingsUsedInEffectPredicate = [&](IRISEXP val)
        {
          auto envWrite = std::dynamic_pointer_cast<EnvBindingSEXP>(val);
          auto remoteEnvWrite = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(val);
          auto global = std::dynamic_pointer_cast<GlobalBindingSEXP>(val);
          return envWrite || remoteEnvWrite || global;
        };

        std::set<IRISEXP> bindingsUsedInEffect;
        getAllNodes(envWrite->getRVal(), bindingsUsedInEffectPredicate, bindingsUsedInEffect);

        for (auto &bUsedInEffect : bindingsUsedInEffect)
        {
          if (bUsedInEffect == LVAL) // AnyWritesToBindingsUsedInEffect
          {
            next.bottom = false;
            next.validEffect = false;
            next.store = NULL;
            next.effect = NULL;
            next.stmt = NULL;
            continue;
          }
        }

        auto anyReadsToStorePredicate = [&](IRISEXP val)
        {
          return val == next.store;
        };

        if (hasNode(envWrite->getRVal(), anyReadsToStorePredicate)) // AnyReadsToStore
        {
          next.bottom = false;
          next.validEffect = false;
          next.store = NULL;
          next.effect = NULL;
          next.stmt = NULL;
          continue;
        }

        if (hasNode(envWrite, anyReadsOrWritesToCapturedVariablesPredicate)) // AnyReadsOrWritesToCapturedVariables
        {
          next.bottom = false;
          next.validEffect = false;
          next.store = NULL;
          next.effect = NULL;
          next.stmt = NULL;
          continue;
        }

        if (hasNode(envWrite->getRVal(), maybeEffect)) // RVAL maybeEffectful
        {
          next.bottom = false;
          next.validEffect = false;
          next.store = NULL;
          next.effect = NULL;
          next.stmt = NULL;
        }
      }
    }
    else if (hasNode(stmt, maybeEffect)) // maybeEffectful
    {
      next.bottom = false;
      next.validEffect = false;
      next.store = NULL;
      next.effect = NULL;
      next.stmt = NULL;
    }
    else if (hasNode(envWrite, anyReadsOrWritesToCapturedVariablesPredicate)) // AnyReadsOrWritesToCapturedVariables
    {
      next.bottom = false;
      next.validEffect = false;
      next.store = NULL;
      next.effect = NULL;
      next.stmt = NULL;
      continue;
    }
  }

  return next;
}