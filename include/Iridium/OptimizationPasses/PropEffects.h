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

  static bool patchExpr(IRISEXP expr, std::set<IRISEXP> &killset, EffectAtStmt &val)
  {
    // If any effectful node comes before, quit early, its unsafe
    // Population is only safe RTL

    // WIP
    // if (auto obj = std::dynamic_pointer_cast<JSComputedFieldReadSEXP>(expr))
    // {
    //   if (!patchExpr(obj->getObj(), killset, val))
    //     return false;
    //   if (!patchExpr(obj->getField(), killset, val))
    //     return false;
    // }

    // if (auto obj = std::dynamic_pointer_cast<JSComputedFieldWriteSEXP>(expr))
    // {
    //   if (!patchExpr(obj->getObj(), killset, val))
    //     return false;
    //   if (!patchExpr(obj->getField(), killset, val))
    //     return false;
    //   if (!patchExpr(obj->getValue(), killset, val))
    //     return false;
    // }

    // if (auto obj = std::dynamic_pointer_cast<JSPrivateFieldReadSEXP>(expr))
    // {
    //   if (!patchExpr(obj->getObj(), killset, val))
    //     return false;
    //   if (!patchExpr(obj->getField(), killset, val))
    //     return false;
    // }

    // if (auto obj = std::dynamic_pointer_cast<JSPrivateFieldWriteSEXP>(expr))
    // {
    //   if (!patchExpr(obj->getObj(), killset, val))
    //     return false;
    //   if (!patchExpr(obj->getField(), killset, val))
    //     return false;
    //   if (!patchExpr(obj->getValue(), killset, val))
    //     return false;
    // }

    // if (auto obj = std::dynamic_pointer_cast<JSSuperFieldReadSEXP>(expr))
    // {
    //   return false;
    // }
    // if (auto obj = std::dynamic_pointer_cast<JSSuperFieldWriteSEXP>(expr))
    // {
    //   return false;
    // }

    // if (auto obj = std::dynamic_pointer_cast<FieldReadSEXP>(expr))
    // {
    //   if (!patchExpr(obj->getObj(), killset, val))
    //     return false;
    //   if (!patchExpr(obj->getField(), killset, val))
    //     return false;
    // }

    // if (auto obj = std::dynamic_pointer_cast<FieldWriteSEXP>(expr))
    // {
    //   if (!patchExpr(obj->getObj(), killset, val))
    //     return false;
    //   if (!patchExpr(obj->getField(), killset, val))
    //     return false;
    //   if (!patchExpr(obj->getValue(), killset, val))
    //     return false;
    // }

    if (maybeEffect(expr))
      return false;

    for (size_t i = 0; i < expr->args.size(); i++)
    {
      auto &curr = expr->args[i];
      if (auto envReadNode = std::dynamic_pointer_cast<EnvReadSEXP>(curr))
      {
        if (envReadNode->getObj() == val.store)
        {
          expr->args[i] = val.effect;
          killset.insert(val.stmt);
        }
      }
      if (!patchExpr(expr->args[i], killset, val))
        return false;
    }
    return true;
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

    if (auto stackRej = std::dynamic_pointer_cast<StackRejectSEXP>(expr))
    {
      for (auto a : stackRej->args)
        patchExprOuter(a, killset, val);
    }
    else if (auto stackRetain = std::dynamic_pointer_cast<StackRetainSEXP>(expr))
    {
      for (auto a : stackRetain->args)
        patchExprOuter(a, killset, val);
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
      }
    }
    else if (auto cSite = std::dynamic_pointer_cast<CallSiteSEXP>(expr))
    {
      for (size_t i = 0; i < cSite->args.size(); i++)
      {
        auto &curr = cSite->args[i];
        if (auto envReadNode = std::dynamic_pointer_cast<EnvReadSEXP>(curr))
        {
          if (envReadNode->getObj() == val.store)
          {
            auto hasCurrBinding = [&](IRISEXP val){
              return val == envReadNode->getObj();
            };
            bool safeToProp = true;
            for (size_t j = i + 1; j < cSite->args.size(); j++)
            {
              if (hasNode(cSite->args[j], hasCurrBinding))
              {
                safeToProp = false;
                break;
              }
            }
            if (safeToProp)
            {
              expr->args[i] = val.effect; // Transformation finally happens here <-
              killset.insert(val.stmt);
            }
          }
        }
        else if (maybeEffect(curr)) return;
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