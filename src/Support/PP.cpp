#include "Support/PP.hpp"
#include "Config.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumMeta.h"
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
    if (pp.on(binding, RemoteEnvBindingSEXP::FLAG_IDX_MODULE))
      return "m:" + parent;
    if (pp.on(binding, RemoteEnvBindingSEXP::FLAG_IDX_MODULEI))
      return "mi:" + parent;
    if (pp.on(binding, RemoteEnvBindingSEXP::FLAG_IDX_MODULENSI))
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

std::string ppWriteLHS(const PPContext &pp, IRID write) {
  auto tag = pp.tagOf(write);
  if (tag != LWrite && tag != RWrite && tag != GWrite && tag != MWrite)
    throw std::runtime_error("Unexpected write node");

  std::string prefix;
  if (tag == GWrite) {
    GWriteSEXP gWrite(write, pp.ctx);
    if (gWrite.hasDECLVAR())
      prefix = "decl(script) ";
    else if (gWrite.hasDECLFUN())
      prefix = "declFun(script) ";
  }

  return prefix + ppBinding(pp, pp.args(write)[0]) + " " +
         ppWriteOp(pp.on(write, LWriteSEXP::FLAG_IDX_INIT),
                   pp.on(write, LWriteSEXP::FLAG_IDX_SAFE));
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
    if (pp.on(node, CallSiteSEXP::FLAG_IDX_ConstructorCall))
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

std::string lower(std::string_view s) {
  std::string out(s);
  std::transform(out.begin(), out.end(), out.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return out;
}

// Names of the void/bool flags that are set, lower-cased: "arguments proto".
std::string ppFlagWords(const PPContext &pp, IRID node,
                        std::initializer_list<uint32_t> idxs) {
  std::string out;
  auto tag = pp.tagOf(node);
  for (uint32_t idx : idxs) {
    if (!pp.on(node, idx))
      continue;
    if (!out.empty())
      out += " ";
    out += lower(dump_flag(IridiumMeta::get_flag_enum(tag, idx)));
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
    std::string kind = ppFlagWords(pp, file,
                                   {FileSEXP::FLAG_IDX_JSScript,
                                    FileSEXP::FLAG_IDX_JSModule,
                                    FileSEXP::FLAG_IDX_TLA});
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
      std::string what = pp.on(item, StaticImportSEXP::FLAG_IDX_NSIMPORT)
                             ? "*"
                             : std::string(pp.str(s.getFIELD()));
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
      if (pp.on(b, EnvBindingSEXP::FLAG_IDX_JSARG))
        formals.emplace_back(eb.getREFIDX(), name);
      else if (pp.on(b, EnvBindingSEXP::FLAG_IDX_JSRESTARG))
        formals.emplace_back(eb.getREFIDX(), "..." + name);
      else if (pp.on(b, EnvBindingSEXP::FLAG_IDX_JSCONST))
        consts.push_back(name);
      else if (pp.on(b, EnvBindingSEXP::FLAG_IDX_JSVAR))
        vars.push_back(name);
      else
        lets.push_back(name);
    }
    std::sort(formals.begin(), formals.end());

    // Header: function f(p, q)  ; entry BB1, scope 1, argc 2, arguments proto
    std::string kind = containerKind(c.getContainerFlagID());
    if (pp.on(bbc, BBContainerSEXP::FLAG_IDX_ASYNC))
      kind = "async " + kind;
    if (pp.on(bbc, BBContainerSEXP::FLAG_IDX_GENERATOR))
      kind += "*";
    std::string name(c.hasNAME() ? pp.str(c.getNAME()) : "");

    os << "\n" << kind << (name.empty() ? "" : " " + name) << "(";
    for (size_t i = 0; i < formals.size(); ++i)
      os << (i ? ", " : "") << formals[i].second;
    os << ")  ; entry " << ppBB(c.getStartBBIDX()) << ", scope "
       << ppNumber(c.getScopeIDX()) << ", argc " << ppNumber(c.getECMAArgs());

    std::string words = ppFlagWords(
        pp, bbc,
        {BBContainerSEXP::FLAG_IDX_ARGUMENTS, BBContainerSEXP::FLAG_IDX_STRICT,
         BBContainerSEXP::FLAG_IDX_PROTO, BBContainerSEXP::FLAG_IDX_NEW,
         BBContainerSEXP::FLAG_IDX_SCALL, BBContainerSEXP::FLAG_IDX_SOBJ,
         BBContainerSEXP::FLAG_IDX_HOME, BBContainerSEXP::FLAG_IDX_DERIVED,
         BBContainerSEXP::FLAG_IDX_TopLevel});
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
        pp, bb,
        {BBSEXP::FLAG_IDX_TopLevel, BBSEXP::FLAG_IDX_ClosureBoundary,
         BBSEXP::FLAG_IDX_Lexical, BBSEXP::FLAG_IDX_VARBoundary,
         BBSEXP::FLAG_IDX_TryBB});
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
