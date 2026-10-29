#include "Support/PP.hpp"
#include "Config.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Storage/IRIContext.h"
#include "Support/PP/PPDispatch.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace IRI_STRUCTURAL {

using namespace IRI_GEN;

std::string ppName(std::string_view raw) {
  if (raw.starts_with("~$")) {
    std::string out = "%";
    for (char c : raw.substr(2))
      if (c != '$')
        out += c;
    return out;
  }
  return std::string(raw);
}

std::string ppStringLit(std::string_view s) {
  std::ostringstream oss;
  IRI_STORAGE::append_escaped_json_string(oss, s);
  return oss.str();
}

std::string ppNumber(double d) {
  if (std::isnan(d))
    return "NaN";
  if (std::isinf(d))
    return d > 0 ? "Infinity" : "-Infinity";
  if (d == 0 && std::signbit(d))
    return "-0";
  char buf[64];
  auto res = std::to_chars(buf, buf + sizeof(buf), d);
  return std::string(buf, res.ptr);
}

std::string ppBinding(const PPContext &pp, IRID binding) {
  switch (pp.tagOf(binding)) {
  case EnvBinding:
    return ppName(pp.str(EnvBindingSEXP(binding, pp.ctx).getNAME()));
  case ScriptBinding:
    return "s:" + ppName(pp.str(ScriptBindingSEXP(binding, pp.ctx).getNAME()));
  case GlobalBinding:
    return "g:" + ppName(pp.str(GlobalBindingSEXP(binding, pp.ctx).getNAME()));
  case RemoteEnvBinding: {
    RemoteEnvBindingSEXP sexp(binding, pp.ctx);
    std::string parent = sexp.hasArg_ParentReference()
                             ? ppBinding(pp, sexp.getArg_ParentReference())
                             : "?";
    if (sexp.hasMODULE())
      return "m:" + parent;
    if (sexp.hasMODULEI())
      return "mi:" + parent;
    if (sexp.hasMODULENSI())
      return "mns:" + parent;
    return "^" + parent;
  }
  default:
    return ppRVal(pp, binding);
  }
}

static bool isIdentifier(std::string_view s) {
  if (s.empty() || std::isdigit(static_cast<unsigned char>(s[0])))
    return false;
  return std::all_of(s.begin(), s.end(), [](char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' ||
           c == '$';
  });
}

std::string ppMember(const PPContext &pp, IRID obj, IRID key) {
  std::string base = ppOperand(pp, obj);
  if (pp.tagOf(key) == String) {
    StringSEXP s(key, pp.ctx);
    if (s.hasIridiumPrimitive()) {
      std::string_view name = pp.str(s.getIridiumPrimitive());
      if (isIdentifier(name))
        return base + "." + std::string(name);
    }
  }
  return base + "[" + ppRVal(pp, key) + "]";
}

const char *ppWriteOp(bool init, bool safe) {
  if (init)
    return "<-";
  return safe ? "=" : ".=";
}

template <typename S>
static std::string writeLHS(const PPContext &pp, const S &write,
                            const std::string &prefix = "") {
  return prefix + ppBinding(pp, write.getArg_LValTarget()) + " " +
         ppWriteOp(write.hasINIT(), write.hasSAFE());
}

std::string ppWriteLHS(const PPContext &pp, IRID write) {
  switch (pp.tagOf(write)) {
  case LWrite:
    return writeLHS(pp, LWriteSEXP(write, pp.ctx));
  case RWrite:
    return writeLHS(pp, RWriteSEXP(write, pp.ctx));
  case MWrite:
    return writeLHS(pp, MWriteSEXP(write, pp.ctx));
  case GWrite: {
    GWriteSEXP gWrite(write, pp.ctx);
    if (gWrite.hasDECLVAR())
      return writeLHS(pp, gWrite, "decl(script) ");
    if (gWrite.hasDECLFUN())
      return writeLHS(pp, gWrite, "declFun(script) ");
    return writeLHS(pp, gWrite);
  }
  default:
    throw std::runtime_error("Unexpected write node");
  }
}

std::string ppBB(double idx) { return "BB" + ppNumber(idx); }

std::string ppOperand(const PPContext &pp, IRID node) {
  std::string s = ppRVal(pp, node);
  switch (pp.tagOf(node)) {
  case Binop:
  case JSBinop:
  case Unop:
  case JSUnop:
  case Await:
  case Yield:
  case UNOPDelMemberExpr:
  case UNOPDelVar:
  case JSClass:
    return "(" + s + ")";
  case CallSite:
    if (CallSiteSEXP(node, pp.ctx).hasConstructorCall())
      return "(" + s + ")";
    return s;
  default:
    return s;
  }
}

