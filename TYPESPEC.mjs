export default [
  {
    "tag": "File",
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
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": ["SAFE"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "TDZRead",
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
    "args": [],
    "flags": {
      "string": [],
      "void": ["TopLevel", "ClosureBoundary", "Lexical", "VARBoundary"],
      "bool": [],
      "double": ["IDX", "ScopeIDX"],
    }
  },
  {
    "tag": "Return",
    "args": ["Obj"],
    "flags": {
      "string": [],
      "void": ["ModuleEarlyReturn"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "IfJump",
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
    "args": ["LocalBindings", "RemoteBindings", "Lambdas"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["ParentScope"],
    }
  },
  {
    "tag": "StarExport",
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
    "args": ["StorageLocation"],
    "flags": {
      "string": ["FIELD"],
      "void": [],
      "bool": [],
      "double": ["MODULEREQIDX"],
    }
  },
  {
    "tag": "LocalStaticExport",
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
    "args": [],
    "flags": {
      "string": ["EXPORTNAME"],
      "void": [],
      "bool": [],
      "double": ["MODULEREQIDX"],
    }
  },
  {
    "tag": "ModuleRequest",
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
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": ["ASW", "JSARG", "JSRESTARG", "JSLET", "JSCONST", "JSVAR"],
      "bool": [],
      "double": ["IDX", "REFIDX", "Scope", "ParentScope", "NEXT"],
    }
  },
  {
    "tag": "RemoteEnvBinding",
    "args": ["ParentReference"],
    "flags": {
      "string": [],
      "void": ["NSIMPORT"],
      "bool": [],
      "double": ["REFIDX"],
    }
  },
  {
    "tag": "GlobalBinding",
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "EnvWrite",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["SLOPPY", "THROWERR"],
      "bool": ["SAFE", "THISINIT"],
      "double": [],
    }
  },
  {
    "tag": "SiblingSpecialWrite",
    "args": ["LValTarget", "RVal"],
    "flags": {
      "string": [],
      "void": ["SLOPPY", "THROWERR"],
      "bool": ["SAFE", "THISINIT"],
      "double": ["ScopeIDX"],
    }
  },
  {
    "tag": "JSNUBD",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSSloppyDecl",
    "args": [],
    "flags": {
      "string": ["NAME"],
      "void": ["JSLET", "JSCONST", "JSVAR"],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "Number",
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
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "StackReject",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["NVAL"],
    }
  },
  {
    "tag": "ResolvePrivateEnvBinding",
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
    "args": ["Lambda"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["StartBBIDX", "REFIDX"],
    }
  },
  {
    "tag": "ResolveContinueTarget",
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
    "tag": "StackRetain",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": ["NVAL", "NIP"],
    }
  },
  {
    "tag": "StackPop",
    "args": [],
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
    "args": [],
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
      "void": ["NOENUM"],
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
    "tag": "BitInt",
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
    "args": ["Obj", "DoneTarget", "NextValue"],
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
  }
];