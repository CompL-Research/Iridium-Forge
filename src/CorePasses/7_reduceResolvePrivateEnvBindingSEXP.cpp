#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IRIContext.h"
#include "Storage/StringPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/IRIS.hpp"
#include <optional>
#include "Support/FileSupport.hpp"
#include <stdexcept>
#include <string>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

using PrivateInfo = std::pair<std::string, std::string>; // binding, kind

// The private element #x as declared by the innermost enclosing class that
// declares it in any form: a field or method (x), a getter (get:x), a setter
// (set:x).
struct PrivateDecl {
  std::optional<PrivateInfo> plain, getter, setter;
  // The binding #x itself refers to (for reads of the name, e.g. #x in o)
  const PrivateInfo &any() const {
    return plain ? *plain : getter ? *getter : *setter;
  }
};

static PrivateDecl resolvePrivate(IRIContext &ctx, const std::string &name,
                                  double scopeIDX,
                                  BUILD_CTX &iridiumBuildContext) {
  double targetScopeIDX = scopeIDX;
  while (true) {
    auto buildContext = iridiumBuildContext[targetScopeIDX];
    if (buildContext->privateMapping) {
      auto &privateMapping = buildContext->privateMapping.value();
      auto find = [&](const std::string &key) -> std::optional<PrivateInfo> {
        auto it = privateMapping.find(key);
        if (it == privateMapping.end())
          return std::nullopt;
        return it->second;
      };
      PrivateDecl decl{find(name), find("get:" + name), find("set:" + name)};
      if (decl.plain || decl.getter || decl.setter)
        return decl;
    }
    // Move to parent lexical scope before finding the aprent closure scope,
    // otherwise we might end up in infinite loops
    targetScopeIDX = ctx.iris->getLexicalScope(targetScopeIDX);
    if (targetScopeIDX < 0)
      throw std::runtime_error("Failed to resolve private binding : " + name);
  }
}

// A fresh copy of the object read (EnvRead of a not yet resolved binding), so
// it can be used twice without sharing a node.
static IRID copyObjRead(IRIContext &ctx, IRID read) {
  if (IRI_NODE(ctx, read).tag != IRI_GEN::EnvRead)
    throw std::runtime_error("private accessor: expected a read as the object");
  EnvReadSEXP er(read, ctx);
  if (IRI_NODE(ctx, er.getArg_Obj()).tag != IRI_GEN::ResolveEnvBinding)
    throw std::runtime_error("private accessor: expected an unresolved read");
  return EnvReadSEXP::create(
      ctx,
      IRI_HELPERS::createNoASWResolveEnvBindingSEXP(
          ctx, ResolveEnvBindingSEXP(er.getArg_Obj(), ctx).getNAME()),
      er.hasSAFE(), er.hasTAINTED());
}

// Resolves #name to the read of the binding holding its private symbol (a
// field) or function (method / accessor), and picks the operation:
//   obj.#m (method)       -> JSCheckBrand(obj, m)
//   obj.#g (getter)       -> obj->JSCheckBrand(obj, getter)()
//   obj.#s = v (setter)   -> obj->JSCheckBrand(obj, setter)(v)
//   read of a setter-only / write of a method or getter-only -> JSThrowError
// Returns the node to use in place of `node`.
inline IRID patchNode(IRIContext &ctx, IRID node, double startScopeIDX,
                      BUILD_CTX &iridiumBuildContext) {
  auto args = ctx.storage.nodes.get_args(node);
  std::string pvtNameStr;
  std::optional<PrivateDecl> decl;

  for (int i = 0; i < args.size(); i++) {
    auto currID = args[i];
    auto currTag = IRI_NODE(ctx, currID).tag;

    if (currTag == IRI_GEN::ResolvePrivateEnvBinding) {
      ResolvePrivateEnvBindingSEXP pvt(currID, ctx);
      pvtNameStr = std::string(ctx.storage.strings.get(pvt.getNAME()));
      decl = resolvePrivate(ctx, pvtNameStr, startScopeIDX, iridiumBuildContext);
      ctx.storage.nodes.update_arg_inplace(
          node, i,
          IRI_HELPERS::createUnsafeEnvReadSEXP(
              ctx, ctx.storage.strings.intern(decl->any().first)));
    } else {
      IRID patched = patchNode(ctx, currID, startScopeIDX, iridiumBuildContext);
      if (patched != currID)
        ctx.storage.nodes.update_arg_inplace(node, i, patched);
    }
  }
  if (!decl)
    return node;

  auto readOnly = [&]() {
    return JSThrowErrorSEXP::create(
        ctx, ctx.storage.strings.intern("#" + pvtNameStr));
  };
  // obj->obj.#method[fn](...args): brand-checked call of a getter / setter
  auto callAccessor = [&](IRID obj, const PrivateInfo &fn,
                          std::vector<IRID> callArgs) {
    IRID checked = JSCheckBrandSEXP::create(
        ctx, copyObjRead(ctx, obj),
        IRI_HELPERS::createUnsafeEnvReadSEXP(
            ctx, ctx.storage.strings.intern(fn.first)));
    IRID call = CallSiteSEXP::create(ctx, checked, obj);
    std::vector<IRID> all = {checked, obj};
    all.insert(all.end(), callArgs.begin(), callArgs.end());
    ctx.storage.nodes.set_args(call, all);
    return call;
  };
  std::string plainKind = decl->plain ? decl->plain->second : "";

  auto tag = IRI_NODE(ctx, node).tag;
  if (tag == IRI_GEN::JSPrivateFieldRead) {
    JSPrivateFieldReadSEXP read(node, ctx);
    if (plainKind == "PROP")
      return node;
    if (plainKind == "METHOD")
      return JSCheckBrandSEXP::create(ctx, read.getArg_Obj(), read.getArg_Field());
    if (decl->getter)
      return callAccessor(read.getArg_Obj(), *decl->getter, {});
    return readOnly(); // setter only
  }
  if (tag == IRI_GEN::JSPrivateFieldWrite) {
    JSPrivateFieldWriteSEXP write(node, ctx);
    if (plainKind == "PROP")
      return node;
    if (decl->setter)
      return callAccessor(write.getArg_Obj(), *decl->setter,
                          {write.getArg_Value()});
    return readOnly(); // method or getter only
  }
  return node;
}

void _7_RRPEBS(IRIContext &ctx, IRID fileSEXP,
               BUILD_CTX &iridiumBuildContext) {
  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);

    auto currentScopeIDX = bbc.getScopeIDX();

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);
      for (auto [stmtID, offset] : bb.stmts()) {
        IRID patched = patchNode(ctx, stmtID, bb.getScopeIDX(), iridiumBuildContext);
        if (patched != stmtID)
          ctx.storage.nodes.update_arg_inplace(bbID, offset, patched);
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