std::string ppList(const PPContext &pp, std::span<const IRID> args,
                   size_t from) {
  std::string out;
  for (size_t i = from; i < args.size(); ++i) {
    if (i != from)
      out += ", ";
    out += ppRVal(pp, args[i]);
  }
  return out;
}

namespace {

// Words for the flags that are set: "arguments proto".
std::string ppFlagWords(
    std::initializer_list<std::pair<bool, const char *>> flags) {
  std::string out;
  for (auto [set, word] : flags) {
    if (!set)
      continue;
    if (!out.empty())
      out += " ";
    out += word;
  }
  return out;
}

const char *containerKind(double flagID) {
  switch (static_cast<int>(flagID)) {
  case CF_TOP_LEVEL_MODULE:
    return "module";
  case CF_TOP_LEVEL_SCRIPT:
    return "script";
  case CF_ARROW_FUNCTION:
    return "arrow";
  case CF_FUNCTION:
    return "function";
  case CF_CTR:
    return "ctor";
  case CF_DERIVED_CTR:
    return "derived-ctor";
  case CF_CLASS_METHOD:
    return "method";
  case CF_PROP_INIT:
    return "prop-init";
  default:
    return "closure";
  }
}

class Printer {
public:
  Printer(std::ostream &os, const PPContext &pp) : os(os), pp(pp) {}

  void printFile(IRID file) {
    if (pp.tagOf(file) != File) {
      os << ppGeneric(pp, file) << "\n";
      return;
    }
    FileSEXP f(file, pp.ctx);
    std::string kind = ppFlagWords({{f.hasJSScript(), "jsscript"},
                                    {f.hasJSModule(), "jsmodule"},
                                    {f.hasTLA(), "tla"}});
    os << "; file" << (kind.empty() ? "" : " [" + kind + "]") << "\n";

    for (IRID child : pp.args(file)) {
      if (pp.tagOf(child) == List)
        printModuleList(child);
      else if (pp.tagOf(child) == BBContainer)
        printContainer(child);
      else
        os << "\n" << ppGeneric(pp, child) << "\n";
    }
  }

private:
  std::ostream &os;
  const PPContext &pp;

  // ModuleRequest / StaticImport / exports: only printed when non-empty.
  void printModuleList(IRID list) {
    auto items = pp.args(list);
    if (items.empty())
      return;
    for (IRID item : items)
      os << moduleItem(item) << "\n";
  }

  std::string moduleItem(IRID item) {
    switch (pp.tagOf(item)) {
    case ModuleRequest: {
      ModuleRequestSEXP s(item, pp.ctx);
      return "request #" + ppNumber(s.getREQIDX()) + " " +
             ppStringLit(pp.str(s.getSOURCE()));
    }
    case StaticImport: {
      StaticImportSEXP s(item, pp.ctx);
      std::string what =
          s.hasNSIMPORT() ? "*" : std::string(pp.str(s.getFIELD()));
      return "import " + what + " from #" + ppNumber(s.getMODULEREQIDX()) +
             " -> " + ppBinding(pp, s.getArg_StorageLocation());
    }
    case LocalStaticExport: {
      LocalStaticExportSEXP s(item, pp.ctx);
      return "export " + ppBinding(pp, s.getArg_StorageLocation()) + " as " +
             std::string(pp.str(s.getEXPORTNAME()));
    }
    case NamedReexport: {
      NamedReexportSEXP s(item, pp.ctx);
      return "export " + std::string(pp.str(s.getLOCALNAME())) + " as " +
             std::string(pp.str(s.getEXPORTNAME())) + " from #" +
             ppNumber(s.getMODULEREQIDX());
    }
    case StarExport: {
      StarExportSEXP s(item, pp.ctx);
      return "export * from #" + ppNumber(s.getMODULEREQIDX());
    }
    default:
      return ppGeneric(pp, item);
    }
  }

