// Generated: 2026-07-10 16:00:55
#pragma once
#include "IridiumEnums.h"
#include <cstdint>
#include <stdexcept>

namespace IRI_GEN {
class IridiumMeta {
public:
  // Used by the Iridium Pool to know how many flag slots to reserve
  static uint32_t get_flag_slots(IRI_GEN::IRI_TAG tag) {
    switch(tag) {
      case IRI_GEN::File: return 3;
      case IRI_GEN::ResolveEnvBinding: return 2;
      case IRI_GEN::List: return 1;
      case IRI_GEN::JSImplicitBindingDeclaration: return 9;
      case IRI_GEN::EnvRead: return 2;
      case IRI_GEN::TDZRead: return 1;
      case IRI_GEN::String: return 1;
      case IRI_GEN::FieldRead: return 0;
      case IRI_GEN::JSExplicitBindingDeclaration: return 6;
      case IRI_GEN::JSExplicitBindingDeclarationN: return 6;
      case IRI_GEN::JSExplicitBindingDeclarationX: return 6;
      case IRI_GEN::CallSite: return 8;
      case IRI_GEN::Apply: return 3;
      case IRI_GEN::ReturnAsync: return 0;
      case IRI_GEN::BB: return 7;
      case IRI_GEN::Return: return 0;
      case IRI_GEN::UnresolvedReturn: return 0;
      case IRI_GEN::IfJump: return 2;
      case IRI_GEN::IfElseJump: return 3;
      case IRI_GEN::Goto: return 1;
      case IRI_GEN::JSFuncDecl: return 6;
      case IRI_GEN::Lambda: return 4;
      case IRI_GEN::NOP: return 0;
      case IRI_GEN::BBContainer: return 16;
      case IRI_GEN::Bindings: return 0;
      case IRI_GEN::StarExport: return 1;
      case IRI_GEN::StaticImport: return 3;
      case IRI_GEN::LocalStaticExport: return 2;
      case IRI_GEN::NamedReexport: return 3;
      case IRI_GEN::ModuleRequest: return 2;
      case IRI_GEN::EnvBinding: return 10;
      case IRI_GEN::RemoteEnvBinding: return 5;
      case IRI_GEN::GlobalBinding: return 2;
      case IRI_GEN::ScriptBinding: return 5;
      case IRI_GEN::EnvWrite: return 4;
      case IRI_GEN::SiblingSpecialWrite: return 5;
      case IRI_GEN::JSNUBD: return 0;
      case IRI_GEN::Number: return 1;
      case IRI_GEN::JSClass: return 2;
      case IRI_GEN::JSCheckConstructor: return 0;
      case IRI_GEN::ResolvePrivateEnvBinding: return 2;
      case IRI_GEN::PVTEnvRead: return 3;
      case IRI_GEN::JSPrivate: return 1;
      case IRI_GEN::JSPrivateFieldWrite: return 1;
      case IRI_GEN::JSADDBRAND: return 0;
      case IRI_GEN::JSPrivateFieldRead: return 0;
      case IRI_GEN::PoolBinding: return 1;
      case IRI_GEN::ResolveContinueTarget: return 1;
      case IRI_GEN::ResolveBreakTarget: return 1;
      case IRI_GEN::JSForOfIteratorClose: return 0;
      case IRI_GEN::PopCatchContext: return 0;
      case IRI_GEN::PopFinalizerReturnTarget: return 0;
      case IRI_GEN::InvokeFinalizer: return 1;
      case IRI_GEN::JSForInStart: return 0;
      case IRI_GEN::JSForInNext: return 0;
      case IRI_GEN::JSForOfStart: return 1;
      case IRI_GEN::JSForOfNext: return 1;
      case IRI_GEN::PushCatchContext: return 1;
      case IRI_GEN::JSCatchContext: return 1;
      case IRI_GEN::Throw: return 0;
      case IRI_GEN::Ret: return 0;
      case IRI_GEN::JSBinop: return 1;
      case IRI_GEN::FieldWrite: return 0;
      case IRI_GEN::JSUnop: return 1;
      case IRI_GEN::Unop: return 1;
      case IRI_GEN::JSObject: return 0;
      case IRI_GEN::Boolean: return 1;
      case IRI_GEN::JSDefineObjProp: return 0;
      case IRI_GEN::JSArray: return 0;
      case IRI_GEN::Binop: return 1;
      case IRI_GEN::Null: return 1;
      case IRI_GEN::JSComputedFieldRead: return 1;
      case IRI_GEN::JSComputedFieldWrite: return 1;
      case IRI_GEN::JSSuperFieldRead: return 0;
      case IRI_GEN::JSSuperFieldWrite: return 0;
      case IRI_GEN::JSToObject: return 0;
      case IRI_GEN::JSAppend: return 0;
      case IRI_GEN::JSDefineObjMethod: return 4;
      case IRI_GEN::JSCopyDataProperties: return 0;
      case IRI_GEN::RegExp: return 2;
      case IRI_GEN::UNOPDelMemberExpr: return 0;
      case IRI_GEN::UNOPDelVar: return 1;
      case IRI_GEN::JSTemplate: return 0;
      case IRI_GEN::JSBigInt: return 1;
      case IRI_GEN::Await: return 0;
      case IRI_GEN::Yield: return 0;
      case IRI_GEN::JSInitialYield: return 0;
      case IRI_GEN::IDOP: return 2;
      case IRI_GEN::JSIDOP: return 2;
      case IRI_GEN::StackToHeap: return 0;
      case IRI_GEN::LoopInitPreludeEnd: return 0;
      case IRI_GEN::JSSetHome: return 0;
      case IRI_GEN::JSSetName: return 0;
      case IRI_GEN::JSSetPrototypeOf: return 0;
      case IRI_GEN::GWrite: return 4;
      case IRI_GEN::LWrite: return 3;
      case IRI_GEN::RWrite: return 3;
      case IRI_GEN::MWrite: return 2;
      case IRI_GEN::DCTRRet: return 0;
      case IRI_GEN::ToNumeric: return 0;
      case IRI_GEN::JSCTX: return 1;
      case IRI_GEN::QJSModuleInit: return 0;
      case IRI_GEN::CompoundAssn: return 0;
      default: return 0;
    }
  }

