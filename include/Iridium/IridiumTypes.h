#pragma once
#include "Iridium/Globals.h"
#include "Iridium/IridiumSEXP.h"

#define DEFINE_ARG_FUNCS(FuncName, Index) \
  void set##FuncName(const IRISEXP &obj)  \
  {                                       \
    this->args.at(Index) = obj;           \
  }                                       \
  IRISEXP get##FuncName() const           \
  {                                       \
    return this->args.at(Index);          \
  }

#define DEFINE_VOID_FLAG_FUNCS(FlagName)            \
  void set##FlagName() { setFlag(#FlagName); }      \
  void unset##FlagName() { removeFlag(#FlagName); } \
  bool has##FlagName() { return hasFlag(#FlagName); }

#define DEFINE_STRING_FLAG_FUNCS(FlagName)                                    \
  void set##FlagName(const std::string &value) { setFlag(#FlagName, value); } \
  void unset##FlagName() { removeFlag(#FlagName); }                           \
  bool has##FlagName() { return hasFlag(#FlagName); }                         \
  std::string get##FlagName() { return getFlagString(#FlagName); }

#define DEFINE_DOUBLE_FLAG_FUNCS(FlagName)                        \
  void set##FlagName(double value) { setFlag(#FlagName, value); } \
  void unset##FlagName() { removeFlag(#FlagName); }               \
  bool has##FlagName() { return hasFlag(#FlagName); }             \
  double get##FlagName() { return getFlagDouble(#FlagName); }

#define DEFINE_BOOL_FLAG_FUNCS(FlagName)                        \
  void set##FlagName(bool value) { setFlag(#FlagName, value); } \
  void unset##FlagName() { removeFlag(#FlagName); }             \
  bool has##FlagName() { return hasFlag(#FlagName); }           \
  bool get##FlagName() { return getFlagBool(#FlagName); }

