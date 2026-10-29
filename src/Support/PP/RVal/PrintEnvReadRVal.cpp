// Generated Stub for IRI_TAG::EnvRead (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: EnvRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 *   - void   TAINTED -> sexp.hasTAINTED()
 */
std::string printEnvReadRVal(const PPContext &pp, IRID node) {
  // Reads that may throw (TDZ check) are marked: x!
  // Arguments, globals and pool bindings never lower to a checked read, so
  // SAFE does not apply to them and they are never marked.
  IRI_GEN::EnvReadSEXP sexp(node, pp.ctx);
  IRID binding = sexp.getArg_Obj();
  bool checked = !sexp.hasSAFE();
  switch (pp.tagOf(binding)) {
  case IRI_GEN::GlobalBinding:
  case IRI_GEN::PoolBinding:
    checked = false;
    break;
  case IRI_GEN::EnvBinding: {
    IRI_GEN::EnvBindingSEXP eb(binding, pp.ctx);
    if (eb.hasJSARG() || eb.hasJSRESTARG())
      checked = false;
    break;
  }
  default:
    break;
  }
  std::string out = ppBinding(pp, binding);
  if (checked)
    out += "!";
  if (sexp.hasTAINTED())
    out = "taint(" + out + ")";
  return out;
}

} // namespace IRI_STRUCTURAL
