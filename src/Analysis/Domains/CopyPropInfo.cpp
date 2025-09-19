#include "Iridium/Analysis/Domains/CopyPropInfo.h"

std::set<std::shared_ptr<EnvBindingSEXP>> CopyPropInfo::blacklist;

CopyPropInfo CopyPropInfo::transfer(const std::shared_ptr<BBSEXP> &bb) const
{
  return iter(bb, [&](size_t idx, CopyPropInfo & val) {});
}

CopyPropInfo CopyPropInfo::iter(const std::shared_ptr<BBSEXP> &bb, std::function<void(size_t, CopyPropInfo &)> callback) const
{
  auto next = this->clone();

  size_t idx = 0;
  for (const auto &stmt : bb->args)
  {
    callback(idx++, next);
    auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt);
    if (!envWrite)
      continue;

    std::vector<std::shared_ptr<EnvBindingSEXP>> lValsToUpdate;
    std::shared_ptr<EnvBindingSEXP> RVAL = NULL;

    if (auto r = std::dynamic_pointer_cast<EnvReadSEXP>(envWrite->getRVal()))
    {
      if (auto t = std::dynamic_pointer_cast<EnvBindingSEXP>(r->getObj()))
      {
        RVAL = t;
      }
    }

    if (auto lVal = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getLValTarget()))
    {
      lValsToUpdate.push_back(lVal);
    }

    if (auto innerWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(envWrite->getRVal()))
    {
      if (auto lVal = std::dynamic_pointer_cast<EnvBindingSEXP>(innerWrite->getLValTarget()))
      {
        lValsToUpdate.push_back(lVal);
      }

      if (auto r = std::dynamic_pointer_cast<EnvReadSEXP>(innerWrite->getRVal()))
      {
        if (auto t = std::dynamic_pointer_cast<EnvBindingSEXP>(r->getObj()))
        {
          RVAL = t;
        }
      }
    }

    // Step 1: Build the KILL set (union of blacklist and lValsToUpdate)
    std::unordered_set<std::shared_ptr<EnvBindingSEXP>> kill;

    kill.insert(blacklist.begin(), blacklist.end());
    kill.insert(lValsToUpdate.begin(), lValsToUpdate.end());

    // Step 2: Erase from dfv any entry whose key or value is in KILL
    for (auto it = next.dfv.begin(); it != next.dfv.end();)
    {
      if (kill.count(it->first) || kill.count(it->second))
        it = next.dfv.erase(it); // erase returns the next iterator
      else
        ++it;
    }

    if (RVAL && lValsToUpdate.size() > 0)
    {
      for (auto &lVal : lValsToUpdate)
      {
        next.dfv[lVal] = RVAL;
      }
    }
  }

  return next;
}