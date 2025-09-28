#pragma once
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include "Iridium/Analysis/Helpers/UnionedDataMap.h"

// At a given statement, remember the last stored effect, any intermediate effectful statements NULL this value
struct EffectAtStmt
{
  bool validEffect = false;
  std::shared_ptr<EnvBindingSEXP> store = NULL;
  IRISEXP effect = NULL;
  IRISEXP stmt = NULL;
  
  static std::set<std::shared_ptr<EnvBindingSEXP>> blacklist;

  void dump(std::ostream &oss) const
  {
    oss << "{";
    if (validEffect)
    {
      oss << "  " << store->getNAME() << " → ";
      effect->prettyPrint(oss, 0);
    }
    oss << " }";
  }

  bool operator==(const EffectAtStmt &other) const
  {
    if (validEffect == other.validEffect)
    {
      if (validEffect)
      {
        return store == other.store && effect == other.effect;
      }
      return true;
    }
    return false;
  }

  EffectAtStmt merge(EffectAtStmt &other) const
  {
    if (*this == other) return EffectAtStmt{validEffect, store, effect, stmt};
    return EffectAtStmt{false, NULL, NULL, NULL};
  }

  EffectAtStmt clone() const
  {
    EffectAtStmt copy = *this;
    return copy;
  }

  EffectAtStmt transfer(const std::shared_ptr<BBSEXP> &bb) const;

  EffectAtStmt iter(const std::shared_ptr<BBSEXP> &bb, std::function<void(size_t, EffectAtStmt&)> callback) const;
};