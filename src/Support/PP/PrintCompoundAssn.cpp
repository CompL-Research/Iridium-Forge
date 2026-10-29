// Generated Stub for IRI_TAG::CompoundAssn
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: CompoundAssn
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
// Names of the values the rhs pushes, in pop order (top of stack first).
static std::vector<std::string> slotNames(const PPContext &pp, IRID rhs,
                                          size_t n) {
  switch (pp.tagOf(rhs)) {
  case IRI_GEN::JSForOfNext:
  case IRI_GEN::JSForInNext:
    return {"done", "value"};
  case IRI_GEN::Yield: // return: resumed via .return(v) rather than .next(v)
    return {"return", "value"};
  case IRI_GEN::JSAppend:
    return {"idx", "array"};
  case IRI_GEN::JSClass:
    return {"proto", "ctor"};
  default: {
    std::vector<std::string> names;
    for (size_t i = 0; i < n; ++i)
      names.push_back("#" + std::to_string(i));
    return names;
  }
  }
}

std::string printCompoundAssn(const PPContext &pp, IRID node) {
  // [rhs, write...]: rhs pushes one value per write (whose own RVal is a Null
  // placeholder); writes pop them in order. Printed as object destructuring,
  // one slot per line (the caller indents continuation lines):
  //   {
  //     done: <loop-done>,
  //     value: <loop-next>
  //   } .= forof.next(it)
  // Mixed operators keep theirs per write: `proto: a <-` ... `} := rhs`
  auto a = pp.args(node);
  if (a.size() < 2)
    throw std::runtime_error("[PP] compound assn needs an rhs and a write");

  std::vector<PPWrite> writes;
  for (size_t i = 1; i < a.size(); ++i)
    writes.push_back(ppWriteParts(pp, a[i]));
  std::vector<std::string> names = slotNames(pp, a[0], writes.size());
  if (names.size() != writes.size())
    throw std::runtime_error("[PP] compound assn slot count mismatch");

  bool sameOp = std::all_of(writes.begin(), writes.end(), [&](const PPWrite &w) {
    return std::string_view(w.op) == writes.front().op;
  });

  std::string out = "{";
  for (size_t i = 0; i < writes.size(); ++i) {
    out += i ? ",\n  " : "\n  ";
    out += names[i] + ": " + writes[i].target;
    if (!sameOp)
      out += std::string(" ") + writes[i].op;
  }
  out += std::string("\n} ") + (sameOp ? writes.front().op : ":=");
  return out + " " + ppRVal(pp, a[0]);
}

} // namespace IRI_STRUCTURAL
