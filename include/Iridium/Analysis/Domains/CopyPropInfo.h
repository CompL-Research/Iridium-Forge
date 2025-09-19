#pragma once
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include "Iridium/Analysis/Helpers/UnionedDataMap.h"

struct CopyPropInfo
{
  using DFVT = std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::shared_ptr<EnvBindingSEXP>>;

  DFVT dfv;
  
  static std::set<std::shared_ptr<EnvBindingSEXP>> blacklist;

  void dump(std::ostream &oss) const
  {
    oss << "{";
    for (const auto &kv : dfv)
    {
      oss << "  " << kv.first->getNAME() << " → " << kv.second->getNAME();
      ;
    }
    oss << " }";
  }

  bool operator==(const CopyPropInfo &other) const
  {
    if (dfv.size() != other.dfv.size())
      return false;

    for (const auto &kv : dfv)
    {
      auto it = other.dfv.find(kv.first);
      if (it == other.dfv.end())
        return false;
      if (kv.second != it->second)
        return false;
    }
    return true;
  }

  CopyPropInfo merge(CopyPropInfo &other) const
  {
    CopyPropInfo out;
    for (const auto &kv : dfv)
    {
      if ((other.dfv.count(kv.first) > 0) && (other.dfv[kv.first] == kv.second))
      {
        out.dfv[kv.first] = kv.second;
      }
    }
    return out;
  }

  CopyPropInfo clone() const
  {
    CopyPropInfo copy;
    copy.dfv = dfv; // copy underlying map
    return copy;
  }

  CopyPropInfo transfer(const std::shared_ptr<BBSEXP> &bb) const;

  CopyPropInfo iter(const std::shared_ptr<BBSEXP> &bb, std::function<void(size_t, CopyPropInfo&)> callback) const;
};