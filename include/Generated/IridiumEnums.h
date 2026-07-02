// Generated: 2026-07-02 15:27:11
#pragma once
#include <string>

namespace IRI_GEN {
  enum IRI_TAG {
    File,
    ResolveEnvBinding,
    List,
    JSImplicitBindingDeclaration,
    EnvRead,
    TDZRead,
    String,
    FieldRead,
    JSExplicitBindingDeclaration,
    JSExplicitBindingDeclarationN,
    JSExplicitBindingDeclarationX,
    CallSite,
    Apply,
    ReturnAsync,
    BB,
    Return,
    UnresolvedReturn,
    IfJump,
    IfElseJump,
    Goto,
    JSFuncDecl,
    Lambda,
    NOP,
    BBContainer,
    Bindings,
    StarExport,
    StaticImport,
    LocalStaticExport,
    NamedReexport,
    ModuleRequest,
    EnvBinding,
    RemoteEnvBinding,
    GlobalBinding,
    ScriptBinding,
    EnvWrite,
    SiblingSpecialWrite,
    JSNUBD,
    Number,
    JSClass,
    JSCheckConstructor,
    ResolvePrivateEnvBinding,
    PVTEnvRead,
    JSPrivate,
    JSPrivateFieldWrite,
    JSADDBRAND,
    JSPrivateFieldRead,
    PoolBinding,
    ResolveContinueTarget,
    ResolveBreakTarget,
    JSForOfIteratorClose,
    PopCatchContext,
    PopFinalizerReturnTarget,
    InvokeFinalizer,
    JSForInStart,
    JSForInNext,
    JSForOfStart,
    JSForOfNext,
    PushCatchContext,
    JSCatchContext,
    Throw,
    Ret,
    JSBinop,
    FieldWrite,
    JSUnop,
    Unop,
    JSObject,
    Boolean,
    JSDefineObjProp,
    JSArray,
    Binop,
    Null,
    JSComputedFieldRead,
    JSComputedFieldWrite,
    JSSuperFieldRead,
    JSSuperFieldWrite,
    JSToObject,
    JSAppend,
    JSDefineObjMethod,
    JSCopyDataProperties,
    RegExp,
    UNOPDelMemberExpr,
    UNOPDelVar,
    JSTemplate,
    BitInt,
    Await,
    Yield,
    JSInitialYield,
    IDOP,
    JSIDOP,
    StackToHeap,
    LoopInitPreludeEnd,
    JSSetHome,
    JSSetName,
    JSSetPrototypeOf,
    GWrite,
    LWrite,
    RWrite,
    MWrite,
    DCTRRet,
    NIPCatchCTX,
    ToNumeric,
    JSCTX,
    QJSModuleInit,
    CompoundAssn
  };

