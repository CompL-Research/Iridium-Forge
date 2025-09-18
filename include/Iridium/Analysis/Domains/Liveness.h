#pragma once
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include <set>
#include <sstream>
#include <functional>
#include <algorithm>

struct Liveness
{
  using DFVT = std::set<std::shared_ptr<EnvBindingSEXP>>;

  DFVT dfv;

  void dump(std::ostringstream &oss) const
  {
    oss << "{ ";
    for (auto & val : dfv)
    {
      oss << val->getNAME() << " ";
    }
    oss << "}";
  }

  bool operator==(const Liveness &other) const
  {
    return dfv == other.dfv;
  }

  Liveness merge(const Liveness &other) const
  {
    Liveness res;
    std::set_union(dfv.begin(), dfv.end(),
                   other.dfv.begin(), other.dfv.end(),
                   std::inserter(res.dfv, res.dfv.begin()));
    return res;
  }

  static Liveness boundary(const std::set<std::shared_ptr<EnvBindingSEXP>> &alwaysLive)
  {
    Liveness res;
    res.dfv.insert(alwaysLive.begin(), alwaysLive.end());
    return res;
  }

  static Liveness bottom()
  {
    return Liveness{}; // empty set
  }

  Liveness clone() const
  {
    Liveness copy;
    copy.dfv = dfv; // copy underlying set
    return copy;
  }

  Liveness transfer(const std::shared_ptr<BBSEXP> &bb) const;

  Liveness iter(const std::shared_ptr<BBSEXP> &bb,
                std::function<void(size_t, Liveness)> callback) const;
};