class FileSEXP : public IridiumSEXP
{
public:
  FileSEXP(IRISEXP obj)
  {
    assert(obj->tag == "File");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_VOID_FLAG_FUNCS(JSScript)
  DEFINE_VOID_FLAG_FUNCS(JSModule)
};

class ResolveEnvBindingSEXP : public IridiumSEXP
{
public:
  ResolveEnvBindingSEXP(IRISEXP obj)
  {
    assert(obj->tag == "ResolveEnvBinding");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_VOID_FLAG_FUNCS(ASW)
  DEFINE_STRING_FLAG_FUNCS(NAME)
};

class ListSEXP : public IridiumSEXP
{
public:
  ListSEXP(IRISEXP obj)
  {
    assert(obj->tag == "List");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_STRING_FLAG_FUNCS(TYPE)
};

class JSImplicitBindingDeclarationSEXP : public IridiumSEXP
{
public:
  JSImplicitBindingDeclarationSEXP(IRISEXP obj)
  {
    assert(obj->tag == "JSImplicitBindingDeclaration");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(Store, 0)
  DEFINE_ARG_FUNCS(Args, 1)

  DEFINE_STRING_FLAG_FUNCS(NAME)

  DEFINE_VOID_FLAG_FUNCS(JSLET)
  DEFINE_VOID_FLAG_FUNCS(JSCONST)
  DEFINE_VOID_FLAG_FUNCS(JSVAR)

  DEFINE_DOUBLE_FLAG_FUNCS(OPID)
  DEFINE_VOID_FLAG_FUNCS(SAFE)
  DEFINE_VOID_FLAG_FUNCS(THISINIT)
  DEFINE_VOID_FLAG_FUNCS(SLOPPY)
  DEFINE_VOID_FLAG_FUNCS(SKIPINIT)
};

class EnvReadSEXP : public IridiumSEXP
{
public:
  EnvReadSEXP(IRISEXP obj)
  {
    assert(obj->tag == "EnvRead");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(Obj, 0)
};

class IfJumpSEXP : public IridiumSEXP
{
public:
  IfJumpSEXP(IRISEXP obj)
  {
    assert(obj->tag == "IfJump");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(Test, 0)
  DEFINE_DOUBLE_FLAG_FUNCS(IDX)
  DEFINE_VOID_FLAG_FUNCS(NOT)

};

class StringSEXP : public IridiumSEXP
{
public:
  StringSEXP(IRISEXP obj)
  {
    assert(obj->tag == "String");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_STRING_FLAG_FUNCS(IridiumPrimitive)
};

class FieldReadSEXP : public IridiumSEXP
{
public:
  FieldReadSEXP(IRISEXP obj)
  {
    assert(obj->tag == "FieldRead");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(Obj, 0)
  DEFINE_ARG_FUNCS(Field, 1)
};

class JSExplicitBindingDeclarationSEXP : public IridiumSEXP
{
public:
  JSExplicitBindingDeclarationSEXP(IRISEXP obj)
  {
    assert(obj->tag == "JSExplicitBindingDeclaration");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(LValTarget, 0)
  DEFINE_ARG_FUNCS(RVal, 1)

  DEFINE_VOID_FLAG_FUNCS(JSLET)
  DEFINE_VOID_FLAG_FUNCS(JSCONST)
  DEFINE_VOID_FLAG_FUNCS(JSVAR)

  DEFINE_VOID_FLAG_FUNCS(SAFE)
  DEFINE_VOID_FLAG_FUNCS(THISINIT)
  DEFINE_VOID_FLAG_FUNCS(SLOPPY)

};

class CallSiteSEXP : public IridiumSEXP
{
public:
  CallSiteSEXP(IRISEXP obj)
  {
    assert(obj->tag == "CallSite");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_VOID_FLAG_FUNCS(CCall)
  DEFINE_VOID_FLAG_FUNCS(ConstructorCall)
  DEFINE_VOID_FLAG_FUNCS(PrivateCall)
  DEFINE_VOID_FLAG_FUNCS(Import)
  DEFINE_VOID_FLAG_FUNCS(Super)
  DEFINE_VOID_FLAG_FUNCS(JSDirectEval)

};

class ReturnAsyncSEXP : public IridiumSEXP
{
public:
  ReturnAsyncSEXP(IRISEXP obj)
  {
    assert(obj->tag == "ReturnAsync");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(RetVal, 0)

};

enum BBSEXPFLAGS {
  TopLevel,
  ClosureBoundary,
  Lexical
};

class BBSEXP : public IridiumSEXP
{
public:
  BBSEXP(IRISEXP obj)
  {
    assert(obj->tag == "BB");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_VOID_FLAG_FUNCS(TopLevel)
  DEFINE_VOID_FLAG_FUNCS(ClosureBoundary)
  DEFINE_VOID_FLAG_FUNCS(Lexical)

  DEFINE_DOUBLE_FLAG_FUNCS(IDX)

  DEFINE_DOUBLE_FLAG_FUNCS(ScopeIDX)

};

class ReturnSEXP : public IridiumSEXP
{
public:
  ReturnSEXP(IRISEXP obj)
  {
    assert(obj->tag == "Return");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_VOID_FLAG_FUNCS(ModuleEarlyReturn)
};

class IfElseJumpSEXP : public IridiumSEXP
{
public:
  IfElseJumpSEXP(IRISEXP obj)
  {
    assert(obj->tag == "IfElseJump");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(Test, 0)

  DEFINE_DOUBLE_FLAG_FUNCS(TRUE)

  DEFINE_DOUBLE_FLAG_FUNCS(FALSE)
};

class GotoSEXP : public IridiumSEXP
{
public:
  GotoSEXP(IRISEXP obj)
  {
    assert(obj->tag == "Goto");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }
  DEFINE_DOUBLE_FLAG_FUNCS(IDX)
};

class JSFuncDeclSEXP : public IridiumSEXP
{
public:
  JSFuncDeclSEXP(IRISEXP obj)
  {
    assert(obj->tag == "JSFuncDecl");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(LValTarget, 0)
  DEFINE_ARG_FUNCS(RVal, 1)

  DEFINE_VOID_FLAG_FUNCS(JSLET)
  DEFINE_VOID_FLAG_FUNCS(JSCONST)
  DEFINE_VOID_FLAG_FUNCS(JSVAR)

  DEFINE_VOID_FLAG_FUNCS(SAFE)
  DEFINE_VOID_FLAG_FUNCS(THISINIT)
  DEFINE_VOID_FLAG_FUNCS(SLOPPY)
};

class LambdaSEXP : public IridiumSEXP
{
public:
  LambdaSEXP(IRISEXP obj)
  {
    assert(obj->tag == "Lambda");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_DOUBLE_FLAG_FUNCS(StartBBIDX)
};

class NOPSEXP : public IridiumSEXP
{
public:
  NOPSEXP(IRISEXP obj)
  {
    assert(obj->tag == "NOP");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }
};

class BBContainerSEXP : public IridiumSEXP
{
public:
  BBContainerSEXP(IRISEXP obj)
  {
    assert(obj->tag == "BBContainer");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(Bindings, 0)
  DEFINE_ARG_FUNCS(BB, 1)

  DEFINE_DOUBLE_FLAG_FUNCS(ECMAArgs)
  DEFINE_DOUBLE_FLAG_FUNCS(StartBBIDX)
  DEFINE_DOUBLE_FLAG_FUNCS(ScopeIDX)

  DEFINE_VOID_FLAG_FUNCS(ARGUMENTS)
  DEFINE_VOID_FLAG_FUNCS(ASYNC)
  DEFINE_VOID_FLAG_FUNCS(STRICT)
  DEFINE_VOID_FLAG_FUNCS(GENERATOR)
  DEFINE_VOID_FLAG_FUNCS(PROTO)
  DEFINE_VOID_FLAG_FUNCS(NEW)
  DEFINE_VOID_FLAG_FUNCS(SCALL)
  DEFINE_VOID_FLAG_FUNCS(SOBJ)
  DEFINE_VOID_FLAG_FUNCS(HOME)
  DEFINE_VOID_FLAG_FUNCS(DERIVED)
  DEFINE_VOID_FLAG_FUNCS(TopLevel)

  DEFINE_DOUBLE_FLAG_FUNCS(ContainerFlagID)
};

class BindingsSEXP : public IridiumSEXP
{
public:
  BindingsSEXP(IRISEXP obj)
  {
    assert(obj->tag == "Bindings");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(LocalBindings, 0)
  DEFINE_ARG_FUNCS(RemoteBindings, 1)
  DEFINE_ARG_FUNCS(Lambdas, 2)

  DEFINE_DOUBLE_FLAG_FUNCS(ParentScope)
};

class StarExportSEXP : public IridiumSEXP
{
public:
  StarExportSEXP(IRISEXP obj)
  {
    assert(obj->tag == "StarExport");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_DOUBLE_FLAG_FUNCS(MODULEREQIDX)
};

class StaticImportSEXP : public IridiumSEXP
{
public:
  StaticImportSEXP(IRISEXP obj)
  {
    assert(obj->tag == "StaticImport");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(StorageLocation, 0)

  DEFINE_STRING_FLAG_FUNCS(FIELD)
  DEFINE_DOUBLE_FLAG_FUNCS(MODULEREQIDX)
};

class LocalStaticExportSEXP : public IridiumSEXP
{
public:
  LocalStaticExportSEXP(IRISEXP obj)
  {
    assert(obj->tag == "LocalStaticExport");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(StorageLocation, 0)

  DEFINE_STRING_FLAG_FUNCS(LOCALNAME)
  DEFINE_STRING_FLAG_FUNCS(EXPORTNAME)
};

class NamedReexportSEXP : public IridiumSEXP
{
public:
  NamedReexportSEXP(IRISEXP obj)
  {
    assert(obj->tag == "NamedReexport");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_DOUBLE_FLAG_FUNCS(MODULEREQIDX)
  DEFINE_STRING_FLAG_FUNCS(EXPORTNAME)
};

class ModuleRequestSEXP : public IridiumSEXP
{
public:
  ModuleRequestSEXP(IRISEXP obj)
  {
    assert(obj->tag == "ModuleRequest");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_STRING_FLAG_FUNCS(SOURCE)
  DEFINE_DOUBLE_FLAG_FUNCS(REQIDX)
};

class EnvBindingSEXP  : public IridiumSEXP
{
public:
  EnvBindingSEXP(IRISEXP obj)
  {
    assert(obj->tag == "EnvBinding");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_STRING_FLAG_FUNCS(NAME)
  DEFINE_VOID_FLAG_FUNCS(ASW)

  DEFINE_VOID_FLAG_FUNCS(JSARG)
  DEFINE_VOID_FLAG_FUNCS(JSRESTARG)
  DEFINE_VOID_FLAG_FUNCS(JSLET)
  DEFINE_VOID_FLAG_FUNCS(JSCONST)
  DEFINE_VOID_FLAG_FUNCS(JSVAR)
  
  DEFINE_DOUBLE_FLAG_FUNCS(IDX)
  DEFINE_DOUBLE_FLAG_FUNCS(REFIDX)
  DEFINE_DOUBLE_FLAG_FUNCS(Scope)
  DEFINE_DOUBLE_FLAG_FUNCS(ParentScope)
  DEFINE_DOUBLE_FLAG_FUNCS(NEXT)
};

class RemoteEnvBindingSEXP  : public IridiumSEXP
{
public:
  RemoteEnvBindingSEXP(IRISEXP obj)
  {
    assert(obj->tag == "RemoteEnvBinding");

    this->tag = obj->tag;
    this->args = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }

