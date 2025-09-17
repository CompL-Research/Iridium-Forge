#pragma once
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include "Iridium/Analysis/Helpers/UnionedDataMap.h"

template <typename T>
static bool compareAs(const std::shared_ptr<IridiumSEXP> &lhs,
                      const std::shared_ptr<IridiumSEXP> &rhs)
{
  auto l = std::dynamic_pointer_cast<T>(lhs);
  auto r = std::dynamic_pointer_cast<T>(rhs);
  assert(l && r);
  return l->getIridiumPrimitive() == r->getIridiumPrimitive();
}

struct ConstantLatticeValue
{
  enum ValKind
  {
    NAC,
    NUBD,
    Null,

    Boolean,
    Number,
    String,
    JSBigInt,
    BOTTOM
  } kind;

  IRISEXP value;

  void dump(std::ostringstream &oss) const
  {
    switch (kind)
    {
    case NAC:
      oss << "NAC";
      break;
    case NUBD:
      oss << "NUBD";
      break;
    case Null:
      oss << "Null";
      break;
    case Boolean:
    {
      auto b = std::dynamic_pointer_cast<BooleanSEXP>(value);
      oss << "Boolean(" << (b ? (b->getIridiumPrimitive() ? "true" : "false") : "??") << ")";
      break;
    }
    case Number:
    {
      auto n = std::dynamic_pointer_cast<NumberSEXP>(value);
      oss << "Number(" << (n ? std::to_string(n->getIridiumPrimitive()) : "??") << ")";
      break;
    }
    case String:
    {
      auto s = std::dynamic_pointer_cast<StringSEXP>(value);
      oss << "String(\"" << (s ? s->getIridiumPrimitive() : "??") << "\")";
      break;
    }
    case JSBigInt:
    {
      auto bi = std::dynamic_pointer_cast<BitIntSEXP>(value);
      oss << "BigInt(" << (bi ? bi->getIridiumPrimitive() : "??") << ")";
      break;
    }
    case BOTTOM:
      oss << "BOTTOM";
      break;
    default:
      oss << "UnknownKind";
      break;
    }
  }

  static ConstantLatticeValue generate(IRISEXP val)
  {
    if (std::dynamic_pointer_cast<JSNUBDSEXP>(val))
      return ConstantLatticeValue{NUBD, NULL};
    if (std::dynamic_pointer_cast<NullSEXP>(val))
      return ConstantLatticeValue{Null, NULL};
    if (std::dynamic_pointer_cast<BooleanSEXP>(val))
      return ConstantLatticeValue{Boolean, val};
    if (std::dynamic_pointer_cast<NumberSEXP>(val))
      return ConstantLatticeValue{Number, val};
    if (std::dynamic_pointer_cast<StringSEXP>(val))
      return ConstantLatticeValue{String, val};
    if (std::dynamic_pointer_cast<BitIntSEXP>(val))
      return ConstantLatticeValue{JSBigInt, val};

    return ConstantLatticeValue{NAC, NULL};
  }

  bool operator==(const ConstantLatticeValue &other) const
  {
    if (kind != other.kind)
      return false;
    if (kind <= Null)
      return true;

    switch (kind)
    {
    case Boolean:
      return compareAs<BooleanSEXP>(value, other.value);
    case Number:
      return compareAs<NumberSEXP>(value, other.value);
    case String:
      return compareAs<StringSEXP>(value, other.value);
    case JSBigInt:
      return compareAs<BitIntSEXP>(value, other.value);
    case BOTTOM:
      throw std::runtime_error("comparing BOTTOM values?");
    default:
      throw std::runtime_error("Unreachable");
    }
  }

  ConstantLatticeValue merge(const ConstantLatticeValue &other) const
  {
    if (kind == BOTTOM && other.kind == BOTTOM)
      throw std::runtime_error("Merging BOTTOM values??");

    // One is bottom, return the other
    if (kind == BOTTOM)
      return ConstantLatticeValue{other.kind, other.value};
    if (other.kind == BOTTOM)
      return ConstantLatticeValue{kind, value};

    if (kind != other.kind)
      return ConstantLatticeValue{NAC, value};

    if (kind == NAC)
      return ConstantLatticeValue{NAC, NULL};
    if (kind == NUBD)
      return ConstantLatticeValue{NUBD, NULL};
    if (kind == Null)
      return ConstantLatticeValue{Null, NULL};

    if (*this == other)
      return ConstantLatticeValue{kind, value};
    else
      return ConstantLatticeValue{NAC, NULL};
  }
};

struct ConstantsAtStmt
{
  using DFVT = UnionedDataMap<IRISEXP, ConstantLatticeValue>;

  DFVT dfv;

  void dump(std::ostringstream &oss) const
  {
    oss << "{" << std::endl;
    for (const auto &kv : dfv.store)
    {
      oss << kv.first->getFlagString("NAME") << " → ";
      kv.second.dump(oss);
      oss << std::endl;
    }
    oss << "}";
  }

  bool operator==(const ConstantsAtStmt &other) const
  {
    return dfv == other.dfv;
  }

  ConstantsAtStmt merge(const ConstantsAtStmt &other) const
  {
    auto res = clone();
    res.dfv = res.dfv.merge(other.dfv);
    return res;
  }

  static ConstantsAtStmt boundary() { return ConstantsAtStmt(); }
  static ConstantsAtStmt bottom() { return ConstantsAtStmt(); }

  ConstantsAtStmt clone() const
  {
    ConstantsAtStmt copy;
    copy.dfv.store = dfv.store; // copy underlying map
    return copy;
  }

  ConstantsAtStmt transfer(const std::shared_ptr<BBSEXP> &bb) const;

  ConstantsAtStmt iter(const std::shared_ptr<BBSEXP> & bb, std::function<void(size_t, ConstantsAtStmt)> callback) const;
};