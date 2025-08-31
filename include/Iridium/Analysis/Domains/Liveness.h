#pragma once
#include <set>
#include <string>
#include "generated/IridiumTypes.h"

struct Liveness
{
  std::set<std::string> vars;

  static Liveness top() { return Liveness{}; }

  bool operator==(const Liveness &other) const
  {
    return vars == other.vars;
  }

  Liveness merge(const Liveness &other) const
  {
    Liveness res = *this;
    res.vars.insert(other.vars.begin(), other.vars.end());
    return res;
  }

  Liveness transfer(const std::shared_ptr<BBSEXP> &block) const
  {
    Liveness res = *this;
    // Fake transfer: assume "x = ..." kills x, and RHS uses vars
    for (auto &instr : block->args)
    {
      // if (instr.find('=') != std::string::npos)
      // {
      //   auto lhs = instr.substr(0, instr.find('='));
      //   res.vars.erase(lhs);
      //   // fake: everything else is used
      //   res.vars.insert("rhs");
      // }
    }
    return res;
  }
};