  DEFINE_ARG_FUNCS(ParentReference, 0)

  DEFINE_DOUBLE_FLAG_FUNCS(REFIDX)
  DEFINE_VOID_FLAG_FUNCS(NSIMPORT)
};

IRISEXP specializeSEXP(IRISEXP val);

BBSEXPFLAGS getBBFlag(std::shared_ptr<BBSEXP> b);

void setBBFlag(std::shared_ptr<BBSEXP> b, BBSEXPFLAGS flagToSet);

std::shared_ptr<NOPSEXP> makeNOPSEXP();
std::shared_ptr<ListSEXP> makeListSEXP();
std::shared_ptr<BindingsSEXP> makeBindingsSEXP(double parentScope);
std::shared_ptr<BBContainerSEXP> makeBBContainerSEXP(double scopeIdx, double parentScope);

enum EnvBindingSEXPKindFlag {
  JSARG, JSRESTARG, JSLET, JSCONST, JSVAR
};
std::shared_ptr<EnvBindingSEXP> makeEnvBindingSEXP(double refIdx, double idx, std::string b, EnvBindingSEXPKindFlag kindFlag, double scope, double parentScope);
std::shared_ptr<RemoteEnvBindingSEXP> makeRemoteEnvBindingSEXP(std::shared_ptr<EnvBindingSEXP>, double refIdx);