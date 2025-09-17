#pragma once
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include "Iridium/Analysis/Helpers/UnionedDataMap.h"

struct TDZLattice
{
  enum ValKind
  {
    TDZ,
    SAFE
  } kind;

  void dump(std::ostringstream &oss) const
  {
    switch (kind)
    {
    case TDZ:
      oss << "TDZ";
      break;
    default:
      oss << "SAFE";
      break;
    }
  }

  static TDZLattice generate(IRISEXP val)
  {
    if (std::dynamic_pointer_cast<JSNUBDSEXP>(val))
      return TDZLattice{TDZ};
    return TDZLattice{SAFE};
  }

  bool operator==(const TDZLattice &other) const
  {
    return kind != other.kind;
  }

  TDZLattice merge(const TDZLattice &other) const
  {
    if (kind == TDZ || other.kind == TDZ) return TDZLattice{TDZ};
    return TDZLattice{SAFE};
  }
};

struct TDZA
{
  using DFVT = UnionedDataMap<IRISEXP, TDZLattice>;

  DFVT dfv;

  void dump(std::ostringstream &oss) const
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

  bool operator==(const TDZA &other) const
  {
    return dfv == other.dfv;
  }

  TDZA merge(const TDZA &other) const
  {
    auto res = clone();
    res.dfv = res.dfv.merge(other.dfv);
    return res;
  }

  static TDZA bottom() { return TDZA(); }

  TDZA clone() const
  {
    TDZA copy;
    copy.dfv.store = dfv.store; // copy underlying map
    return copy;
  }

  TDZA transfer(const std::shared_ptr<BBSEXP> &bb) const;
};