  inline std::string dump_tag(IRI_TAG value) {
    switch(value) {
      case File: return "File";
      case ResolveEnvBinding: return "ResolveEnvBinding";
      case List: return "List";
      case JSImplicitBindingDeclaration: return "JSImplicitBindingDeclaration";
      case EnvRead: return "EnvRead";
      case TDZRead: return "TDZRead";
      case String: return "String";
      case FieldRead: return "FieldRead";
      case JSExplicitBindingDeclaration: return "JSExplicitBindingDeclaration";
      case JSExplicitBindingDeclarationN: return "JSExplicitBindingDeclarationN";
      case JSExplicitBindingDeclarationX: return "JSExplicitBindingDeclarationX";
      case CallSite: return "CallSite";
      case Apply: return "Apply";
      case ReturnAsync: return "ReturnAsync";
      case BB: return "BB";
      case Return: return "Return";
      case UnresolvedReturn: return "UnresolvedReturn";
      case IfJump: return "IfJump";
      case IfElseJump: return "IfElseJump";
      case Goto: return "Goto";
      case JSFuncDecl: return "JSFuncDecl";
      case Lambda: return "Lambda";
      case NOP: return "NOP";
      case BBContainer: return "BBContainer";
      case Bindings: return "Bindings";
      case StarExport: return "StarExport";
      case StaticImport: return "StaticImport";
      case LocalStaticExport: return "LocalStaticExport";
      case NamedReexport: return "NamedReexport";
      case ModuleRequest: return "ModuleRequest";
      case EnvBinding: return "EnvBinding";
      case RemoteEnvBinding: return "RemoteEnvBinding";
      case GlobalBinding: return "GlobalBinding";
      case ScriptBinding: return "ScriptBinding";
      case EnvWrite: return "EnvWrite";
      case SiblingSpecialWrite: return "SiblingSpecialWrite";
      case JSNUBD: return "JSNUBD";
      case Number: return "Number";
      case JSClass: return "JSClass";
      case JSCheckConstructor: return "JSCheckConstructor";
      case ResolvePrivateEnvBinding: return "ResolvePrivateEnvBinding";
      case PVTEnvRead: return "PVTEnvRead";
      case JSPrivate: return "JSPrivate";
      case JSPrivateFieldWrite: return "JSPrivateFieldWrite";
      case JSADDBRAND: return "JSADDBRAND";
      case JSPrivateFieldRead: return "JSPrivateFieldRead";
      case PoolBinding: return "PoolBinding";
      case ResolveContinueTarget: return "ResolveContinueTarget";
      case ResolveBreakTarget: return "ResolveBreakTarget";
      case JSForOfIteratorClose: return "JSForOfIteratorClose";
      case PopCatchContext: return "PopCatchContext";
      case PopFinalizerReturnTarget: return "PopFinalizerReturnTarget";
      case InvokeFinalizer: return "InvokeFinalizer";
      case JSForInStart: return "JSForInStart";
      case JSForInNext: return "JSForInNext";
      case JSForOfStart: return "JSForOfStart";
      case JSForOfNext: return "JSForOfNext";
      case PushCatchContext: return "PushCatchContext";
      case JSCatchContext: return "JSCatchContext";
      case Throw: return "Throw";
      case Ret: return "Ret";
      case JSBinop: return "JSBinop";
      case FieldWrite: return "FieldWrite";
      case JSUnop: return "JSUnop";
      case Unop: return "Unop";
      case JSObject: return "JSObject";
      case Boolean: return "Boolean";
      case JSDefineObjProp: return "JSDefineObjProp";
      case JSArray: return "JSArray";
      case Binop: return "Binop";
      case Null: return "Null";
      case JSComputedFieldRead: return "JSComputedFieldRead";
      case JSComputedFieldWrite: return "JSComputedFieldWrite";
      case JSSuperFieldRead: return "JSSuperFieldRead";
      case JSSuperFieldWrite: return "JSSuperFieldWrite";
      case JSToObject: return "JSToObject";
      case JSAppend: return "JSAppend";
      case JSDefineObjMethod: return "JSDefineObjMethod";
      case JSCopyDataProperties: return "JSCopyDataProperties";
      case RegExp: return "RegExp";
      case UNOPDelMemberExpr: return "UNOPDelMemberExpr";
      case UNOPDelVar: return "UNOPDelVar";
      case JSTemplate: return "JSTemplate";
      case BitInt: return "BitInt";
      case Await: return "Await";
      case Yield: return "Yield";
      case JSInitialYield: return "JSInitialYield";
      case IDOP: return "IDOP";
      case JSIDOP: return "JSIDOP";
      case StackToHeap: return "StackToHeap";
      case LoopInitPreludeEnd: return "LoopInitPreludeEnd";
      case JSSetHome: return "JSSetHome";
      case JSSetName: return "JSSetName";
      case JSSetPrototypeOf: return "JSSetPrototypeOf";
      case GWrite: return "GWrite";
      case LWrite: return "LWrite";
      case RWrite: return "RWrite";
      case MWrite: return "MWrite";
      case DCTRRet: return "DCTRRet";
      case NIPCatchCTX: return "NIPCatchCTX";
      case ToNumeric: return "ToNumeric";
      case JSCTX: return "JSCTX";
      case QJSModuleInit: return "QJSModuleInit";
      case CompoundAssn: return "CompoundAssn";
      default: return "unknown_tag";
    }
  }

