#pragma once
#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/Analysis/Domains/EffectAtStmt.h"

class PropEffects
{

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
        std::dynamic_pointer_cast<StackPopSEXP>(rVAL));
  }

  static bool patchExprNew(IRISEXP expr, std::set<IRISEXP> &killset, EffectAtStmt &val)
  {
    if (auto o = std::dynamic_pointer_cast<BooleanSEXP>(expr)) return true;
    if (auto o = std::dynamic_pointer_cast<LambdaSEXP>(expr)) return true;
    if (auto o = std::dynamic_pointer_cast<NullSEXP>(expr)) return true;
    if (auto o = std::dynamic_pointer_cast<NumberSEXP>(expr)) return true;
    if (auto o = std::dynamic_pointer_cast<RegExpSEXP>(expr)) return true;
    if (auto o = std::dynamic_pointer_cast<StringSEXP>(expr)) return true;
    if (auto o = std::dynamic_pointer_cast<BitIntSEXP>(expr)) return true;
    if (auto o = std::dynamic_pointer_cast<JSNUBDSEXP>(expr)) return true;
    if (auto o = std::dynamic_pointer_cast<JSObjectSEXP>(expr)) return true;
    if (auto o = std::dynamic_pointer_cast<JSPrivateSEXP>(expr)) return true;

    if (auto o = std::dynamic_pointer_cast<StackPopSEXP>(expr)) return false;

    if (auto o = std::dynamic_pointer_cast<UnopSEXP>(expr)) {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getVal()))
      {
        if (node->getObj() == val.store)
        {
          o->setVal(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      } else return patchExprNew(o->getVal(), killset, val);
      return true;
    }

    if (auto o = std::dynamic_pointer_cast<BinopSEXP>(expr)) {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getLBinop()))
      {
        if (node->getObj() == val.store)
        {
          o->setLBinop(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getLBinop(), killset, val)) return false;
      }

      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getRBinop()))
      {
        if (node->getObj() == val.store)
        {
          o->setRBinop(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getRBinop(), killset, val)) return false;
      }
      return true;
    }

    if (auto l = std::dynamic_pointer_cast<ListSEXP>(expr)) {

      for (size_t i = 0; i < l->args.size(); i++)
      {
        auto o = l->args.at(i);
        if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o))
        {
          if (node->getObj() == val.store)
          {
            l->args.at(i) = val.effect;
            killset.insert(val.stmt);
            return false;
          }
        }
        else if (!patchExprNew(o, killset, val)) return false;
      }

      return true;
    }

    if (auto l = std::dynamic_pointer_cast<JSArraySEXP>(expr)) {

      for (size_t i = 0; i < l->args.size(); i++)
      {
        auto o = l->args.at(i);
        if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o))
        {
          if (node->getObj() == val.store)
          {
            l->args.at(i) = val.effect;
            killset.insert(val.stmt);
            return false;
          }
        }
        else if (!patchExprNew(o, killset, val)) return false;
      }

      return true;
    }

    if (auto o = std::dynamic_pointer_cast<JSBinopSEXP>(expr)) {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getLBinop()))
      {
        if (node->getObj() == val.store)
        {
          o->setLBinop(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getLBinop(), killset, val)) return false;
      }

      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getRBinop()))
      {
        if (node->getObj() == val.store)
        {
          o->setRBinop(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getRBinop(), killset, val)) return false;
      }
      return true;
    }

    if (auto o = std::dynamic_pointer_cast<JSClassSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSSpreadSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSTemplateSEXP>(expr)) return false;

    if (auto o = std::dynamic_pointer_cast<JSUnopSEXP>(expr)) {
      if (o->getOP() == "delete") return false;
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getVal()))
      {
        if (node->getObj() == val.store)
        {
          o->setVal(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      } else return patchExprNew(o->getVal(), killset, val);
      return true;
    }

    if (auto o = std::dynamic_pointer_cast<UNOPDelMemberExprSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<UNOPDelVarSEXP>(expr)) return false;

    if (auto o = std::dynamic_pointer_cast<AwaitSEXP>(expr)) return false;

    if (auto o = std::dynamic_pointer_cast<JSComputedFieldReadSEXP>(expr)) 
    {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getObj()))
      {
        if (node->getObj() == val.store)
        {
          o->setObj(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getObj(), killset, val)) return false;
      }

      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getField()))
      {
        if (node->getObj() == val.store)
        {
          o->setField(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getField(), killset, val)) return false;
      }

      return false;
    }

    if (auto o = std::dynamic_pointer_cast<JSComputedFieldWriteSEXP>(expr)) 
    {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getObj()))
      {
        if (node->getObj() == val.store)
        {
          o->setObj(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getObj(), killset, val)) return false;
      }

      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getField()))
      {
        if (node->getObj() == val.store)
        {
          o->setField(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getField(), killset, val)) return false;
      }

      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getValue()))
      {
        if (node->getObj() == val.store)
        {
          o->setValue(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getValue(), killset, val)) return false;
      }

      return false;
    }

    if (auto o = std::dynamic_pointer_cast<JSPrivateFieldReadSEXP>(expr)) 
    {
      // WIP

      return false;
    }

    if (auto o = std::dynamic_pointer_cast<JSPrivateFieldWriteSEXP>(expr)) 
    {
      // WIP

      return false;
    }

    if (auto o = std::dynamic_pointer_cast<JSSuperFieldReadSEXP>(expr)) 
    {
      // WIP

      return false;
    }

    if (auto o = std::dynamic_pointer_cast<JSSuperFieldWriteSEXP>(expr)) 
    {
      // WIP

      return false;
    }

    if (auto o = std::dynamic_pointer_cast<PVTEnvReadSEXP>(expr)) return false;

    if (auto o = std::dynamic_pointer_cast<EnvReadSEXP>(expr)) {
      throw std::runtime_error("Unreachable case... prop effects");
      return false;
    }

    if (auto o = std::dynamic_pointer_cast<FieldReadSEXP>(expr)) 
    {
      // WIP
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getObj()))
      {
        if (node->getObj() == val.store)
        {
          o->setObj(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getObj(), killset, val)) return false;
      }

      return false;
    }

    if (auto o = std::dynamic_pointer_cast<FieldWriteSEXP>(expr)) 
    {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getObj()))
      {
        if (node->getObj() == val.store)
        {
          o->setObj(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getObj(), killset, val)) return false;
      }

      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getValue()))
      {
        if (node->getObj() == val.store)
        {
          o->setValue(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getValue(), killset, val)) return false;
      }

      return false;
    }

    if (auto o = std::dynamic_pointer_cast<EnvWriteSEXP>(expr)) 
    {
      return patchExprNew(o->getRVal(), killset, val);
    }

    if (auto o = std::dynamic_pointer_cast<CallSiteSEXP>(expr)) 
    {
      for (size_t i = 0; i < expr->args.size(); i++)
      {
        auto o = expr->args.at(i);
        if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o))
        {
          if (node->getObj() == val.store)
          {
            expr->args.at(i) = val.effect;
            killset.insert(val.stmt);
            return false;
          }
        }
        else if (!patchExprNew(o, killset, val)) return false;
      }

      return false;
    }

    if (auto o = std::dynamic_pointer_cast<JSToObjectSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSCatchContextSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSADDBRANDSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSCheckConstructorSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSForInNextSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSForInStartSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSForOfIteratorCloseSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSForOfNextSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSForOfStartSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSAppendSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSCopyDataPropertiesSEXP>(expr)) return false;
    if (auto o = std::dynamic_pointer_cast<JSDefineObjMethodSEXP>(expr)) return false;

    if (auto o = std::dynamic_pointer_cast<JSDefineObjPropSEXP>(expr)) {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getTargetObj()))
      {
        if (node->getObj() == val.store)
        {
          o->setTargetObj(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getTargetObj(), killset, val)) return false;
      }

      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getKey()))
      {
        if (node->getObj() == val.store)
        {
          o->setKey(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getKey(), killset, val)) return false;
      }

      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(o->getValue()))
      {
        if (node->getObj() == val.store)
        {
          o->setValue(val.effect);
          killset.insert(val.stmt);
          return false;
        }
      }
      else
      {
        if (!patchExprNew(o->getValue(), killset, val)) return false;
      }
      return true; // Under thought, can this really have any side effect???
      // assuming no for now...
    }

    if (auto o = std::dynamic_pointer_cast<IDOPSEXP>(expr)) 
    {
      patchExprNew(o->getObj(), killset, val);
      return false;
    }

    if (auto o = std::dynamic_pointer_cast<JSIDOPSEXP>(expr)) 
    {
      patchExprNew(o->getObj(), killset, val);
      return false;
    }

    throw std::runtime_error("Unhandled case... prop effects TAG: " + expr->tag);
    return false;
  }

  static void patchExprOuter(IRISEXP expr, std::set<IRISEXP> &killset, EffectAtStmt &val)
  {
    // StackRejectSEXP
    // | StackRetainSEXP
    // | GotoSEXP
    // | IfElseJumpSEXP
    // | IfJumpSEXP
    // | InvokeFinalizerSEXP
    // | PopCatchContextSEXP
    // | PushCatchContextSEXP
    // | RetSEXP
    // | ReturnSEXP
    // | ThrowSEXP
    // | JSInitialYieldSEXP
    // | ReturnAsyncSEXP
    // | YieldSEXP
    // | JSDefineObjPropSEXP
    // | JSFuncDeclSEXP
    // | JSImplicitBindingDeclarationSEXP
    // | JSSloppyDeclSEXP

    auto numOccurences = countNode(expr, [&](IRISEXP val1) {
      if (auto vv = std::dynamic_pointer_cast<EnvReadSEXP>(val1))
      {
        return vv->getObj() == val.store;
      }
      return false;
    });

    if (numOccurences != 1) return;

    if (auto stackRej = std::dynamic_pointer_cast<StackRejectSEXP>(expr))
    {
      for (auto a : stackRej->args)
      {
        patchExprNew(a, killset, val);
      }
    }
    else if (auto stackRetain = std::dynamic_pointer_cast<StackRetainSEXP>(expr))
    {
      for (auto a : stackRetain->args)
      {
        patchExprNew(a, killset, val);
      }
    }
    else if (auto ifElseJump = std::dynamic_pointer_cast<IfElseJumpSEXP>(expr))
    {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(ifElseJump->getTest()))
      {
        if (node->getObj() == val.store)
        {
          ifElseJump->setTest(val.effect);
          killset.insert(val.stmt);
        }
      }
      else
      {
        patchExprNew(ifElseJump->getTest(), killset, val);
      }
    }
    else if (auto ret = std::dynamic_pointer_cast<ReturnSEXP>(expr))
    {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(ret->getObj()))
      {
        if (node->getObj() == val.store)
        {
          ret->setObj(val.effect);
          killset.insert(val.stmt);
        }
      }
      else
      {
        patchExprNew(ret->getObj(), killset, val);
      }
    }
    else if (auto thr = std::dynamic_pointer_cast<ThrowSEXP>(expr))
    {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(thr->getThrowVal()))
      {
        if (node->getObj() == val.store)
        {
          thr->setThrowVal(val.effect);
          killset.insert(val.stmt);
        }
      }
      else
      {
        patchExprNew(thr->getThrowVal(), killset, val);
      }
    }
    else if (auto yield = std::dynamic_pointer_cast<YieldSEXP>(expr))
    {
      if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(yield->getObj()))
      {
        if (node->getObj() == val.store)
        {
          yield->setObj(val.effect);
          killset.insert(val.stmt);
        }
      }
      else
      {
        patchExprNew(yield->getObj(), killset, val);
      }
    }
    else if (auto envWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(expr))
    {
      if (auto innerWriteSEXP = std::dynamic_pointer_cast<EnvWriteSEXP>(envWriteSEXP->getRVal()))
      {
        if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(innerWriteSEXP->getRVal()))
        {
          if (node->getObj() == val.store)
          {
            innerWriteSEXP->setRVal(val.effect);
            killset.insert(val.stmt);
          }
        }
        else
        {
          patchExprNew(innerWriteSEXP->getRVal(), killset, val);
        }
      }
      else
      {
        if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(envWriteSEXP->getRVal()))
        {
          if (node->getObj() == val.store)
          {
            envWriteSEXP->setRVal(val.effect);
            killset.insert(val.stmt);
          }
        }
        else
        {
          patchExprNew(envWriteSEXP->getRVal(), killset, val);
        }
      }
    }
  }