  void printContainer(IRID bbc) {
    BBContainerSEXP c(bbc, pp.ctx);
    BindingsSEXP bindings(c.getArg_Bindings(), pp.ctx);

    // Formals come from the local bindings, ordered by their REFIDX.
    std::vector<std::pair<double, std::string>> formals;
    std::vector<std::string> lets, consts, vars;
    for (IRID b : pp.args(bindings.getArg_LocalBindings())) {
      if (pp.tagOf(b) != EnvBinding) {
        lets.push_back(ppGeneric(pp, b));
        continue;
      }
      EnvBindingSEXP eb(b, pp.ctx);
      std::string name = ppBinding(pp, b);
      if (eb.hasJSARG())
        formals.emplace_back(eb.getREFIDX(), name);
      else if (eb.hasJSRESTARG())
        formals.emplace_back(eb.getREFIDX(), "..." + name);
      else if (eb.hasJSCONST())
        consts.push_back(name);
      else if (eb.hasJSVAR())
        vars.push_back(name);
      else
        lets.push_back(name);
    }
    std::sort(formals.begin(), formals.end());

    // Header: function f(p, q)  ; entry BB1, scope 1, argc 2, arguments proto
    std::string kind = containerKind(c.getContainerFlagID());
    if (c.hasASYNC())
      kind = "async " + kind;
    if (c.hasGENERATOR())
      kind += "*";
    std::string name(c.hasNAME() ? pp.str(c.getNAME()) : "");

    os << "\n" << kind << (name.empty() ? "" : " " + name) << "(";
    for (size_t i = 0; i < formals.size(); ++i)
      os << (i ? ", " : "") << formals[i].second;
    os << ")  ; entry " << ppBB(c.getStartBBIDX()) << ", scope "
       << ppNumber(c.getScopeIDX()) << ", argc " << ppNumber(c.getECMAArgs());

    std::string words = ppFlagWords(
        {{c.hasARGUMENTS(), "arguments"}, {c.hasSTRICT(), "strict"},
         {c.hasPROTO(), "proto"}, {c.hasNEW(), "new"},
         {c.hasSCALL(), "scall"}, {c.hasSOBJ(), "sobj"},
         {c.hasHOME(), "home"}, {c.hasDERIVED(), "derived"},
         {c.hasTopLevel(), "toplevel"}});
    if (!words.empty())
      os << ", " << words;
    if (pp.showIds)
      os << "  #" << bbc;
    os << "\n";

    printNames("let", lets);
    printNames("const", consts);
    printNames("var", vars);

    std::vector<std::string> remotes;
    for (IRID r : pp.args(bindings.getArg_RemoteBindings()))
      remotes.push_back(ppBinding(pp, r));
    printNames("capture", remotes);

    std::vector<std::string> pool;
    for (IRID p : pp.args(bindings.getArg_Lambdas()))
      pool.push_back(ppRVal(pp, p));
    printNames("pool", pool);

    for (IRID bb : pp.args(c.getArg_BB()))
      printBB(bb);
  }

  void printNames(const char *label, const std::vector<std::string> &names) {
    if (names.empty())
      return;
    os << "  " << std::left << std::setw(8) << label;
    for (size_t i = 0; i < names.size(); ++i)
      os << (i ? ", " : "") << names[i];
    os << "\n";
  }

  void printBB(IRID bb) {
    if (pp.tagOf(bb) != BB) {
      os << "  " << ppGeneric(pp, bb) << "\n";
      return;
    }
    BBSEXP b(bb, pp.ctx);
    os << "\n  " << ppBB(b.getIDX()) << ":  ; scope " << ppNumber(b.getScopeIDX());
    std::string words = ppFlagWords(
        {{b.hasTopLevel(), "toplevel"},
         {b.hasClosureBoundary(), "closureboundary"},
         {b.hasLexical(), "lexical"},
         {b.hasVARBoundary(), "varboundary"},
         {b.hasTryBB(), "trybb"}});
    if (!words.empty())
      os << ", " << words;
    os << "\n";

    for (IRID stmt : pp.args(bb)) {
      os << "    " << ppStatement(pp, stmt);
      if (pp.showIds)
        os << "  ; #" << stmt;
      os << "\n";
    }
  }
};

} // namespace

void prettyPrint(std::ostream &os, IRI_STORAGE::IRIContext &ctx, IRID root,
                 bool showIds) {
  PPContext pp(ctx);
  pp.showIds = showIds;
  Printer(os, pp).printFile(root);
}

std::string ppGeneric(const PPContext &pp, IRID node) {
  throw std::runtime_error("[PP] ppGeneric case for " + dump_tag(IRI_NODE(pp.ctx, node).tag));
}

} // namespace IRI_STRUCTURAL
