#pragma once
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include "Iridium/Analysis/Helpers/UnionedDataMap.h"

// At a given statement, remember the last stored effect, any intermediate effectful statements NULL this value
struct EffectAtStmt
{
  bool bottom = true;
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
    if (bottom)
    {
      oss << "BOTTOM";
    }
    oss << " }";
  }

  bool operator==(const EffectAtStmt &other) const
  {
    if (bottom && !other.bottom) return false;
    if (!bottom && other.bottom) return false;
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
    if (bottom) return EffectAtStmt{other.bottom, other.validEffect, other.store, other.effect, other.stmt};
    if (other.bottom) return EffectAtStmt{bottom, validEffect, store, effect, stmt};

    if (*this == other) return EffectAtStmt{bottom, validEffect, store, effect, stmt};
    return EffectAtStmt{false, false, NULL, NULL, NULL};
  }

  EffectAtStmt clone() const
  {
    EffectAtStmt copy = *this;
    return copy;
  }

  EffectAtStmt transfer(const std::shared_ptr<BBSEXP> &bb) const;

  EffectAtStmt iter(const std::shared_ptr<BBSEXP> &bb, std::function<void(size_t, EffectAtStmt&)> callback) const;
};