// Generated Stub for IRI_TAG::JSTemplate (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSTemplate
 * Meta:    AMP
 * Arguments: (none)
 * Flags: (none)
 */
std::string printJSTemplateRVal(const PPContext &pp, IRID node) {
  // Quasis and substitutions alternate: `q0${e0}q1...`
  auto a = pp.args(node);
  std::string out = "`";
  for (size_t i = 0; i < a.size(); ++i) {
    if (i % 2 == 1) {
      out += "${" + ppRVal(pp, a[i]) + "}";
    } else if (pp.tagOf(a[i]) == IRI_GEN::String) {
      IRI_GEN::StringSEXP s(a[i], pp.ctx);
      for (char c : pp.str(s.getIridiumPrimitive())) {
        if (c == '`' || c == '\\')
          out += '\\';
        out += c;
      }
    } else {
      out += "${" + ppRVal(pp, a[i]) + "}";
    }
  }
  return out + "`";
}

} // namespace IRI_STRUCTURAL
