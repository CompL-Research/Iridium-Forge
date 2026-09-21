#pragma once

#include "external/Prakriti.hpp"
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace IRI_STRUCTURAL {

// A[[Set]] F = VALS
inline void setProperty(Prakriti::ECMAGraph *G,
                        const std::set<Prakriti::NodeUID> &objs,
                        const std::string &field,
                        const std::set<Prakriti::NodeUID> &vals) {
  std::vector<Prakriti::NodeUID> discarded;
  std::vector<Prakriti::ECMAGraph> branches;
  for (const auto obj : objs) {
    auto acts =
        G->getPointees(obj, Prakriti::PKRGlobalState::EdgeIntern(PKR_Set));
    if (acts.empty())
      continue; // a primitive receiver has no [[Set]]
    for (const auto v : vals)
      Prakriti::Karma(G, acts, {nullptr, {obj, v}, {field}}, discarded,
                      branches);
  }
  Prakriti::KarmaJoin(G, branches);
}

// A[[DefineOwnProperty]] F = VALS
inline void definePropertyValue(Prakriti::ECMAGraph *G,
                                const std::set<Prakriti::NodeUID> &objs,
                                const std::string &field,
                                const std::set<Prakriti::NodeUID> &vals) {
  std::vector<Prakriti::NodeUID> discarded;
  std::vector<Prakriti::ECMAGraph> branches;
  for (const auto obj : objs) {
    auto acts = G->getPointees(
        obj, Prakriti::PKRGlobalState::EdgeIntern(PKR_DefineOwnProperty));
    if (acts.empty())
      continue;
    for (const auto v : vals) {
      auto desc = std::make_shared<Prakriti::TempFieldDescriptor>();
      desc->addValue(v);
      desc->addWritable(Prakriti::PKRGlobalState::getTRUE());
      desc->addEnumerable(Prakriti::PKRGlobalState::getTRUE());
      desc->addConfigurable(Prakriti::PKRGlobalState::getTRUE());
      // An unknown key defines some property, never this one.
      if (field != PKR_UNKNOWN_FIELD)
        desc->addDefinite(Prakriti::PKRGlobalState::getTRUE());
      Prakriti::Karma(G, acts, {nullptr, {obj}, {field}, {desc}}, discarded,
                      branches);
    }
  }
  Prakriti::KarmaJoin(G, branches);
}

// A[[DefineOwnProperty | Set,Get]] F = VALS
inline void definePropertySetterGetter(Prakriti::ECMAGraph *G,
                                       const std::set<Prakriti::NodeUID> &objs,
                                       const std::string &field,
                                       const std::set<Prakriti::NodeUID> &vals,
                                       bool setter) {
  std::vector<Prakriti::NodeUID> discarded;
  std::vector<Prakriti::ECMAGraph> branches;
  for (const auto obj : objs) {
    auto acts = G->getPointees(
        obj, Prakriti::PKRGlobalState::EdgeIntern(PKR_DefineOwnProperty));
    if (acts.empty())
      continue;
    for (const auto v : vals) {
      auto desc = std::make_shared<Prakriti::TempFieldDescriptor>();
      if (setter)
        desc->addSet(v);
      else
        desc->addGet(v);
      desc->addWritable(Prakriti::PKRGlobalState::getTRUE());
      desc->addEnumerable(Prakriti::PKRGlobalState::getTRUE());
      desc->addConfigurable(Prakriti::PKRGlobalState::getTRUE());
      // An unknown key defines some property, never this one.
      if (field != PKR_UNKNOWN_FIELD)
        desc->addDefinite(Prakriti::PKRGlobalState::getTRUE());
      Prakriti::Karma(G, acts, {nullptr, {obj}, {field}, {desc}}, discarded,
                      branches);
    }
  }
  Prakriti::KarmaJoin(G, branches);
}

inline void definePropertySetter(Prakriti::ECMAGraph *G,
                                 const std::set<Prakriti::NodeUID> &objs,
                                 const std::string &field,
                                 const std::set<Prakriti::NodeUID> &vals) {
  definePropertySetterGetter(G, objs, field, vals, true);
}
inline void definePropertyGetter(Prakriti::ECMAGraph *G,
                                 const std::set<Prakriti::NodeUID> &objs,
                                 const std::string &field,
                                 const std::set<Prakriti::NodeUID> &vals) {
  definePropertySetterGetter(G, objs, field, vals, false);
}

// A[[Get]] F
inline std::set<Prakriti::NodeUID>
getProperty(Prakriti::ECMAGraph *G, const std::set<Prakriti::NodeUID> &objs,
            const std::string &field) {
  std::vector<Prakriti::NodeUID> vals;
  std::vector<Prakriti::ECMAGraph> branches;
  for (const auto obj : objs) {
    auto acts =
        G->getPointees(obj, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get));
    if (acts.empty())
      continue;
    Prakriti::Karma(G, acts, {nullptr, {obj, obj}, {field}}, vals, branches);
  }
  Prakriti::KarmaJoin(G, branches);
  return {vals.begin(), vals.end()};
}

} // namespace IRI_STRUCTURAL
