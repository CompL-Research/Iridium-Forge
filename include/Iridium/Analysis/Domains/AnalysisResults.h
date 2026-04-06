#pragma once

#include <unordered_map>
#include <set>

#include "Iridium/Structure/CFGManager.h"
#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"
#include "Iridium/Analysis/Domains/CopyPropInfo.h"
#include "Iridium/Analysis/Domains/TDZA.h"
#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/Analysis/Domains/EffectAtStmt.h"
#include "Iridium/Analysis/Domains/SetSafePropKeyAccesses.h"

using ConstantsResult = std::unordered_map<Vertex, ConstantsAtStmt>;
using CopyPropResult = std::unordered_map<Vertex, CopyPropInfo>;
using TDZAResult = std::unordered_map<Vertex, TDZA>;
using LivenessResult = std::unordered_map<Vertex, Liveness>;
using EffectResult = std::unordered_map<Vertex, EffectAtStmt>;
using SafePropKeyResult = std::unordered_map<Vertex, SetSafePropKeyAccesses>;

using CapturedBindingsResult = std::set<std::shared_ptr<EnvBindingSEXP>>;
using UncapturedBindingsResult = std::set<IRISEXP>;
using AllBindingsResult = std::set<IRISEXP>;
