#pragma once
#include <unordered_map>

template <typename Key, typename DFValue>
struct UnionedDataMap
{
  std::unordered_map<Key, DFValue> store;

  bool operator==(const UnionedDataMap &other) const
  {
    if (store.size() != other.store.size())
      return false;

    for (const auto &kv : store)
    {
      auto it = other.store.find(kv.first);
      if (it == other.store.end())
        return false;

      if (!(kv.second == it->second))
        return false;
    }
    return true;
  }

  UnionedDataMap merge(const UnionedDataMap &other) const
  {
    UnionedDataMap result;

    // Union of keys
    std::set<Key> allKeys;
    for (const auto &kv : store)
      allKeys.insert(kv.first);
    for (const auto &kv : other.store)
      allKeys.insert(kv.first);

    for (const auto &key : allKeys)
    {
      auto it1 = store.find(key);
      auto it2 = other.store.find(key);

      if (it1 != store.end() && it2 != other.store.end())
      {
        result.store[key] = it1->second.merge(it2->second);
      }
      else if (it1 != store.end())
      {
        result.store[key] = it1->second;
      }
      else
      {
        result.store[key] = it2->second;
      }
    }

    return result;
  }
};