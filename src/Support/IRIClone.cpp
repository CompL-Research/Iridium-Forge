#include "Support/IRIClone.hpp"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/IRIS.hpp"
#include <cstdio>
#include <cstdlib>
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {
using namespace IRI_STORAGE;
using namespace IRI_GEN;

// iris interns one IRID per binding; duplication would break resolution
// passes that treat them as singletons.
bool IRICloner::isSingletonTag(IRI_TAG tag) {
  switch (tag) {
  case IRI_TAG::EnvBinding:
  case IRI_TAG::GlobalBinding:
  case IRI_TAG::ScriptBinding:
  case IRI_TAG::RemoteEnvBinding:
  case IRI_TAG::Lambda:
    return true;
  default:
    return false;
  }
}

void IRICloner::remapTargets(IRIContext &ctx, IRID stmtID,
                             const std::unordered_map<BBIDX, BBIDX> &targetMap) {
  auto re = [&](size_t pos) {
    auto flags = ctx.storage.nodes.get_flags_m(stmtID);
    if (std::holds_alternative<UInt32>(flags[pos])) {
      auto it = targetMap.find(std::get<UInt32>(flags[pos]).v);
      if (it != targetMap.end())
        flags[pos] = UInt32{it->second};
    }
  };
  switch (IRI_NODE(ctx, stmtID).tag) {
  case IRI_TAG::Goto:
    re(GotoSEXP::FLAG_IDX_IDX);
    break;
  case IRI_TAG::IfJump:
    re(IfJumpSEXP::FLAG_IDX_IDX);
    break;
  case IRI_TAG::IfElseJump:
    re(IfElseJumpSEXP::FLAG_IDX_TRUE);
    re(IfElseJumpSEXP::FLAG_IDX_FALSE);
    break;
  case IRI_TAG::PushCatchContext:
    re(PushCatchContextSEXP::FLAG_IDX_IDX);
    break;
  case IRI_TAG::InvokeFinalizer:
    re(InvokeFinalizerSEXP::FLAG_IDX_IDX);
    break;
  default:
    break;
  }
}

IRID IRICloner::cloneNode(IRID id) {
  auto tag = IRI_NODE(ctx, id).tag;
  if (isSingletonTag(tag))
    return id;
  auto it = nodeMap.find(id);
  if (it != nodeMap.end())
    return it->second;

  std::vector<IRID> cArgs;
  for (auto a : ctx.storage.nodes.get_args(id))
    cArgs.push_back(cloneNode(a));

  auto flags = ctx.storage.nodes.get_flags(id);
  std::vector<FlagValue> cFlags(flags.begin(), flags.end());

  IRID cid = ctx.storage.nodes.add_node(tag, cArgs, cFlags);
  nodeMap[id] = cid;
  return cid;
}

IRID IRICloner::cloneStmt(IRID id) {
  IRID cid = cloneNode(id);
  if (cid != id)
    remapTargets(ctx, cid, targetMap);
  return cid;
}

// Same flags (presence semantics included) and scope as src, fresh IDX,
// statements taken verbatim.
static IRID newBBLike(IRIContext &ctx, IRID srcBBID, BBIDX newIDX) {
  BBSEXP src(srcBBID, ctx);
  auto flags = ctx.storage.nodes.get_flags(srcBBID);
  std::vector<FlagValue> cFlags(flags.begin(), flags.end());
  cFlags[BBSEXP::FLAG_IDX_IDX] = UInt32{newIDX};
  return ctx.storage.nodes.add_node(IRI_TAG::BB, {}, cFlags);
}

IRID cloneBB(IRIContext &ctx, IRID bbID, BBIDX newIDX, IRICloner &cloner) {
  IRID nb = newBBLike(ctx, bbID, newIDX);
  std::vector<IRID> cStmts;
  for (auto s : ctx.storage.nodes.get_args(bbID))
    cStmts.push_back(cloner.cloneStmt(s));
  ctx.storage.nodes.set_args(nb, cStmts);

  // Dispatch wiring travels with the region: newBBLike already copied the
  // source's EXCIDX, so only a target that landed inside the clone is rewired.
  BBSEXP copy(nb, ctx);
  if (copy.hasEXCIDX()) {
    auto it = cloner.targetMap.find(copy.getEXCIDX());
    if (it != cloner.targetMap.end())
      copy.setEXCIDX(it->second);
  }
  return nb;
}

IRID splitBBSuffix(IRIContext &ctx, IRID bbID, size_t fromIdx) {
  auto args = ctx.storage.nodes.get_args(bbID);
  if (fromIdx == 0 || fromIdx >= args.size())
    throw std::runtime_error("splitBBSuffix: index out of range");
  BBIDX newIDX = ++ctx.lastBBIDX;
  // newBBLike copies EXCIDX: the suffix keeps the source's dispatch target.
  IRID nb = newBBLike(ctx, bbID, newIDX);
  ctx.storage.nodes.set_args(
      nb, std::vector<IRID>(args.begin() + fromIdx, args.end()));
  ctx.storage.nodes.update_num_args(bbID, fromIdx);
  return nb;
}

// Plain jumps plus catch entries: a region being cloned includes the
// dispatch structure of nested try/catch (entered at runtime through the
// pushed catch context), so its clone must carry that structure along.
std::vector<BBIDX> bbTargets(IRIContext &ctx, IRID bbID) {
  std::vector<BBIDX> out;
  auto add = [&](IRID s, size_t pos) {
    auto flags = ctx.storage.nodes.get_flags(s);
    if (std::holds_alternative<UInt32>(flags[pos]))
      out.push_back(std::get<UInt32>(flags[pos]).v);
  };
  for (auto s : ctx.storage.nodes.get_args(bbID)) {
    switch (IRI_NODE(ctx, s).tag) {
    case IRI_TAG::Goto:
      add(s, GotoSEXP::FLAG_IDX_IDX);
      break;
    case IRI_TAG::IfJump:
      add(s, IfJumpSEXP::FLAG_IDX_IDX);
      break;
    case IRI_TAG::IfElseJump:
      add(s, IfElseJumpSEXP::FLAG_IDX_TRUE);
      add(s, IfElseJumpSEXP::FLAG_IDX_FALSE);
      break;
    case IRI_TAG::PushCatchContext:
      add(s, PushCatchContextSEXP::FLAG_IDX_IDX);
      break;
    default:
      break;
    }
  }
  return out;
}

RegionClone cloneRegion(IRIContext &ctx, BBContainerSupport &bbc,
                        BBIDX entryIDX) {
  IRID entryBB = bbc.getBBByIDX(entryIDX);
  SCPIDX regionScope = BBSupport(entryBB, ctx).getScopeIDX();

  // Closure from the entry, bounded to the region's scope subtree.
  // Escape targets (break/continue/return) fall outside and stay shared.
  std::set<BBIDX> span;
  std::vector<BBIDX> stack{entryIDX};
  while (!stack.empty()) {
    BBIDX d = stack.back();
    stack.pop_back();
    if (!span.insert(d).second)
      continue;
    for (BBIDX t : bbTargets(ctx, bbc.getBBByIDX(d))) {
      IRID tb = bbc.getBBByIDX(t);
      if (ctx.iris->hasScopePath(BBSupport(tb, ctx).getScopeIDX(), regionScope))
        stack.push_back(t);
    }
  }

  RegionClone rc(ctx);
  for (BBIDX d : span)
    rc.cloner.targetMap[d] = ++ctx.lastBBIDX;

  std::vector<IRID> list = ctx.storage.nodes.get_args(bbc.getArg_BB());
  for (BBIDX d : span) {
    IRID nb = cloneBB(ctx, bbc.getBBByIDX(d), rc.cloner.targetMap[d],
                      rc.cloner);
    list.push_back(nb);
  }
  ctx.storage.nodes.set_args(bbc.getArg_BB(), list);
  rc.entryIDX = rc.cloner.targetMap[entryIDX];

  if (std::getenv("IRI_CLONE_DEBUG")) {
    fprintf(stderr, "[CLONE] region %u -> %u:", entryIDX, rc.entryIDX);
    for (BBIDX d : span)
      fprintf(stderr, " %u->%u", d, rc.cloner.targetMap[d]);
    fprintf(stderr, "\n");
  }
  return rc;
}

} // namespace IRI_STRUCTURAL