public:
  static void Transform(std::shared_ptr<BBSEXP> &bb, std::set<IRISEXP> &killset, Liveness livenessAfterStmt, EffectAtStmt effectAtStmt)
  {
    // Effects stack right to left, so move throught the node LTR depth wise
    // If any effect already exists, this propagation is unsafe
    // Otherwise move it...
    // Killset is used to delete the STMT after propagation

    std::unordered_map<size_t, Liveness> livenessInfo;
    std::unordered_map<size_t, EffectAtStmt> effectInfo;

    livenessAfterStmt.iter(bb, [&](size_t idx, Liveness val)
                           { livenessInfo[idx] = val; });

    effectAtStmt.iter(bb, [&](size_t idx, EffectAtStmt val)
                      { effectInfo[idx] = val; });

    for (size_t currIdx = 0; currIdx < bb->args.size(); currIdx++)
    {
      auto &stmt = bb->args.at(currIdx);
      auto &currEffect = effectInfo[currIdx];
      auto &livenessInfoAfterStmt = livenessInfo[currIdx];

      if (effectInfo[currIdx].validEffect && livenessInfoAfterStmt.dfv.count(currEffect.store) == 0)
      {
        patchExprOuter(stmt, killset, currEffect);
      }
      // std::cout << std::endl;
      // stmt->prettyPrint(std::cout, 0);
      // std::cout << std::endl;
      // effectInfo[currIdx].dump(std::cout);
      // std::cout << " -- ";
      // livenessInfo[currIdx].dump(std::cout);
      // std::cout << std::endl;
    }
  }
};