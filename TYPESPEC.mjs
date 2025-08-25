export default [
  {
    "tag": "File",
    "args": [],
    "flags": {
      "string": [],
      "void": ["JSScript", "JSModule"],
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
      "void": [],
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
    "tag": "CallSite",
    "args": [],
    "flags": {
      "string": [],
      "void": ["CCall", "ConstructorCall", "PrivateCall", "Import", "Super", "JSDirectEval"],
      "bool": [],
      "double": [],
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
      "void": ["TopLevel", "ClosureBoundary", "Lexical"],
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
    "tag": "IfElseJump",
    "args": ["Test"],
    "flags": {
      "string": [],
      "void": [],
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
      "string": [],
      "void": [],
      "bool": [],
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
      "string": [],
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
      "void": ["SLOPPY"],
      "bool": ["SAFE", "THISINIT"],
      "double": [],
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
    "args": ["NAME", "Parent", "Constructor", "PropInit", "MethodList", "StaticMethodList", "StaticPropInit"],
    "flags": {
      "string": [],
      "void": ["Derived", "BrandPrototype", "BrandConstructor"],
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
      "bool": [],
      "double": [],
    }
  },
  {
    "tag": "JSForOfNext",
    "args": [],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
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
    "tag": "ReturnAsync",
    "args": ["RetVal"],
    "flags": {
      "string": [],
      "void": [],
      "bool": [],
      "double": [],
    }
  },
];