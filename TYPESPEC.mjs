export default [
  {
    "tag": "File",
    "meta": "",
    "args": [],
    "flags": {
      "string": [],
      "void": ["JSScript", "JSModule", "TLA"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "ResolveEnvBinding",
    "meta": "*RVAL",
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": ["ASW"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "List",
    "meta": "",
    "args": [],
    "flags": {
      "string": ["TYPE"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSImplicitBindingDeclaration",
    "meta": "*STMT",
    "args": ["Store", "Args"],
    "flags": {
      "string": ["NAME"],
      "void": ["JSLET", "JSCONST", "JSVAR", "SLOPPY", "SKIPINIT"],
      "bool": ["SAFE", "THISINIT"],
      "double": ["OPID"],
    }
  },
  {
    "tag": "EnvRead",
    "meta": "AMP",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": ["SAFE", "TAINTED"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "TDZRead",
    "meta": "*STMT",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": ["SAFE"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "String",
    "meta": "RVAL",
    "args": [],
    "flags": {
      "string": ["IridiumPrimitive"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "FieldRead",
    "meta": "AMP",
    "args": ["Obj", "Field"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSExplicitBindingDeclaration",
    "meta": "*STMT",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["JSLET", "JSCONST", "JSVAR", "SLOPPY"],
      "bool": ["SAFE", "THISINIT"],
      "double": [],
    }
  },
  {
    "tag": "JSExplicitBindingDeclarationN",
    "meta": "*STMT",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["JSLET", "JSCONST", "JSVAR", "SLOPPY"],
      "bool": ["SAFE", "THISINIT"],
      "double": [],
    }
  },
  {
    "tag": "JSExplicitBindingDeclarationX",
    "meta": "*STMT",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["JSLET", "JSCONST", "JSVAR", "SLOPPY"],
      "bool": ["SAFE", "THISINIT"],
      "double": [],
    }
  },
  {
    "tag": "CallSite",
    "meta": "AMP",
    "args": [],
    "flags": {
      "string": [],
      "void": ["CCall", "ConstructorCall", "PrivateCall", "Import", "Super", "V8Intrinsic", "TAILCALL"],
      "bool": [],
      "double": ["JSDirectEval"],
    }
  },
  {
    "tag": "Apply",
    "meta": "AMP",
    "args": ["Callee", "Context", "ArgList"],
    "flags": {
      "string": [],
      "void": ["ConstructorCall", "Super"],
      "bool": [],
      "double": ["JSDirectEval"],
    }
  },
  {
    "tag": "ReturnAsync",
    "meta": "STMT",
    "args": ["RetVal"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "BB",
    "meta": "",
    "args": [],
    "flags": {
      "string": [],
      "void": ["TopLevel", "ClosureBoundary", "Lexical", "VARBoundary", "TryBB"],
      "bool": [],
      "double": ["IDX", "ScopeIDX"],
    }
  },
  {
    "tag": "Return",
    "meta": "STMT",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "UnresolvedReturn",
    "meta": "*STMT",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "IfJump",
    "meta": "STMT",
    "args": ["Test"],
    "flags": {
      "string": [],
      "void": ["NOT"],
      "bool": [],
      "double": ["IDX"],
    }
  },
  {
    "tag": "IfElseJump",
    "meta": "STMT",
    "args": ["Test"],
    "flags": {
      "string": [],
      "void": ["NOT"],
      "bool": [],
      "double": ["TRUE", "FALSE"],
    }
  },
  {
    "tag": "Goto",
    "meta": "STMT",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["IDX"],
    }
  },
  {
    "tag": "JSFuncDecl",
    "meta": "*STMT",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["JSLET", "JSCONST", "JSVAR", "SLOPPY"],
      "bool": ["SAFE", "THISINIT"],
      "double": [],
    }
  },
  {
    "tag": "Lambda",
    "meta": "RVAL",
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": [],
      "bool": ["CNAME", "SETNAME"],
      "double": ["StartBBIDX"],
    }
  },
  {
    "tag": "NOP",
    "meta": "STMT",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "BBContainer",
    "meta": "",
    "args": ["Bindings", "BB"],
    "flags": {
      "string": ["NAME"],
      "void": ["ARGUMENTS", "ASYNC", "STRICT", "GENERATOR", "PROTO", "NEW", "SCALL", "SOBJ", "HOME", "DERIVED", "TopLevel"],
      "bool": [],
      "double": ["ECMAArgs", "StartBBIDX", "ScopeIDX", "ContainerFlagID"],
    }
  },
  {
    "tag": "Bindings",
    "meta": "",
    "args": ["LocalBindings", "RemoteBindings", "Lambdas"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "StarExport",
    "meta": "",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["MODULEREQIDX"],
    }
  },
  {
    "tag": "StaticImport",
    "meta": "",
    "args": ["StorageLocation"],
    "flags": {
      "string": ["FIELD"],
      "void": ["NSIMPORT"],
      "bool": [],
      "double": ["MODULEREQIDX"],
    }
  },
  {
    "tag": "LocalStaticExport",
    "meta": "",
    "args": ["StorageLocation"],
    "flags": {
      "string": ["LOCALNAME", "EXPORTNAME"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "NamedReexport",
    "meta": "",
    "args": [],
    "flags": {
      "string": ["LOCALNAME", "EXPORTNAME"],
      "void": [],
      "bool": [],
      "double": ["MODULEREQIDX"],
    }
  },
  {
    "tag": "ModuleRequest",
    "meta": "",
    "args": [],
    "flags": {
      "string": ["SOURCE"],
      "void": [],
      "bool": [],
      "double": ["REQIDX"],
    }
  },
  {
    "tag": "EnvBinding",
    "meta": "RVAL",
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": ["JSARG", "JSRESTARG", "JSLET", "JSCONST", "JSVAR"],
      "bool": [],
      "double": ["REFIDX", "SCOPE", "NEXT", "LINK"],
    }
  },
  {
    "tag": "RemoteEnvBinding",
    "meta": "RVAL",
    "args": ["ParentReference"],
    "flags": {
      "string": [],
      "void": ["MODULE", "MODULEI", "MODULENSI"],
      "bool": [],
      "double": ["REFIDX", "LINK"],
    }
  },
  {
    "tag": "GlobalBinding",
    "meta": "RVAL",
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": [],
      "bool": [],
      "double": ["LINK"],
    }
  },
  {
    "tag": "ScriptBinding",
    "meta": "RVAL",
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": ["JSLET", "JSCONST", "JSVAR"],
      "bool": [],
      "double": ["LINK"],
    }
  },
  {
    "tag": "EnvWrite",
    "meta": "*STMT",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["SLOPPY"],
      "bool": ["SAFE", "THISINIT"],
      "double": [],
    }
  },
  {
    "tag": "SiblingSpecialWrite",
    "meta": "*STMT",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["SLOPPY"],
      "bool": ["SAFE", "THISINIT"],
      "double": ["ScopeIDX"],
    }
  },
  {
    "tag": "JSNUBD",
    "meta": "RVAL",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "Number",
    "meta": "RVAL",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["IridiumPrimitive"],
    }
  },
  {
    "tag": "JSClass",
    "meta": "RVAL",
    "args": ["Parent", "Constructor"],
    "flags": {
      "string": ["NAME"],
      "void": ["DERIVED"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSCheckConstructor",
    "meta": "STMT",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "ResolvePrivateEnvBinding",
    "meta": "*RVAL",
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": ["FULLY_RESOLVE"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "PVTEnvRead",
    "meta": "AMP",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": ["SYMBOL", "METHOD", "FULLY_RESOLVE"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSPrivate",
    "meta": "RVAL",
    "args": [],
    "flags": {
      "string": ["IridiumPrimitive"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSPrivateFieldWrite",
    "meta": "STMT",
    "args": ["Obj", "Field", "Value"],
    "flags": {
      "string": [],
      "void": ["DECL"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSADDBRAND",
    "meta": "STMT",
    "args": ["Obj", "HomeObj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSPrivateFieldRead",
    "meta": "AMP",
    "args": ["Obj", "Field"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "PoolBinding",
    "meta": "RVAL",
    "args": ["Lambda"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["REFIDX"],
    }
  },
  {
    "tag": "ResolveContinueTarget",
    "meta": "*STMT",
    "args": [],
    "flags": {
      "string": ["Label"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "ResolveBreakTarget",
    "meta": "*STMT",
    "args": [],
    "flags": {
      "string": ["Label"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSForOfIteratorClose",
    "meta": "STMT",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "PopCatchContext",
    "meta": "STMT",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "PopFinalizerReturnTarget",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "InvokeFinalizer",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["IDX"],
    }
  },
  {
    "tag": "JSForInStart",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSForInNext",
    "args": ["IteratorObj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSForOfStart",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": ["AWAIT"],
      "double": [],
    }
  },
  {
    "tag": "JSForOfNext",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": ["AWAIT"],
      "double": [],
    }
  },
  {
    "tag": "PushCatchContext",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["IDX"],
    }
  },
  {
    "tag": "JSCatchContext",
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "Throw",
    "args": ["ThrowVal"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "Ret",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSBinop",
    "args": ["LBinop", "RBinop"],
    "flags": {
      "string": ["OP"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "FieldWrite",
    "args": ["Obj", "Field", "Value"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSUnop",
    "args": ["Val"],
    "flags": {
      "string": ["OP"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "Unop",
    "args": ["Val"],
    "flags": {
      "string": ["OP"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSObject",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "Boolean",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": ["IridiumPrimitive"],
      "double": [],
    }
  },
  {
    "tag": "JSDefineObjProp",
    "args": ["TargetObj", "Key", "Value"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSArray",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "Binop",
    "args": ["LBinop", "RBinop"],
    "flags": {
      "string": ["OP"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "Null",
    "args": [],
    "flags": {
      "string": [],
      "void": ["IridiumPrimitive"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSComputedFieldRead",
    "args": ["Obj", "Field"],
    "flags": {
      "string": [],
      "void": ["SAFE"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSComputedFieldWrite",
    "args": ["Obj", "Field", "Value"],
    "flags": {
      "string": [],
      "void": ["SAFE"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSSuperFieldRead",
    "args": ["This", "Super", "Field"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSSuperFieldWrite",
    "args": ["This", "Super", "Field", "Value"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSToObject",
    "args": ["TargetObj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSAppend",
    "args": ["TargetObj", "InsertionIdx", "SpreadObj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSDefineObjMethod",
    "args": ["TargetObj", "Key", "Value"],
    "flags": {
      "string": [],
      "void": ["NOENUM", "METHOD", "GET", "SET"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSCopyDataProperties",
    "args": ["ExclusionObj", "SourceObj", "TargetObj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "RegExp",
    "args": [],
    "flags": {
      "string": ["EXP", "FLAGS"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "UNOPDelMemberExpr",
    "args": ["Receiver", "Field"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "UNOPDelVar",
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSTemplate",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSBigInt",
    "args": [],
    "flags": {
      "string": ["IridiumPrimitive"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "Await",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "Yield",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSInitialYield",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "IDOP",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": ["PREFIX", "INCREMENT"],
      "double": [],
    }
  },
  {
    "tag": "JSIDOP",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": ["PREFIX", "INCREMENT"],
      "double": [],
    }
  },
  {
    "tag": "StackToHeap",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "LoopInitPreludeEnd",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSSetHome",
    "args": ["HomeObj", "FuncObj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSSetName",
    "args": ["obj", "name"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSSetPrototypeOf",
    "args": ["TargetObj", "ProtoValue"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "GWrite",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["INIT", "SAFE", "DECLVAR", "DECLFUN"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "LWrite",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["INIT", "SAFE", "THISINIT"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "RWrite",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["INIT", "SAFE", "THISINIT"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "MWrite",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["INIT", "SAFE"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "DCTRRet",
    "args": ["userObj", "thisObj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "ToNumeric",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSCTX",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["OPID"],
    }
  },
  {
    "tag": "QJSModuleInit",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "CompoundAssn",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
];
