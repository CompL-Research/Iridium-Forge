#pragma once

#include "external/Prakriti.hpp"
#include <memory>
#include <set>
#include <stdexcept>
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
      // PRECISION: [[Definite]] is the must-fact the read paths prune on. An
      // unknown key defines some property, never this one, so it is withheld.
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
      // No [[Writable]]: ECMA 6.1.7.1 makes accessor and data descriptors
      // disjoint, and adding it makes the FieldProxy read as both.
      desc->addEnumerable(Prakriti::PKRGlobalState::getTRUE());
      desc->addConfigurable(Prakriti::PKRGlobalState::getTRUE());
      // PRECISION: [[Definite]] is the must-fact the read paths prune on. An
      // unknown key defines some property, never this one, so it is withheld.
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

inline std::string computedFieldName(const std::set<Prakriti::NodeUID> &keys) {
  if (keys.size() == 1)
    if (const std::string *label =
            Prakriti::PKRGlobalState::wellKnownSymbolLabel(*keys.begin()))
      return *label;
  return PKR_UNKNOWN_FIELD;
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
    // See HandleFieldReadRVal: a primitive receiver would silently contribute
    // no values, which under-reports the result.
    if (acts.empty())
      throw std::runtime_error(
          "PTA: property read on a primitive receiver is not modelled");
    Prakriti::Karma(G, acts, {nullptr, {obj, obj}, {field}}, vals, branches);
  }
  Prakriti::KarmaJoin(G, branches);
  return {vals.begin(), vals.end()};
}

} // namespace IRI_STRUCTURAL