  enum IRI_FLAG {
    JSScript,
    JSModule,
    TLA,
    NAME,
    ASW,
    TYPE,
    JSLET,
    JSCONST,
    JSVAR,
    SLOPPY,
    SKIPINIT,
    SAFE,
    THISINIT,
    OPID,
    TAINTED,
    IridiumPrimitive,
    CCall,
    ConstructorCall,
    PrivateCall,
    Import,
    Super,
    V8Intrinsic,
    TAILCALL,
    JSDirectEval,
    TopLevel,
    ClosureBoundary,
    Lexical,
    VARBoundary,
    TryBB,
    IDX,
    ScopeIDX,
    NOT,
    TRUE,
    FALSE,
    CNAME,
    SETNAME,
    StartBBIDX,
    ARGUMENTS,
    ASYNC,
    STRICT,
    GENERATOR,
    PROTO,
    NEW,
    SCALL,
    SOBJ,
    HOME,
    DERIVED,
    ECMAArgs,
    ContainerFlagID,
    MODULEREQIDX,
    FIELD,
    NSIMPORT,
    LOCALNAME,
    EXPORTNAME,
    SOURCE,
    REQIDX,
    JSARG,
    JSRESTARG,
    REFIDX,
    SCOPE,
    NEXT,
    LINK,
    MODULE,
    MODULEI,
    MODULENSI,
    FULLY_RESOLVE,
    SYMBOL,
    METHOD,
    DECL,
    Label,
    AWAIT,
    OP,
    NOENUM,
    GET,
    SET,
    EXP,
    FLAGS,
    PREFIX,
    INCREMENT,
    INIT,
    DECLVAR,
    DECLFUN
  };

  inline std::string dump_flag(IRI_FLAG value) {
    switch(value) {
      case JSScript: return "JSScript";
      case JSModule: return "JSModule";
      case TLA: return "TLA";
      case NAME: return "NAME";
      case ASW: return "ASW";
      case TYPE: return "TYPE";
      case JSLET: return "JSLET";
      case JSCONST: return "JSCONST";
      case JSVAR: return "JSVAR";
      case SLOPPY: return "SLOPPY";
      case SKIPINIT: return "SKIPINIT";
      case SAFE: return "SAFE";
      case THISINIT: return "THISINIT";
      case OPID: return "OPID";
      case TAINTED: return "TAINTED";
      case IridiumPrimitive: return "IridiumPrimitive";
      case CCall: return "CCall";
      case ConstructorCall: return "ConstructorCall";
      case PrivateCall: return "PrivateCall";
      case Import: return "Import";
      case Super: return "Super";
      case V8Intrinsic: return "V8Intrinsic";
      case TAILCALL: return "TAILCALL";
      case JSDirectEval: return "JSDirectEval";
      case TopLevel: return "TopLevel";
      case ClosureBoundary: return "ClosureBoundary";
      case Lexical: return "Lexical";
      case VARBoundary: return "VARBoundary";
      case TryBB: return "TryBB";
      case IDX: return "IDX";
      case ScopeIDX: return "ScopeIDX";
      case NOT: return "NOT";
      case TRUE: return "TRUE";
      case FALSE: return "FALSE";
      case CNAME: return "CNAME";
      case SETNAME: return "SETNAME";
      case StartBBIDX: return "StartBBIDX";
      case ARGUMENTS: return "ARGUMENTS";
      case ASYNC: return "ASYNC";
      case STRICT: return "STRICT";
      case GENERATOR: return "GENERATOR";
      case PROTO: return "PROTO";
      case NEW: return "NEW";
      case SCALL: return "SCALL";
      case SOBJ: return "SOBJ";
      case HOME: return "HOME";
      case DERIVED: return "DERIVED";
      case ECMAArgs: return "ECMAArgs";
      case ContainerFlagID: return "ContainerFlagID";
      case MODULEREQIDX: return "MODULEREQIDX";
      case FIELD: return "FIELD";
      case NSIMPORT: return "NSIMPORT";
      case LOCALNAME: return "LOCALNAME";
      case EXPORTNAME: return "EXPORTNAME";
      case SOURCE: return "SOURCE";
      case REQIDX: return "REQIDX";
      case JSARG: return "JSARG";
      case JSRESTARG: return "JSRESTARG";
      case REFIDX: return "REFIDX";
      case SCOPE: return "SCOPE";
      case NEXT: return "NEXT";
      case LINK: return "LINK";
      case MODULE: return "MODULE";
      case MODULEI: return "MODULEI";
      case MODULENSI: return "MODULENSI";
      case FULLY_RESOLVE: return "FULLY_RESOLVE";
      case SYMBOL: return "SYMBOL";
      case METHOD: return "METHOD";
      case DECL: return "DECL";
      case Label: return "Label";
      case AWAIT: return "AWAIT";
      case OP: return "OP";
      case NOENUM: return "NOENUM";
      case GET: return "GET";
      case SET: return "SET";
      case EXP: return "EXP";
      case FLAGS: return "FLAGS";
      case PREFIX: return "PREFIX";
      case INCREMENT: return "INCREMENT";
      case INIT: return "INIT";
      case DECLVAR: return "DECLVAR";
      case DECLFUN: return "DECLFUN";
      default: return "unknown_flag";
    }
  }
} // namespace IRI_GEN