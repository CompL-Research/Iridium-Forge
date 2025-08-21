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


IRISEXP specializeSEXP(IRISEXP val);