  // Used by the Parser to map an incoming msgpack flag enum to its static slot index
  static int get_flag_index(IRI_GEN::IRI_TAG tag, IRI_GEN::IRI_FLAG flag) {
    switch(tag) {
      case IRI_GEN::File:
        switch(flag) {
        case IRI_GEN::JSScript: return 0;
        case IRI_GEN::JSModule: return 1;
        case IRI_GEN::TLA: return 2;
          default: return -1;
        }
        break;
      case IRI_GEN::ResolveEnvBinding:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
        case IRI_GEN::ASW: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::List:
        switch(flag) {
        case IRI_GEN::TYPE: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSImplicitBindingDeclaration:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
        case IRI_GEN::JSLET: return 1;
        case IRI_GEN::JSCONST: return 2;
        case IRI_GEN::JSVAR: return 3;
        case IRI_GEN::SLOPPY: return 4;
        case IRI_GEN::SKIPINIT: return 5;
        case IRI_GEN::SAFE: return 6;
        case IRI_GEN::THISINIT: return 7;
        case IRI_GEN::OPID: return 8;
          default: return -1;
        }
        break;
      case IRI_GEN::EnvRead:
        switch(flag) {
        case IRI_GEN::SAFE: return 0;
        case IRI_GEN::TAINTED: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::TDZRead:
        switch(flag) {
        case IRI_GEN::SAFE: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::String:
        switch(flag) {
        case IRI_GEN::IridiumPrimitive: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSExplicitBindingDeclaration:
        switch(flag) {
        case IRI_GEN::JSLET: return 0;
        case IRI_GEN::JSCONST: return 1;
        case IRI_GEN::JSVAR: return 2;
        case IRI_GEN::SLOPPY: return 3;
        case IRI_GEN::SAFE: return 4;
        case IRI_GEN::THISINIT: return 5;
          default: return -1;
        }
        break;
      case IRI_GEN::JSExplicitBindingDeclarationN:
        switch(flag) {
        case IRI_GEN::JSLET: return 0;
        case IRI_GEN::JSCONST: return 1;
        case IRI_GEN::JSVAR: return 2;
        case IRI_GEN::SLOPPY: return 3;
        case IRI_GEN::SAFE: return 4;
        case IRI_GEN::THISINIT: return 5;
          default: return -1;
        }
        break;
      case IRI_GEN::JSExplicitBindingDeclarationX:
        switch(flag) {
        case IRI_GEN::JSLET: return 0;
        case IRI_GEN::JSCONST: return 1;
        case IRI_GEN::JSVAR: return 2;
        case IRI_GEN::SLOPPY: return 3;
        case IRI_GEN::SAFE: return 4;
        case IRI_GEN::THISINIT: return 5;
          default: return -1;
        }
        break;
      case IRI_GEN::CallSite:
        switch(flag) {
        case IRI_GEN::CCall: return 0;
        case IRI_GEN::ConstructorCall: return 1;
        case IRI_GEN::PrivateCall: return 2;
        case IRI_GEN::Import: return 3;
        case IRI_GEN::Super: return 4;
        case IRI_GEN::V8Intrinsic: return 5;
        case IRI_GEN::TAILCALL: return 6;
        case IRI_GEN::JSDirectEval: return 7;
          default: return -1;
        }
        break;
      case IRI_GEN::Apply:
        switch(flag) {
        case IRI_GEN::ConstructorCall: return 0;
        case IRI_GEN::Super: return 1;
        case IRI_GEN::JSDirectEval: return 2;
          default: return -1;
        }
        break;
      case IRI_GEN::BB:
        switch(flag) {
        case IRI_GEN::TopLevel: return 0;
        case IRI_GEN::ClosureBoundary: return 1;
        case IRI_GEN::Lexical: return 2;
        case IRI_GEN::VARBoundary: return 3;
        case IRI_GEN::TryBB: return 4;
        case IRI_GEN::IDX: return 5;
        case IRI_GEN::ScopeIDX: return 6;
          default: return -1;
        }
        break;
      case IRI_GEN::IfJump:
        switch(flag) {
        case IRI_GEN::NOT: return 0;
        case IRI_GEN::IDX: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::IfElseJump:
        switch(flag) {
        case IRI_GEN::NOT: return 0;
        case IRI_GEN::TRUE: return 1;
        case IRI_GEN::FALSE: return 2;
          default: return -1;
        }
        break;
      case IRI_GEN::Goto:
        switch(flag) {
        case IRI_GEN::IDX: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSFuncDecl:
        switch(flag) {
        case IRI_GEN::JSLET: return 0;
        case IRI_GEN::JSCONST: return 1;
        case IRI_GEN::JSVAR: return 2;
        case IRI_GEN::SLOPPY: return 3;
        case IRI_GEN::SAFE: return 4;
        case IRI_GEN::THISINIT: return 5;
          default: return -1;
        }
        break;
      case IRI_GEN::Lambda:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
        case IRI_GEN::CNAME: return 1;
        case IRI_GEN::SETNAME: return 2;
        case IRI_GEN::StartBBIDX: return 3;
          default: return -1;
        }
        break;
      case IRI_GEN::BBContainer:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
        case IRI_GEN::ARGUMENTS: return 1;
        case IRI_GEN::ASYNC: return 2;
        case IRI_GEN::STRICT: return 3;
        case IRI_GEN::GENERATOR: return 4;
        case IRI_GEN::PROTO: return 5;
        case IRI_GEN::NEW: return 6;
        case IRI_GEN::SCALL: return 7;
        case IRI_GEN::SOBJ: return 8;
        case IRI_GEN::HOME: return 9;
        case IRI_GEN::DERIVED: return 10;
        case IRI_GEN::TopLevel: return 11;
        case IRI_GEN::ECMAArgs: return 12;
        case IRI_GEN::StartBBIDX: return 13;
        case IRI_GEN::ScopeIDX: return 14;
        case IRI_GEN::ContainerFlagID: return 15;
          default: return -1;
        }
        break;
      case IRI_GEN::StarExport:
        switch(flag) {
        case IRI_GEN::MODULEREQIDX: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::StaticImport:
        switch(flag) {
        case IRI_GEN::FIELD: return 0;
        case IRI_GEN::NSIMPORT: return 1;
        case IRI_GEN::MODULEREQIDX: return 2;
          default: return -1;
        }
        break;
      case IRI_GEN::LocalStaticExport:
        switch(flag) {
        case IRI_GEN::LOCALNAME: return 0;
        case IRI_GEN::EXPORTNAME: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::NamedReexport:
        switch(flag) {
        case IRI_GEN::LOCALNAME: return 0;
        case IRI_GEN::EXPORTNAME: return 1;
        case IRI_GEN::MODULEREQIDX: return 2;
          default: return -1;
        }
        break;
      case IRI_GEN::ModuleRequest:
        switch(flag) {
        case IRI_GEN::SOURCE: return 0;
        case IRI_GEN::REQIDX: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::EnvBinding:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
        case IRI_GEN::JSARG: return 1;
        case IRI_GEN::JSRESTARG: return 2;
        case IRI_GEN::JSLET: return 3;
        case IRI_GEN::JSCONST: return 4;
        case IRI_GEN::JSVAR: return 5;
        case IRI_GEN::REFIDX: return 6;
        case IRI_GEN::SCOPE: return 7;
        case IRI_GEN::NEXT: return 8;
        case IRI_GEN::LINK: return 9;
          default: return -1;
        }
        break;
      case IRI_GEN::RemoteEnvBinding:
        switch(flag) {
        case IRI_GEN::MODULE: return 0;
        case IRI_GEN::MODULEI: return 1;
        case IRI_GEN::MODULENSI: return 2;
        case IRI_GEN::REFIDX: return 3;
        case IRI_GEN::LINK: return 4;
          default: return -1;
        }
        break;
      case IRI_GEN::GlobalBinding:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
        case IRI_GEN::LINK: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::ScriptBinding:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
        case IRI_GEN::JSLET: return 1;
        case IRI_GEN::JSCONST: return 2;
        case IRI_GEN::JSVAR: return 3;
        case IRI_GEN::LINK: return 4;
          default: return -1;
        }
        break;
      case IRI_GEN::EnvWrite:
        switch(flag) {
        case IRI_GEN::SLOPPY: return 0;
        case IRI_GEN::SAFE: return 1;
        case IRI_GEN::THISINIT: return 2;
        case IRI_GEN::CINIT: return 3;
          default: return -1;
        }
        break;
      case IRI_GEN::SiblingSpecialWrite:
        switch(flag) {
        case IRI_GEN::SLOPPY: return 0;
        case IRI_GEN::SAFE: return 1;
        case IRI_GEN::THISINIT: return 2;
        case IRI_GEN::CINIT: return 3;
        case IRI_GEN::ScopeIDX: return 4;
          default: return -1;
        }
        break;
      case IRI_GEN::Number:
        switch(flag) {
        case IRI_GEN::IridiumPrimitive: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSClass:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
        case IRI_GEN::DERIVED: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::ResolvePrivateEnvBinding:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
        case IRI_GEN::FULLY_RESOLVE: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::PVTEnvRead:
        switch(flag) {
        case IRI_GEN::SYMBOL: return 0;
        case IRI_GEN::METHOD: return 1;
        case IRI_GEN::FULLY_RESOLVE: return 2;
          default: return -1;
        }
        break;
      case IRI_GEN::JSPrivate:
        switch(flag) {
        case IRI_GEN::IridiumPrimitive: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSPrivateFieldWrite:
        switch(flag) {
        case IRI_GEN::DECL: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::PoolBinding:
        switch(flag) {
        case IRI_GEN::REFIDX: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::ResolveContinueTarget:
        switch(flag) {
        case IRI_GEN::Label: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::ResolveBreakTarget:
        switch(flag) {
        case IRI_GEN::Label: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::InvokeFinalizer:
        switch(flag) {
        case IRI_GEN::IDX: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSForOfStart:
        switch(flag) {
        case IRI_GEN::AWAIT: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSForOfNext:
        switch(flag) {
        case IRI_GEN::AWAIT: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::PushCatchContext:
        switch(flag) {
        case IRI_GEN::IDX: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSCatchContext:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSBinop:
        switch(flag) {
        case IRI_GEN::OP: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSUnop:
        switch(flag) {
        case IRI_GEN::OP: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::Unop:
        switch(flag) {
        case IRI_GEN::OP: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::Boolean:
        switch(flag) {
        case IRI_GEN::IridiumPrimitive: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::Binop:
        switch(flag) {
        case IRI_GEN::OP: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::Null:
        switch(flag) {
        case IRI_GEN::IridiumPrimitive: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSComputedFieldRead:
        switch(flag) {
        case IRI_GEN::SAFE: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSComputedFieldWrite:
        switch(flag) {
        case IRI_GEN::SAFE: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSDefineObjMethod:
        switch(flag) {
        case IRI_GEN::NOENUM: return 0;
        case IRI_GEN::METHOD: return 1;
        case IRI_GEN::GET: return 2;
        case IRI_GEN::SET: return 3;
          default: return -1;
        }
        break;
      case IRI_GEN::RegExp:
        switch(flag) {
        case IRI_GEN::EXP: return 0;
        case IRI_GEN::FLAGS: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::UNOPDelVar:
        switch(flag) {
        case IRI_GEN::NAME: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::JSBigInt:
        switch(flag) {
        case IRI_GEN::IridiumPrimitive: return 0;
          default: return -1;
        }
        break;
      case IRI_GEN::IDOP:
        switch(flag) {
        case IRI_GEN::PREFIX: return 0;
        case IRI_GEN::INCREMENT: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::JSIDOP:
        switch(flag) {
        case IRI_GEN::PREFIX: return 0;
        case IRI_GEN::INCREMENT: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::GWrite:
        switch(flag) {
        case IRI_GEN::INIT: return 0;
        case IRI_GEN::SAFE: return 1;
        case IRI_GEN::DECLVAR: return 2;
        case IRI_GEN::DECLFUN: return 3;
          default: return -1;
        }
        break;
      case IRI_GEN::LWrite:
        switch(flag) {
        case IRI_GEN::INIT: return 0;
        case IRI_GEN::SAFE: return 1;
        case IRI_GEN::THISINIT: return 2;
          default: return -1;
        }
        break;
      case IRI_GEN::RWrite:
        switch(flag) {
        case IRI_GEN::INIT: return 0;
        case IRI_GEN::SAFE: return 1;
        case IRI_GEN::THISINIT: return 2;
          default: return -1;
        }
        break;
      case IRI_GEN::MWrite:
        switch(flag) {
        case IRI_GEN::INIT: return 0;
        case IRI_GEN::SAFE: return 1;
          default: return -1;
        }
        break;
      case IRI_GEN::JSCTX:
        switch(flag) {
        case IRI_GEN::OPID: return 0;
          default: return -1;
        }
        break;
      default: return -1;
    }
  }

  // Used by IridiumSEXP::dump() to recover the flag name
  static IRI_GEN::IRI_FLAG get_flag_enum(IRI_GEN::IRI_TAG tag, uint32_t index) {
    switch(tag) {
      case IRI_GEN::File:
        switch(index) {
        case 0: return IRI_GEN::JSScript;
        case 1: return IRI_GEN::JSModule;
        case 2: return IRI_GEN::TLA;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::ResolveEnvBinding:
        switch(index) {
        case 0: return IRI_GEN::NAME;
        case 1: return IRI_GEN::ASW;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::List:
        switch(index) {
        case 0: return IRI_GEN::TYPE;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSImplicitBindingDeclaration:
        switch(index) {
        case 0: return IRI_GEN::NAME;
        case 1: return IRI_GEN::JSLET;
        case 2: return IRI_GEN::JSCONST;
        case 3: return IRI_GEN::JSVAR;
        case 4: return IRI_GEN::SLOPPY;
        case 5: return IRI_GEN::SKIPINIT;
        case 6: return IRI_GEN::SAFE;
        case 7: return IRI_GEN::THISINIT;
        case 8: return IRI_GEN::OPID;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::EnvRead:
        switch(index) {
        case 0: return IRI_GEN::SAFE;
        case 1: return IRI_GEN::TAINTED;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::TDZRead:
        switch(index) {
        case 0: return IRI_GEN::SAFE;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::String:
        switch(index) {
        case 0: return IRI_GEN::IridiumPrimitive;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSExplicitBindingDeclaration:
        switch(index) {
        case 0: return IRI_GEN::JSLET;
        case 1: return IRI_GEN::JSCONST;
        case 2: return IRI_GEN::JSVAR;
        case 3: return IRI_GEN::SLOPPY;
        case 4: return IRI_GEN::SAFE;
        case 5: return IRI_GEN::THISINIT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSExplicitBindingDeclarationN:
        switch(index) {
        case 0: return IRI_GEN::JSLET;
        case 1: return IRI_GEN::JSCONST;
        case 2: return IRI_GEN::JSVAR;
        case 3: return IRI_GEN::SLOPPY;
        case 4: return IRI_GEN::SAFE;
        case 5: return IRI_GEN::THISINIT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSExplicitBindingDeclarationX:
        switch(index) {
        case 0: return IRI_GEN::JSLET;
        case 1: return IRI_GEN::JSCONST;
        case 2: return IRI_GEN::JSVAR;
        case 3: return IRI_GEN::SLOPPY;
        case 4: return IRI_GEN::SAFE;
        case 5: return IRI_GEN::THISINIT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::CallSite:
        switch(index) {
        case 0: return IRI_GEN::CCall;
        case 1: return IRI_GEN::ConstructorCall;
        case 2: return IRI_GEN::PrivateCall;
        case 3: return IRI_GEN::Import;
        case 4: return IRI_GEN::Super;
        case 5: return IRI_GEN::V8Intrinsic;
        case 6: return IRI_GEN::TAILCALL;
        case 7: return IRI_GEN::JSDirectEval;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::Apply:
        switch(index) {
        case 0: return IRI_GEN::ConstructorCall;
        case 1: return IRI_GEN::Super;
        case 2: return IRI_GEN::JSDirectEval;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::BB:
        switch(index) {
        case 0: return IRI_GEN::TopLevel;
        case 1: return IRI_GEN::ClosureBoundary;
        case 2: return IRI_GEN::Lexical;
        case 3: return IRI_GEN::VARBoundary;
        case 4: return IRI_GEN::TryBB;
        case 5: return IRI_GEN::IDX;
        case 6: return IRI_GEN::ScopeIDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::IfJump:
        switch(index) {
        case 0: return IRI_GEN::NOT;
        case 1: return IRI_GEN::IDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::IfElseJump:
        switch(index) {
        case 0: return IRI_GEN::NOT;
        case 1: return IRI_GEN::TRUE;
        case 2: return IRI_GEN::FALSE;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::Goto:
        switch(index) {
        case 0: return IRI_GEN::IDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSFuncDecl:
        switch(index) {
        case 0: return IRI_GEN::JSLET;
        case 1: return IRI_GEN::JSCONST;
        case 2: return IRI_GEN::JSVAR;
        case 3: return IRI_GEN::SLOPPY;
        case 4: return IRI_GEN::SAFE;
        case 5: return IRI_GEN::THISINIT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::Lambda:
        switch(index) {
        case 0: return IRI_GEN::NAME;
        case 1: return IRI_GEN::CNAME;
        case 2: return IRI_GEN::SETNAME;
        case 3: return IRI_GEN::StartBBIDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::BBContainer:
        switch(index) {
        case 0: return IRI_GEN::NAME;
        case 1: return IRI_GEN::ARGUMENTS;
        case 2: return IRI_GEN::ASYNC;
        case 3: return IRI_GEN::STRICT;
        case 4: return IRI_GEN::GENERATOR;
        case 5: return IRI_GEN::PROTO;
        case 6: return IRI_GEN::NEW;
        case 7: return IRI_GEN::SCALL;
        case 8: return IRI_GEN::SOBJ;
        case 9: return IRI_GEN::HOME;
        case 10: return IRI_GEN::DERIVED;
        case 11: return IRI_GEN::TopLevel;
        case 12: return IRI_GEN::ECMAArgs;
        case 13: return IRI_GEN::StartBBIDX;
        case 14: return IRI_GEN::ScopeIDX;
        case 15: return IRI_GEN::ContainerFlagID;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::StarExport:
        switch(index) {
        case 0: return IRI_GEN::MODULEREQIDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::StaticImport:
        switch(index) {
        case 0: return IRI_GEN::FIELD;
        case 1: return IRI_GEN::NSIMPORT;
        case 2: return IRI_GEN::MODULEREQIDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::LocalStaticExport:
        switch(index) {
        case 0: return IRI_GEN::LOCALNAME;
        case 1: return IRI_GEN::EXPORTNAME;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::NamedReexport:
        switch(index) {
        case 0: return IRI_GEN::LOCALNAME;
        case 1: return IRI_GEN::EXPORTNAME;
        case 2: return IRI_GEN::MODULEREQIDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::ModuleRequest:
        switch(index) {
        case 0: return IRI_GEN::SOURCE;
        case 1: return IRI_GEN::REQIDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::EnvBinding:
        switch(index) {
        case 0: return IRI_GEN::NAME;
        case 1: return IRI_GEN::JSARG;
        case 2: return IRI_GEN::JSRESTARG;
        case 3: return IRI_GEN::JSLET;
        case 4: return IRI_GEN::JSCONST;
        case 5: return IRI_GEN::JSVAR;
        case 6: return IRI_GEN::REFIDX;
        case 7: return IRI_GEN::SCOPE;
        case 8: return IRI_GEN::NEXT;
        case 9: return IRI_GEN::LINK;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::RemoteEnvBinding:
        switch(index) {
        case 0: return IRI_GEN::MODULE;
        case 1: return IRI_GEN::MODULEI;
        case 2: return IRI_GEN::MODULENSI;
        case 3: return IRI_GEN::REFIDX;
        case 4: return IRI_GEN::LINK;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::GlobalBinding:
        switch(index) {
        case 0: return IRI_GEN::NAME;
        case 1: return IRI_GEN::LINK;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::ScriptBinding:
        switch(index) {
        case 0: return IRI_GEN::NAME;
        case 1: return IRI_GEN::JSLET;
        case 2: return IRI_GEN::JSCONST;
        case 3: return IRI_GEN::JSVAR;
        case 4: return IRI_GEN::LINK;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::EnvWrite:
        switch(index) {
        case 0: return IRI_GEN::SLOPPY;
        case 1: return IRI_GEN::SAFE;
        case 2: return IRI_GEN::THISINIT;
        case 3: return IRI_GEN::CINIT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::SiblingSpecialWrite:
        switch(index) {
        case 0: return IRI_GEN::SLOPPY;
        case 1: return IRI_GEN::SAFE;
        case 2: return IRI_GEN::THISINIT;
        case 3: return IRI_GEN::CINIT;
        case 4: return IRI_GEN::ScopeIDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::Number:
        switch(index) {
        case 0: return IRI_GEN::IridiumPrimitive;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSClass:
        switch(index) {
        case 0: return IRI_GEN::NAME;
        case 1: return IRI_GEN::DERIVED;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::ResolvePrivateEnvBinding:
        switch(index) {
        case 0: return IRI_GEN::NAME;
        case 1: return IRI_GEN::FULLY_RESOLVE;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::PVTEnvRead:
        switch(index) {
        case 0: return IRI_GEN::SYMBOL;
        case 1: return IRI_GEN::METHOD;
        case 2: return IRI_GEN::FULLY_RESOLVE;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSPrivate:
        switch(index) {
        case 0: return IRI_GEN::IridiumPrimitive;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSPrivateFieldWrite:
        switch(index) {
        case 0: return IRI_GEN::DECL;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::PoolBinding:
        switch(index) {
        case 0: return IRI_GEN::REFIDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::ResolveContinueTarget:
        switch(index) {
        case 0: return IRI_GEN::Label;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::ResolveBreakTarget:
        switch(index) {
        case 0: return IRI_GEN::Label;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::InvokeFinalizer:
        switch(index) {
        case 0: return IRI_GEN::IDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSForOfStart:
        switch(index) {
        case 0: return IRI_GEN::AWAIT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSForOfNext:
        switch(index) {
        case 0: return IRI_GEN::AWAIT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::PushCatchContext:
        switch(index) {
        case 0: return IRI_GEN::IDX;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSCatchContext:
        switch(index) {
        case 0: return IRI_GEN::NAME;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSBinop:
        switch(index) {
        case 0: return IRI_GEN::OP;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSUnop:
        switch(index) {
        case 0: return IRI_GEN::OP;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::Unop:
        switch(index) {
        case 0: return IRI_GEN::OP;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::Boolean:
        switch(index) {
        case 0: return IRI_GEN::IridiumPrimitive;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::Binop:
        switch(index) {
        case 0: return IRI_GEN::OP;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::Null:
        switch(index) {
        case 0: return IRI_GEN::IridiumPrimitive;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSComputedFieldRead:
        switch(index) {
        case 0: return IRI_GEN::SAFE;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSComputedFieldWrite:
        switch(index) {
        case 0: return IRI_GEN::SAFE;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSDefineObjMethod:
        switch(index) {
        case 0: return IRI_GEN::NOENUM;
        case 1: return IRI_GEN::METHOD;
        case 2: return IRI_GEN::GET;
        case 3: return IRI_GEN::SET;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::RegExp:
        switch(index) {
        case 0: return IRI_GEN::EXP;
        case 1: return IRI_GEN::FLAGS;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::UNOPDelVar:
        switch(index) {
        case 0: return IRI_GEN::NAME;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSBigInt:
        switch(index) {
        case 0: return IRI_GEN::IridiumPrimitive;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::IDOP:
        switch(index) {
        case 0: return IRI_GEN::PREFIX;
        case 1: return IRI_GEN::INCREMENT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSIDOP:
        switch(index) {
        case 0: return IRI_GEN::PREFIX;
        case 1: return IRI_GEN::INCREMENT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::GWrite:
        switch(index) {
        case 0: return IRI_GEN::INIT;
        case 1: return IRI_GEN::SAFE;
        case 2: return IRI_GEN::DECLVAR;
        case 3: return IRI_GEN::DECLFUN;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::LWrite:
        switch(index) {
        case 0: return IRI_GEN::INIT;
        case 1: return IRI_GEN::SAFE;
        case 2: return IRI_GEN::THISINIT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::RWrite:
        switch(index) {
        case 0: return IRI_GEN::INIT;
        case 1: return IRI_GEN::SAFE;
        case 2: return IRI_GEN::THISINIT;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::MWrite:
        switch(index) {
        case 0: return IRI_GEN::INIT;
        case 1: return IRI_GEN::SAFE;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      case IRI_GEN::JSCTX:
        switch(index) {
        case 0: return IRI_GEN::OPID;
          default: throw std::runtime_error("Invalid flag index");
        }
        break;
      default: throw std::runtime_error("Invalid tag for flag lookup");
    }
  }
};
} // namespace IRI_GEN