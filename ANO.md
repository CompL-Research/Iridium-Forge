
> This file documents the analysis and optimizations passes implemented for Iridium IR

# Analysis

## 1. ConstantsAtStmt
```
ConstantsAtStmt: 
  This is a forward analysis that computes the {binding -> CONST} pairs at each basic block boundary.

  Boundary = Empty Set

  Dataflow Value: 
    UnionedDataMap<IRISEXP{EnvBindingSEXP | RemoteEnvBindingSEXP}, ConstantLattice> dfv

  Equality:
    ... inherited

  Merge:
    ... inherited

  Transfer (block):
    nextDfv = clone(dfv)
    for (stmt of block)
    {
      if (stmt isa EnvWriteSEXP)
      {
        RVAL;
        if (stmt.rVal == EnvWriteSEXP) RVAL = ConstantLattice{{stmt.rVal.rVal}};
        else RVAL = ConstantLattice{{stmt.rVal}};

        if (stmt.lVal == EnvBindingSEXP || RemoteEnvBindingSEXP)
          nextDfv[stmt.lVal].merge(RVAL)

        if (stmt.rVal == EnvWriteSEXP && stmt.rVal.lVal == EnvBindingSEXP || RemoteEnvBindingSEXP)
          nextDfv[stmt.rVal.lVal].merge(RVAL)
      }
    }
    return nextDfv
```

## 2. TDZA
```
TDZA:
  This analysis computes whether a binding MAY be in a temporal dead zone.

  Boundary = { Set of all non-captured LocalBindings initialized to SAFE }

  Dataflow Value: 
    UnionedDataMap<IRISEXP{EnvBindingSEXP}, TDZLattice> dfv

  Equality:
    ... inherited

  Merge:
    ... inherited

  Transfer (block):
    nextDfv = clone(dfv)
    for (stmt of block)
    {
      if (stmt isa EnvWriteSEXP)
      {
        RVAL;
        if (stmt.rVal == EnvWriteSEXP) RVAL = stmt.rVal.rVal;
        else RVAL = stmt.rVal.rVal;

        if (stmt.lVal == EnvBindingSEXP)
        {
          if (SAFE)
          {
            // skip if stmt.lVal if the key does not exist in the set already
            nextDfv[stmt.lVal] = RVAL == NUBD ? TDZ : SAFE;
          }
        }
        
        if (stmt.rVal == EnvWriteSEXP && stmt.rVal.lVal == EnvBindingSEXP)
        {
          if (SAFE && RVAL == NUBD)
          {
            // skip if stmt.rVal.lVal if the key does not exist in the set already
            nextDfv[stmt.rVal.lVal] = RVAL == NUBD ? TDZ : SAFE;
          }
        }
      }
    }
    return nextDfv
```

# Transformations

## 1. ConstantProp
```
ConstantProp <: (ConstantsAtStmt: D1)

  Transform (block, D1):
    for (stmt of block):
      DataAtStmt = D1[stmt]
      forEachConstantAtStmt -> replace references to constants with the constants
```

## 2. WriteBarrierReduction
```
WriteBarrierReduction <: (TDZA: D1)

  Transform (block, D1):
    for (stmt of block):
      DataAtStmt = D1[stmt]
      reduceEnvWriteSEXP -> Loosen safety if the binding being written to is in the SAFE zone, nothing otherwise.
```

# Lattices

## 1. ConstantLattice

```
ConstantLattice:

  kind = enum {
    NAC,
    NUBD,
    Boolean,
    Null,
    Number,
    String,
    JSBitInt,
    // JSPrivate, not sure this can really be treated as a constant
    BOTTOM
  };

  val : IRISEXP;

  generate(rval):
    if (NUBD) return NUBD
    if (Null) return Null
    if (Boolean) return Boolean
    if (Number) return Number
    if (String) return String
    if (JSBigInt) return JSBigInt
    return NAC

  Equality(this, other):
    if (this.kind == other.kind)
    {
      if (NAC || NUBD || Null) return true
      if (Boolean) ... ensure both boolean values are the same
      if (Number) ... ensure both number values are the same
      if (String) ... ensure both string values are the same
      if (JSBigInt) ... ensure both BigInt values are the same
      unreachable
    }
    else return false

  Merge(this, other):
    if either one is BOTTOM return the other
    if (this.kind != other.kind) return NAC

    if (NAC) return NAC
    if (NUBD) return NUBD
    if (Null) return Null
    if (Boolean) divergence creates NAC, otherwise the value
    if (Number) divergence creates NAC, otherwise the value
    if (String) divergence creates NAC, otherwise the value
    if (JSBigInt) divergence creates NAC, otherwise the value
    unreachable
```


## 2. TDZLattice

```
TDZLattice:

  kind = enum {
    TDZ,
    SAFE
  };

  generate(rval):
    if (NUBD) return TDZ
    return SAFE


  Equality(this, other):
    return this.kind == other.kind

  Merge(this, other):
    if either one is TDZ return TDZ
    return SAFE
```

# Auxiliary
```
1. UnionedDataMap
  Equality:
    Both have the same keys and their corresponding values are also equal

  Merge:
    Union of all the keys, if any key is common take the merge     
```
