// Generated Stub for IRI_TAG::JSCTX (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <iterator>
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCTX
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - double OPID -> sexp.getOPID()
 */
std::string printJSCTXRVal(const PPContext &pp, IRID node) {
  // Implicit binding initializers, see NewBindingCreation.ts for the OPIDs.
  static const char *names[] = {
      "arguments",  "mapped_arguments", "active_func", "new_target",
      "home_object", "var_obj",         "module_meta", "super_ctr",
      "super_obj",  "this",             "nubd",        "ret",
      "active_func_name"};
  IRI_GEN::JSCTXSEXP sexp(node, pp.ctx);
  double opid = sexp.getOPID();
  std::string name = (opid >= 0 && opid < std::size(names))
                         ? names[static_cast<size_t>(opid)]
                         : "op" + ppNumber(opid);
  auto a = pp.args(node);
  return "ctx." + name + (a.empty() ? "" : "(" + ppList(pp, a) + ")");
}

} // namespace IRI_STRUCTURAL
