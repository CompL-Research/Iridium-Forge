#pragma once
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include "Iridium/Analysis/Helpers/UnionedDataMap.h"
#include <set>

struct PropKeyLatticeValue
{
  enum ValKind
  {
    UNSAFE,
    SAFE,
    BOTTOM
  } kind = BOTTOM;

  void dump(std::ostream &oss) const
  {
    switch (kind)
    {
    case UNSAFE:
      oss << "UNSAFE";
      break;
    case SAFE:
      oss << "SAFE";
      break;
    case BOTTOM:
      oss << "BOTTOM";
      break;
    default:
      oss << "UnknownKind";
      break;
    }
  }

  using DFVT = UnionedDataMap<IRISEXP, PropKeyLatticeValue>;

  static bool isSafe(IRISEXP curr, const DFVT &flowVal)
  {
    if (auto o = std::dynamic_pointer_cast<NumberSEXP>(curr))
    {
      return true;
    }
    else if (auto o = std::dynamic_pointer_cast<StringSEXP>(curr))
    {
      return true;
    }
    else if (auto o = std::dynamic_pointer_cast<UnopSEXP>(curr))
    {
      return isSafe(o->getVal(), flowVal);
    }
    else if (auto o = std::dynamic_pointer_cast<BinopSEXP>(curr))
    {
      return (isSafe(o->getLBinop(), flowVal) && isSafe(o->getRBinop(), flowVal));
    }
    else if (auto o = std::dynamic_pointer_cast<EnvReadSEXP>(curr))
    {
      auto it = flowVal.store.find(o->getObj());
      if (it == flowVal.store.end())
      {
        return false;
      }

      return it->second.kind == SAFE || it->second.kind == BOTTOM;
      // BOTTOM = unknown but treated as safe for key cast purposes
    }
    return false;
  }

  static PropKeyLatticeValue generate(IRISEXP val, DFVT & flowVal)
  {
    if (isSafe(val, flowVal))
      return PropKeyLatticeValue{SAFE};
    return PropKeyLatticeValue{UNSAFE};
  }

  bool operator==(const PropKeyLatticeValue &other) const
  {
    return kind == other.kind;
  }

  PropKeyLatticeValue merge(const PropKeyLatticeValue &other) const
  {
    // One is bottom, return the other
    if (kind == BOTTOM)
      return PropKeyLatticeValue{other.kind};
    if (other.kind == BOTTOM)
      return PropKeyLatticeValue{kind};

    if (kind == SAFE && other.kind == SAFE)
      return PropKeyLatticeValue{SAFE};
    
    return PropKeyLatticeValue{UNSAFE};
  }
};

struct SetSafePropKeyAccesses
{
  using DFVT = UnionedDataMap<IRISEXP, PropKeyLatticeValue>;

  DFVT dfv;
  static std::set<std::shared_ptr<EnvBindingSEXP>> blacklist;

  void dump(std::ostream &oss) const
  {
    oss << "{" << std::endl;
    for (const auto &kv : dfv.store)
    {
      oss << kv.first->getFlagString("NAME") << " → ";
      kv.second.dump(oss);
      oss << std::endl;
    }
    oss << "}";
  }

  bool operator==(const SetSafePropKeyAccesses &other) const
  {
    return dfv == other.dfv;
  }

  SetSafePropKeyAccesses merge(const SetSafePropKeyAccesses &other) const
  {
    auto res = clone();
    res.dfv = res.dfv.merge(other.dfv);
    return res;
  }

  static SetSafePropKeyAccesses boundary(std::set<IRISEXP> vals) {
    auto res = SetSafePropKeyAccesses();
    for (auto v : vals) 
    {
      res.dfv.store[v] = PropKeyLatticeValue{PropKeyLatticeValue::BOTTOM};
    }
    return res;
  }
  static SetSafePropKeyAccesses bottom(std::set<IRISEXP> vals) {
    auto res = SetSafePropKeyAccesses();
    for (auto v : vals) 
    {
      res.dfv.store[v] = PropKeyLatticeValue{PropKeyLatticeValue::BOTTOM};
    }
    return res;
  }

  SetSafePropKeyAccesses clone() const
  {
    SetSafePropKeyAccesses copy;
    copy.dfv.store = dfv.store; // copy underlying map
    return copy;
  }

  SetSafePropKeyAccesses transfer(const std::shared_ptr<BBSEXP> &bb) const;

  SetSafePropKeyAccesses iter(const std::shared_ptr<BBSEXP> & bb, std::function<void(size_t, SetSafePropKeyAccesses)> callback) const;
};