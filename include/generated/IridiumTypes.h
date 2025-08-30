// Generated: 2025-08-31 02:59:18
#pragma once
#include "Iridium/Globals.h"
#include "Iridium/IridiumSEXP.h"
class FileSEXP : public IridiumSEXP {
private:

  FileSEXP() { this->tag = "File"; }
  
public:
  static std::shared_ptr<FileSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "File");
    auto res = std::shared_ptr<FileSEXP>(new FileSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  FileSEXP(bool JSScript, bool JSModule) {
    this->tag = "File";
    if (JSScript) this->setJSScript();
    if (JSModule) this->setJSModule();
  }


  void setJSScript() { setFlag("JSScript"); }
  void unsetJSScript() { removeFlag("JSScript"); }
  bool hasJSScript() { return hasFlag("JSScript"); }


  void setJSModule() { setFlag("JSModule"); }
  void unsetJSModule() { removeFlag("JSModule"); }
  bool hasJSModule() { return hasFlag("JSModule"); }

};

class ResolveEnvBindingSEXP : public IridiumSEXP {
private:

  ResolveEnvBindingSEXP() { this->tag = "ResolveEnvBinding"; }
  
public:
  static std::shared_ptr<ResolveEnvBindingSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "ResolveEnvBinding");
    auto res = std::shared_ptr<ResolveEnvBindingSEXP>(new ResolveEnvBindingSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  ResolveEnvBindingSEXP(std::string NAME, bool ASW) {
    this->tag = "ResolveEnvBinding";
    this->setNAME(NAME);
    if (ASW) this->setASW();
  }


  void setNAME(const std::string &value) { setFlag("NAME", value); }
  void unsetNAME() { removeFlag("NAME"); }
  bool hasNAME() { return hasFlag("NAME"); }
  std::string getNAME() { return getFlagString("NAME"); }


  void setASW() { setFlag("ASW"); }
  void unsetASW() { removeFlag("ASW"); }
  bool hasASW() { return hasFlag("ASW"); }

};

class ListSEXP : public IridiumSEXP {
private:

  ListSEXP() { this->tag = "List"; }
  
public:
  static std::shared_ptr<ListSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "List");
    auto res = std::shared_ptr<ListSEXP>(new ListSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  ListSEXP(std::string TYPE) {
    this->tag = "List";
    this->setTYPE(TYPE);
  }


  void setTYPE(const std::string &value) { setFlag("TYPE", value); }
  void unsetTYPE() { removeFlag("TYPE"); }
  bool hasTYPE() { return hasFlag("TYPE"); }
  std::string getTYPE() { return getFlagString("TYPE"); }

};

class JSImplicitBindingDeclarationSEXP : public IridiumSEXP {
private:

  JSImplicitBindingDeclarationSEXP() { this->tag = "JSImplicitBindingDeclaration"; }
  
public:
  static std::shared_ptr<JSImplicitBindingDeclarationSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSImplicitBindingDeclaration");
    auto res = std::shared_ptr<JSImplicitBindingDeclarationSEXP>(new JSImplicitBindingDeclarationSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSImplicitBindingDeclarationSEXP(IRISEXP Store, IRISEXP Args, std::string NAME, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SKIPINIT, bool SAFE, bool THISINIT, double OPID) {
    this->tag = "JSImplicitBindingDeclaration";
    this->args.push_back(Store);
    this->args.push_back(Args);
    this->setNAME(NAME);
    if (JSLET) this->setJSLET();
    if (JSCONST) this->setJSCONST();
    if (JSVAR) this->setJSVAR();
    if (SLOPPY) this->setSLOPPY();
    if (SKIPINIT) this->setSKIPINIT();
    this->setSAFE(SAFE);
    this->setTHISINIT(THISINIT);
    this->setOPID(OPID);
  }


  void setStore(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasStore() { return 0 < this->args.size(); }
  IRISEXP getStore() const { return this->args.at(0); }


  void setArgs(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasArgs() { return 1 < this->args.size(); }
  IRISEXP getArgs() const { return this->args.at(1); }


  void setNAME(const std::string &value) { setFlag("NAME", value); }
  void unsetNAME() { removeFlag("NAME"); }
  bool hasNAME() { return hasFlag("NAME"); }
  std::string getNAME() { return getFlagString("NAME"); }


  void setJSLET() { setFlag("JSLET"); }
  void unsetJSLET() { removeFlag("JSLET"); }
  bool hasJSLET() { return hasFlag("JSLET"); }


  void setJSCONST() { setFlag("JSCONST"); }
  void unsetJSCONST() { removeFlag("JSCONST"); }
  bool hasJSCONST() { return hasFlag("JSCONST"); }


  void setJSVAR() { setFlag("JSVAR"); }
  void unsetJSVAR() { removeFlag("JSVAR"); }
  bool hasJSVAR() { return hasFlag("JSVAR"); }


  void setSLOPPY() { setFlag("SLOPPY"); }
  void unsetSLOPPY() { removeFlag("SLOPPY"); }
  bool hasSLOPPY() { return hasFlag("SLOPPY"); }


  void setSKIPINIT() { setFlag("SKIPINIT"); }
  void unsetSKIPINIT() { removeFlag("SKIPINIT"); }
  bool hasSKIPINIT() { return hasFlag("SKIPINIT"); }


  void setSAFE(bool value) { setFlag("SAFE", value); }
  void unsetSAFE() { removeFlag("SAFE"); }
  bool hasSAFE() { return hasFlag("SAFE"); }
  bool getSAFE() { return getFlagBoolean("SAFE"); }


  void setTHISINIT(bool value) { setFlag("THISINIT", value); }
  void unsetTHISINIT() { removeFlag("THISINIT"); }
  bool hasTHISINIT() { return hasFlag("THISINIT"); }
  bool getTHISINIT() { return getFlagBoolean("THISINIT"); }


  void setOPID(double value) { setFlag("OPID", value); }
  void unsetOPID() { removeFlag("OPID"); }
  bool hasOPID() { return hasFlag("OPID"); }
  double getOPID() { return getFlagDouble("OPID"); }

};

class EnvReadSEXP : public IridiumSEXP {
private:

  EnvReadSEXP() { this->tag = "EnvRead"; }
  
public:
  static std::shared_ptr<EnvReadSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "EnvRead");
    auto res = std::shared_ptr<EnvReadSEXP>(new EnvReadSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  EnvReadSEXP(IRISEXP Obj) {
    this->tag = "EnvRead";
    this->args.push_back(Obj);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }

};

class StringSEXP : public IridiumSEXP {
private:

  StringSEXP() { this->tag = "String"; }
  
public:
  static std::shared_ptr<StringSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "String");
    auto res = std::shared_ptr<StringSEXP>(new StringSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  StringSEXP(std::string IridiumPrimitive) {
    this->tag = "String";
    this->setIridiumPrimitive(IridiumPrimitive);
  }


  void setIridiumPrimitive(const std::string &value) { setFlag("IridiumPrimitive", value); }
  void unsetIridiumPrimitive() { removeFlag("IridiumPrimitive"); }
  bool hasIridiumPrimitive() { return hasFlag("IridiumPrimitive"); }
  std::string getIridiumPrimitive() { return getFlagString("IridiumPrimitive"); }

};

class FieldReadSEXP : public IridiumSEXP {
private:

  FieldReadSEXP() { this->tag = "FieldRead"; }
  
public:
  static std::shared_ptr<FieldReadSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "FieldRead");
    auto res = std::shared_ptr<FieldReadSEXP>(new FieldReadSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  FieldReadSEXP(IRISEXP Obj, IRISEXP Field) {
    this->tag = "FieldRead";
    this->args.push_back(Obj);
    this->args.push_back(Field);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }


  void setField(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasField() { return 1 < this->args.size(); }
  IRISEXP getField() const { return this->args.at(1); }

};

class JSExplicitBindingDeclarationSEXP : public IridiumSEXP {
private:

  JSExplicitBindingDeclarationSEXP() { this->tag = "JSExplicitBindingDeclaration"; }
  
public:
  static std::shared_ptr<JSExplicitBindingDeclarationSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSExplicitBindingDeclaration");
    auto res = std::shared_ptr<JSExplicitBindingDeclarationSEXP>(new JSExplicitBindingDeclarationSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSExplicitBindingDeclarationSEXP(IRISEXP LValTarget, IRISEXP RVal, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SAFE, bool THISINIT) {
    this->tag = "JSExplicitBindingDeclaration";
    this->args.push_back(LValTarget);
    this->args.push_back(RVal);
    if (JSLET) this->setJSLET();
    if (JSCONST) this->setJSCONST();
    if (JSVAR) this->setJSVAR();
    if (SLOPPY) this->setSLOPPY();
    this->setSAFE(SAFE);
    this->setTHISINIT(THISINIT);
  }


  void setLValTarget(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasLValTarget() { return 0 < this->args.size(); }
  IRISEXP getLValTarget() const { return this->args.at(0); }


  void setRVal(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasRVal() { return 1 < this->args.size(); }
  IRISEXP getRVal() const { return this->args.at(1); }


  void setJSLET() { setFlag("JSLET"); }
  void unsetJSLET() { removeFlag("JSLET"); }
  bool hasJSLET() { return hasFlag("JSLET"); }


  void setJSCONST() { setFlag("JSCONST"); }
  void unsetJSCONST() { removeFlag("JSCONST"); }
  bool hasJSCONST() { return hasFlag("JSCONST"); }


  void setJSVAR() { setFlag("JSVAR"); }
  void unsetJSVAR() { removeFlag("JSVAR"); }
  bool hasJSVAR() { return hasFlag("JSVAR"); }


  void setSLOPPY() { setFlag("SLOPPY"); }
  void unsetSLOPPY() { removeFlag("SLOPPY"); }
  bool hasSLOPPY() { return hasFlag("SLOPPY"); }


  void setSAFE(bool value) { setFlag("SAFE", value); }
  void unsetSAFE() { removeFlag("SAFE"); }
  bool hasSAFE() { return hasFlag("SAFE"); }
  bool getSAFE() { return getFlagBoolean("SAFE"); }


  void setTHISINIT(bool value) { setFlag("THISINIT", value); }
  void unsetTHISINIT() { removeFlag("THISINIT"); }
  bool hasTHISINIT() { return hasFlag("THISINIT"); }
  bool getTHISINIT() { return getFlagBoolean("THISINIT"); }

};

class CallSiteSEXP : public IridiumSEXP {
private:

  CallSiteSEXP() { this->tag = "CallSite"; }
  
public:
  static std::shared_ptr<CallSiteSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "CallSite");
    auto res = std::shared_ptr<CallSiteSEXP>(new CallSiteSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  CallSiteSEXP(bool CCall, bool ConstructorCall, bool PrivateCall, bool Import, bool Super, bool V8Intrinsic, double JSDirectEval) {
    this->tag = "CallSite";
    if (CCall) this->setCCall();
    if (ConstructorCall) this->setConstructorCall();
    if (PrivateCall) this->setPrivateCall();
    if (Import) this->setImport();
    if (Super) this->setSuper();
    if (V8Intrinsic) this->setV8Intrinsic();
    this->setJSDirectEval(JSDirectEval);
  }


  void setCCall() { setFlag("CCall"); }
  void unsetCCall() { removeFlag("CCall"); }
  bool hasCCall() { return hasFlag("CCall"); }


  void setConstructorCall() { setFlag("ConstructorCall"); }
  void unsetConstructorCall() { removeFlag("ConstructorCall"); }
  bool hasConstructorCall() { return hasFlag("ConstructorCall"); }


  void setPrivateCall() { setFlag("PrivateCall"); }
  void unsetPrivateCall() { removeFlag("PrivateCall"); }
  bool hasPrivateCall() { return hasFlag("PrivateCall"); }


  void setImport() { setFlag("Import"); }
  void unsetImport() { removeFlag("Import"); }
  bool hasImport() { return hasFlag("Import"); }


  void setSuper() { setFlag("Super"); }
  void unsetSuper() { removeFlag("Super"); }
  bool hasSuper() { return hasFlag("Super"); }


  void setV8Intrinsic() { setFlag("V8Intrinsic"); }
  void unsetV8Intrinsic() { removeFlag("V8Intrinsic"); }
  bool hasV8Intrinsic() { return hasFlag("V8Intrinsic"); }


  void setJSDirectEval(double value) { setFlag("JSDirectEval", value); }
  void unsetJSDirectEval() { removeFlag("JSDirectEval"); }
  bool hasJSDirectEval() { return hasFlag("JSDirectEval"); }
  double getJSDirectEval() { return getFlagDouble("JSDirectEval"); }

};

class ReturnAsyncSEXP : public IridiumSEXP {
private:

  ReturnAsyncSEXP() { this->tag = "ReturnAsync"; }
  
public:
  static std::shared_ptr<ReturnAsyncSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "ReturnAsync");
    auto res = std::shared_ptr<ReturnAsyncSEXP>(new ReturnAsyncSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  ReturnAsyncSEXP(IRISEXP RetVal) {
    this->tag = "ReturnAsync";
    this->args.push_back(RetVal);
  }


  void setRetVal(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasRetVal() { return 0 < this->args.size(); }
  IRISEXP getRetVal() const { return this->args.at(0); }

};

class BBSEXP : public IridiumSEXP {
private:

  BBSEXP() { this->tag = "BB"; }
  
public:
  static std::shared_ptr<BBSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "BB");
    auto res = std::shared_ptr<BBSEXP>(new BBSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  BBSEXP(bool TopLevel, bool ClosureBoundary, bool Lexical, double IDX, double ScopeIDX) {
    this->tag = "BB";
    if (TopLevel) this->setTopLevel();
    if (ClosureBoundary) this->setClosureBoundary();
    if (Lexical) this->setLexical();
    this->setIDX(IDX);
    this->setScopeIDX(ScopeIDX);
  }


  void setTopLevel() { setFlag("TopLevel"); }
  void unsetTopLevel() { removeFlag("TopLevel"); }
  bool hasTopLevel() { return hasFlag("TopLevel"); }


  void setClosureBoundary() { setFlag("ClosureBoundary"); }
  void unsetClosureBoundary() { removeFlag("ClosureBoundary"); }
  bool hasClosureBoundary() { return hasFlag("ClosureBoundary"); }


  void setLexical() { setFlag("Lexical"); }
  void unsetLexical() { removeFlag("Lexical"); }
  bool hasLexical() { return hasFlag("Lexical"); }


  void setIDX(double value) { setFlag("IDX", value); }
  void unsetIDX() { removeFlag("IDX"); }
  bool hasIDX() { return hasFlag("IDX"); }
  double getIDX() { return getFlagDouble("IDX"); }


  void setScopeIDX(double value) { setFlag("ScopeIDX", value); }
  void unsetScopeIDX() { removeFlag("ScopeIDX"); }
  bool hasScopeIDX() { return hasFlag("ScopeIDX"); }
  double getScopeIDX() { return getFlagDouble("ScopeIDX"); }

};

class ReturnSEXP : public IridiumSEXP {
private:

  ReturnSEXP() { this->tag = "Return"; }
  
public:
  static std::shared_ptr<ReturnSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Return");
    auto res = std::shared_ptr<ReturnSEXP>(new ReturnSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  ReturnSEXP(IRISEXP Obj, bool ModuleEarlyReturn) {
    this->tag = "Return";
    this->args.push_back(Obj);
    if (ModuleEarlyReturn) this->setModuleEarlyReturn();
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }


  void setModuleEarlyReturn() { setFlag("ModuleEarlyReturn"); }
  void unsetModuleEarlyReturn() { removeFlag("ModuleEarlyReturn"); }
  bool hasModuleEarlyReturn() { return hasFlag("ModuleEarlyReturn"); }

};

class IfElseJumpSEXP : public IridiumSEXP {
private:

  IfElseJumpSEXP() { this->tag = "IfElseJump"; }
  
public:
  static std::shared_ptr<IfElseJumpSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "IfElseJump");
    auto res = std::shared_ptr<IfElseJumpSEXP>(new IfElseJumpSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  IfElseJumpSEXP(IRISEXP Test, bool NOT, double TRUE, double FALSE) {
    this->tag = "IfElseJump";
    this->args.push_back(Test);
    if (NOT) this->setNOT();
    this->setTRUE(TRUE);
    this->setFALSE(FALSE);
  }


  void setTest(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasTest() { return 0 < this->args.size(); }
  IRISEXP getTest() const { return this->args.at(0); }


  void setNOT() { setFlag("NOT"); }
  void unsetNOT() { removeFlag("NOT"); }
  bool hasNOT() { return hasFlag("NOT"); }


  void setTRUE(double value) { setFlag("TRUE", value); }
  void unsetTRUE() { removeFlag("TRUE"); }
  bool hasTRUE() { return hasFlag("TRUE"); }
  double getTRUE() { return getFlagDouble("TRUE"); }


  void setFALSE(double value) { setFlag("FALSE", value); }
  void unsetFALSE() { removeFlag("FALSE"); }
  bool hasFALSE() { return hasFlag("FALSE"); }
  double getFALSE() { return getFlagDouble("FALSE"); }

};

class GotoSEXP : public IridiumSEXP {
private:

  GotoSEXP() { this->tag = "Goto"; }
  
public:
  static std::shared_ptr<GotoSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Goto");
    auto res = std::shared_ptr<GotoSEXP>(new GotoSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  GotoSEXP(double IDX) {
    this->tag = "Goto";
    this->setIDX(IDX);
  }


  void setIDX(double value) { setFlag("IDX", value); }
  void unsetIDX() { removeFlag("IDX"); }
  bool hasIDX() { return hasFlag("IDX"); }
  double getIDX() { return getFlagDouble("IDX"); }

};

class JSFuncDeclSEXP : public IridiumSEXP {
private:

  JSFuncDeclSEXP() { this->tag = "JSFuncDecl"; }
  
public:
  static std::shared_ptr<JSFuncDeclSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSFuncDecl");
    auto res = std::shared_ptr<JSFuncDeclSEXP>(new JSFuncDeclSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSFuncDeclSEXP(IRISEXP LValTarget, IRISEXP RVal, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SAFE, bool THISINIT) {
    this->tag = "JSFuncDecl";
    this->args.push_back(LValTarget);
    this->args.push_back(RVal);
    if (JSLET) this->setJSLET();
    if (JSCONST) this->setJSCONST();
    if (JSVAR) this->setJSVAR();
    if (SLOPPY) this->setSLOPPY();
    this->setSAFE(SAFE);
    this->setTHISINIT(THISINIT);
  }


  void setLValTarget(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasLValTarget() { return 0 < this->args.size(); }
  IRISEXP getLValTarget() const { return this->args.at(0); }


  void setRVal(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasRVal() { return 1 < this->args.size(); }
  IRISEXP getRVal() const { return this->args.at(1); }


  void setJSLET() { setFlag("JSLET"); }
  void unsetJSLET() { removeFlag("JSLET"); }
  bool hasJSLET() { return hasFlag("JSLET"); }


  void setJSCONST() { setFlag("JSCONST"); }
  void unsetJSCONST() { removeFlag("JSCONST"); }
  bool hasJSCONST() { return hasFlag("JSCONST"); }


  void setJSVAR() { setFlag("JSVAR"); }
  void unsetJSVAR() { removeFlag("JSVAR"); }
  bool hasJSVAR() { return hasFlag("JSVAR"); }


  void setSLOPPY() { setFlag("SLOPPY"); }
  void unsetSLOPPY() { removeFlag("SLOPPY"); }
  bool hasSLOPPY() { return hasFlag("SLOPPY"); }


  void setSAFE(bool value) { setFlag("SAFE", value); }
  void unsetSAFE() { removeFlag("SAFE"); }
  bool hasSAFE() { return hasFlag("SAFE"); }
  bool getSAFE() { return getFlagBoolean("SAFE"); }


  void setTHISINIT(bool value) { setFlag("THISINIT", value); }
  void unsetTHISINIT() { removeFlag("THISINIT"); }
  bool hasTHISINIT() { return hasFlag("THISINIT"); }
  bool getTHISINIT() { return getFlagBoolean("THISINIT"); }

};

class LambdaSEXP : public IridiumSEXP {
private:

  LambdaSEXP() { this->tag = "Lambda"; }
  
public:
  static std::shared_ptr<LambdaSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Lambda");
    auto res = std::shared_ptr<LambdaSEXP>(new LambdaSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  LambdaSEXP(double StartBBIDX) {
    this->tag = "Lambda";
    this->setStartBBIDX(StartBBIDX);
  }


  void setStartBBIDX(double value) { setFlag("StartBBIDX", value); }
  void unsetStartBBIDX() { removeFlag("StartBBIDX"); }
  bool hasStartBBIDX() { return hasFlag("StartBBIDX"); }
  double getStartBBIDX() { return getFlagDouble("StartBBIDX"); }

};

class NOPSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<NOPSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "NOP");
    auto res = std::shared_ptr<NOPSEXP>(new NOPSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  NOPSEXP() {
    this->tag = "NOP";
  }

};

class BBContainerSEXP : public IridiumSEXP {
private:

  BBContainerSEXP() { this->tag = "BBContainer"; }
  
public:
  static std::shared_ptr<BBContainerSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "BBContainer");
    auto res = std::shared_ptr<BBContainerSEXP>(new BBContainerSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  BBContainerSEXP(IRISEXP Bindings, IRISEXP BB, bool ARGUMENTS, bool ASYNC, bool STRICT, bool GENERATOR, bool PROTO, bool NEW, bool SCALL, bool SOBJ, bool HOME, bool DERIVED, bool TopLevel, double ECMAArgs, double StartBBIDX, double ScopeIDX, double ContainerFlagID) {
    this->tag = "BBContainer";
    this->args.push_back(Bindings);
    this->args.push_back(BB);
    if (ARGUMENTS) this->setARGUMENTS();
    if (ASYNC) this->setASYNC();
    if (STRICT) this->setSTRICT();
    if (GENERATOR) this->setGENERATOR();
    if (PROTO) this->setPROTO();
    if (NEW) this->setNEW();
    if (SCALL) this->setSCALL();
    if (SOBJ) this->setSOBJ();
    if (HOME) this->setHOME();
    if (DERIVED) this->setDERIVED();
    if (TopLevel) this->setTopLevel();
    this->setECMAArgs(ECMAArgs);
    this->setStartBBIDX(StartBBIDX);
    this->setScopeIDX(ScopeIDX);
    this->setContainerFlagID(ContainerFlagID);
  }


  void setBindings(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasBindings() { return 0 < this->args.size(); }
  IRISEXP getBindings() const { return this->args.at(0); }


  void setBB(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasBB() { return 1 < this->args.size(); }
  IRISEXP getBB() const { return this->args.at(1); }


  void setARGUMENTS() { setFlag("ARGUMENTS"); }
  void unsetARGUMENTS() { removeFlag("ARGUMENTS"); }
  bool hasARGUMENTS() { return hasFlag("ARGUMENTS"); }


  void setASYNC() { setFlag("ASYNC"); }
  void unsetASYNC() { removeFlag("ASYNC"); }
  bool hasASYNC() { return hasFlag("ASYNC"); }


  void setSTRICT() { setFlag("STRICT"); }
  void unsetSTRICT() { removeFlag("STRICT"); }
  bool hasSTRICT() { return hasFlag("STRICT"); }


  void setGENERATOR() { setFlag("GENERATOR"); }
  void unsetGENERATOR() { removeFlag("GENERATOR"); }
  bool hasGENERATOR() { return hasFlag("GENERATOR"); }


  void setPROTO() { setFlag("PROTO"); }
  void unsetPROTO() { removeFlag("PROTO"); }
  bool hasPROTO() { return hasFlag("PROTO"); }


  void setNEW() { setFlag("NEW"); }
  void unsetNEW() { removeFlag("NEW"); }
  bool hasNEW() { return hasFlag("NEW"); }


  void setSCALL() { setFlag("SCALL"); }
  void unsetSCALL() { removeFlag("SCALL"); }
  bool hasSCALL() { return hasFlag("SCALL"); }


  void setSOBJ() { setFlag("SOBJ"); }
  void unsetSOBJ() { removeFlag("SOBJ"); }
  bool hasSOBJ() { return hasFlag("SOBJ"); }


  void setHOME() { setFlag("HOME"); }
  void unsetHOME() { removeFlag("HOME"); }
  bool hasHOME() { return hasFlag("HOME"); }


  void setDERIVED() { setFlag("DERIVED"); }
  void unsetDERIVED() { removeFlag("DERIVED"); }
  bool hasDERIVED() { return hasFlag("DERIVED"); }


  void setTopLevel() { setFlag("TopLevel"); }
  void unsetTopLevel() { removeFlag("TopLevel"); }
  bool hasTopLevel() { return hasFlag("TopLevel"); }


  void setECMAArgs(double value) { setFlag("ECMAArgs", value); }
  void unsetECMAArgs() { removeFlag("ECMAArgs"); }
  bool hasECMAArgs() { return hasFlag("ECMAArgs"); }
  double getECMAArgs() { return getFlagDouble("ECMAArgs"); }


  void setStartBBIDX(double value) { setFlag("StartBBIDX", value); }
  void unsetStartBBIDX() { removeFlag("StartBBIDX"); }
  bool hasStartBBIDX() { return hasFlag("StartBBIDX"); }
  double getStartBBIDX() { return getFlagDouble("StartBBIDX"); }


  void setScopeIDX(double value) { setFlag("ScopeIDX", value); }
  void unsetScopeIDX() { removeFlag("ScopeIDX"); }
  bool hasScopeIDX() { return hasFlag("ScopeIDX"); }
  double getScopeIDX() { return getFlagDouble("ScopeIDX"); }


  void setContainerFlagID(double value) { setFlag("ContainerFlagID", value); }
  void unsetContainerFlagID() { removeFlag("ContainerFlagID"); }
  bool hasContainerFlagID() { return hasFlag("ContainerFlagID"); }
  double getContainerFlagID() { return getFlagDouble("ContainerFlagID"); }

};

class BindingsSEXP : public IridiumSEXP {
private:

  BindingsSEXP() { this->tag = "Bindings"; }
  
public:
  static std::shared_ptr<BindingsSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Bindings");
    auto res = std::shared_ptr<BindingsSEXP>(new BindingsSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  BindingsSEXP(IRISEXP LocalBindings, IRISEXP RemoteBindings, IRISEXP Lambdas, double ParentScope) {
    this->tag = "Bindings";
    this->args.push_back(LocalBindings);
    this->args.push_back(RemoteBindings);
    this->args.push_back(Lambdas);
    this->setParentScope(ParentScope);
  }


  void setLocalBindings(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasLocalBindings() { return 0 < this->args.size(); }
  IRISEXP getLocalBindings() const { return this->args.at(0); }


  void setRemoteBindings(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasRemoteBindings() { return 1 < this->args.size(); }
  IRISEXP getRemoteBindings() const { return this->args.at(1); }


  void setLambdas(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasLambdas() { return 2 < this->args.size(); }
  IRISEXP getLambdas() const { return this->args.at(2); }


  void setParentScope(double value) { setFlag("ParentScope", value); }
  void unsetParentScope() { removeFlag("ParentScope"); }
  bool hasParentScope() { return hasFlag("ParentScope"); }
  double getParentScope() { return getFlagDouble("ParentScope"); }

};

class StarExportSEXP : public IridiumSEXP {
private:

  StarExportSEXP() { this->tag = "StarExport"; }
  
public:
  static std::shared_ptr<StarExportSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "StarExport");
    auto res = std::shared_ptr<StarExportSEXP>(new StarExportSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  StarExportSEXP(double MODULEREQIDX) {
    this->tag = "StarExport";
    this->setMODULEREQIDX(MODULEREQIDX);
  }


  void setMODULEREQIDX(double value) { setFlag("MODULEREQIDX", value); }
  void unsetMODULEREQIDX() { removeFlag("MODULEREQIDX"); }
  bool hasMODULEREQIDX() { return hasFlag("MODULEREQIDX"); }
  double getMODULEREQIDX() { return getFlagDouble("MODULEREQIDX"); }

};

class StaticImportSEXP : public IridiumSEXP {
private:

  StaticImportSEXP() { this->tag = "StaticImport"; }
  
public:
  static std::shared_ptr<StaticImportSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "StaticImport");
    auto res = std::shared_ptr<StaticImportSEXP>(new StaticImportSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  StaticImportSEXP(IRISEXP StorageLocation, std::string FIELD, double MODULEREQIDX) {
    this->tag = "StaticImport";
    this->args.push_back(StorageLocation);
    this->setFIELD(FIELD);
    this->setMODULEREQIDX(MODULEREQIDX);
  }


  void setStorageLocation(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasStorageLocation() { return 0 < this->args.size(); }
  IRISEXP getStorageLocation() const { return this->args.at(0); }


  void setFIELD(const std::string &value) { setFlag("FIELD", value); }
  void unsetFIELD() { removeFlag("FIELD"); }
  bool hasFIELD() { return hasFlag("FIELD"); }
  std::string getFIELD() { return getFlagString("FIELD"); }


  void setMODULEREQIDX(double value) { setFlag("MODULEREQIDX", value); }
  void unsetMODULEREQIDX() { removeFlag("MODULEREQIDX"); }
  bool hasMODULEREQIDX() { return hasFlag("MODULEREQIDX"); }
  double getMODULEREQIDX() { return getFlagDouble("MODULEREQIDX"); }

};

class LocalStaticExportSEXP : public IridiumSEXP {
private:

  LocalStaticExportSEXP() { this->tag = "LocalStaticExport"; }
  
public:
  static std::shared_ptr<LocalStaticExportSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "LocalStaticExport");
    auto res = std::shared_ptr<LocalStaticExportSEXP>(new LocalStaticExportSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  LocalStaticExportSEXP(IRISEXP StorageLocation, std::string LOCALNAME, std::string EXPORTNAME) {
    this->tag = "LocalStaticExport";
    this->args.push_back(StorageLocation);
    this->setLOCALNAME(LOCALNAME);
    this->setEXPORTNAME(EXPORTNAME);
  }


  void setStorageLocation(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasStorageLocation() { return 0 < this->args.size(); }
  IRISEXP getStorageLocation() const { return this->args.at(0); }


  void setLOCALNAME(const std::string &value) { setFlag("LOCALNAME", value); }
  void unsetLOCALNAME() { removeFlag("LOCALNAME"); }
  bool hasLOCALNAME() { return hasFlag("LOCALNAME"); }
  std::string getLOCALNAME() { return getFlagString("LOCALNAME"); }


  void setEXPORTNAME(const std::string &value) { setFlag("EXPORTNAME", value); }
  void unsetEXPORTNAME() { removeFlag("EXPORTNAME"); }
  bool hasEXPORTNAME() { return hasFlag("EXPORTNAME"); }
  std::string getEXPORTNAME() { return getFlagString("EXPORTNAME"); }

};

class NamedReexportSEXP : public IridiumSEXP {
private:

  NamedReexportSEXP() { this->tag = "NamedReexport"; }
  
public:
  static std::shared_ptr<NamedReexportSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "NamedReexport");
    auto res = std::shared_ptr<NamedReexportSEXP>(new NamedReexportSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  NamedReexportSEXP(std::string EXPORTNAME, double MODULEREQIDX) {
    this->tag = "NamedReexport";
    this->setEXPORTNAME(EXPORTNAME);
    this->setMODULEREQIDX(MODULEREQIDX);
  }


  void setEXPORTNAME(const std::string &value) { setFlag("EXPORTNAME", value); }
  void unsetEXPORTNAME() { removeFlag("EXPORTNAME"); }
  bool hasEXPORTNAME() { return hasFlag("EXPORTNAME"); }
  std::string getEXPORTNAME() { return getFlagString("EXPORTNAME"); }


  void setMODULEREQIDX(double value) { setFlag("MODULEREQIDX", value); }
  void unsetMODULEREQIDX() { removeFlag("MODULEREQIDX"); }
  bool hasMODULEREQIDX() { return hasFlag("MODULEREQIDX"); }
  double getMODULEREQIDX() { return getFlagDouble("MODULEREQIDX"); }

};

class ModuleRequestSEXP : public IridiumSEXP {
private:

  ModuleRequestSEXP() { this->tag = "ModuleRequest"; }
  
public:
  static std::shared_ptr<ModuleRequestSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "ModuleRequest");
    auto res = std::shared_ptr<ModuleRequestSEXP>(new ModuleRequestSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  ModuleRequestSEXP(std::string SOURCE, double REQIDX) {
    this->tag = "ModuleRequest";
    this->setSOURCE(SOURCE);
    this->setREQIDX(REQIDX);
  }


  void setSOURCE(const std::string &value) { setFlag("SOURCE", value); }
  void unsetSOURCE() { removeFlag("SOURCE"); }
  bool hasSOURCE() { return hasFlag("SOURCE"); }
  std::string getSOURCE() { return getFlagString("SOURCE"); }


  void setREQIDX(double value) { setFlag("REQIDX", value); }
  void unsetREQIDX() { removeFlag("REQIDX"); }
  bool hasREQIDX() { return hasFlag("REQIDX"); }
  double getREQIDX() { return getFlagDouble("REQIDX"); }

};

class EnvBindingSEXP : public IridiumSEXP {
private:

  EnvBindingSEXP() { this->tag = "EnvBinding"; }
  
public:
  static std::shared_ptr<EnvBindingSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "EnvBinding");
    auto res = std::shared_ptr<EnvBindingSEXP>(new EnvBindingSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  EnvBindingSEXP(std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT) {
    this->tag = "EnvBinding";
    this->setNAME(NAME);
    if (ASW) this->setASW();
    if (JSARG) this->setJSARG();
    if (JSRESTARG) this->setJSRESTARG();
    if (JSLET) this->setJSLET();
    if (JSCONST) this->setJSCONST();
    if (JSVAR) this->setJSVAR();
    this->setIDX(IDX);
    this->setREFIDX(REFIDX);
    this->setScope(Scope);
    this->setParentScope(ParentScope);
    this->setNEXT(NEXT);
  }


  void setNAME(const std::string &value) { setFlag("NAME", value); }
  void unsetNAME() { removeFlag("NAME"); }
  bool hasNAME() { return hasFlag("NAME"); }
  std::string getNAME() { return getFlagString("NAME"); }


  void setASW() { setFlag("ASW"); }
  void unsetASW() { removeFlag("ASW"); }
  bool hasASW() { return hasFlag("ASW"); }


  void setJSARG() { setFlag("JSARG"); }
  void unsetJSARG() { removeFlag("JSARG"); }
  bool hasJSARG() { return hasFlag("JSARG"); }


  void setJSRESTARG() { setFlag("JSRESTARG"); }
  void unsetJSRESTARG() { removeFlag("JSRESTARG"); }
  bool hasJSRESTARG() { return hasFlag("JSRESTARG"); }


  void setJSLET() { setFlag("JSLET"); }
  void unsetJSLET() { removeFlag("JSLET"); }
  bool hasJSLET() { return hasFlag("JSLET"); }


  void setJSCONST() { setFlag("JSCONST"); }
  void unsetJSCONST() { removeFlag("JSCONST"); }
  bool hasJSCONST() { return hasFlag("JSCONST"); }


  void setJSVAR() { setFlag("JSVAR"); }
  void unsetJSVAR() { removeFlag("JSVAR"); }
  bool hasJSVAR() { return hasFlag("JSVAR"); }


  void setIDX(double value) { setFlag("IDX", value); }
  void unsetIDX() { removeFlag("IDX"); }
  bool hasIDX() { return hasFlag("IDX"); }
  double getIDX() { return getFlagDouble("IDX"); }


  void setREFIDX(double value) { setFlag("REFIDX", value); }
  void unsetREFIDX() { removeFlag("REFIDX"); }
  bool hasREFIDX() { return hasFlag("REFIDX"); }
  double getREFIDX() { return getFlagDouble("REFIDX"); }


  void setScope(double value) { setFlag("Scope", value); }
  void unsetScope() { removeFlag("Scope"); }
  bool hasScope() { return hasFlag("Scope"); }
  double getScope() { return getFlagDouble("Scope"); }


  void setParentScope(double value) { setFlag("ParentScope", value); }
  void unsetParentScope() { removeFlag("ParentScope"); }
  bool hasParentScope() { return hasFlag("ParentScope"); }
  double getParentScope() { return getFlagDouble("ParentScope"); }


  void setNEXT(double value) { setFlag("NEXT", value); }
  void unsetNEXT() { removeFlag("NEXT"); }
  bool hasNEXT() { return hasFlag("NEXT"); }
  double getNEXT() { return getFlagDouble("NEXT"); }

};

class RemoteEnvBindingSEXP : public IridiumSEXP {
private:

  RemoteEnvBindingSEXP() { this->tag = "RemoteEnvBinding"; }
  
public:
  static std::shared_ptr<RemoteEnvBindingSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "RemoteEnvBinding");
    auto res = std::shared_ptr<RemoteEnvBindingSEXP>(new RemoteEnvBindingSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  RemoteEnvBindingSEXP(IRISEXP ParentReference, bool NSIMPORT, double REFIDX) {
    this->tag = "RemoteEnvBinding";
    this->args.push_back(ParentReference);
    if (NSIMPORT) this->setNSIMPORT();
    this->setREFIDX(REFIDX);
  }


  void setParentReference(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasParentReference() { return 0 < this->args.size(); }
  IRISEXP getParentReference() const { return this->args.at(0); }


  void setNSIMPORT() { setFlag("NSIMPORT"); }
  void unsetNSIMPORT() { removeFlag("NSIMPORT"); }
  bool hasNSIMPORT() { return hasFlag("NSIMPORT"); }


  void setREFIDX(double value) { setFlag("REFIDX", value); }
  void unsetREFIDX() { removeFlag("REFIDX"); }
  bool hasREFIDX() { return hasFlag("REFIDX"); }
  double getREFIDX() { return getFlagDouble("REFIDX"); }

};

class GlobalBindingSEXP : public IridiumSEXP {
private:

  GlobalBindingSEXP() { this->tag = "GlobalBinding"; }
  
public:
  static std::shared_ptr<GlobalBindingSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "GlobalBinding");
    auto res = std::shared_ptr<GlobalBindingSEXP>(new GlobalBindingSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  GlobalBindingSEXP(std::string NAME) {
    this->tag = "GlobalBinding";
    this->setNAME(NAME);
  }


  void setNAME(const std::string &value) { setFlag("NAME", value); }
  void unsetNAME() { removeFlag("NAME"); }
  bool hasNAME() { return hasFlag("NAME"); }
  std::string getNAME() { return getFlagString("NAME"); }

};

class EnvWriteSEXP : public IridiumSEXP {
private:

  EnvWriteSEXP() { this->tag = "EnvWrite"; }
  
public:
  static std::shared_ptr<EnvWriteSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "EnvWrite");
    auto res = std::shared_ptr<EnvWriteSEXP>(new EnvWriteSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  EnvWriteSEXP(IRISEXP LValTarget, IRISEXP RVal, bool SLOPPY, bool SAFE, bool THISINIT) {
    this->tag = "EnvWrite";
    this->args.push_back(LValTarget);
    this->args.push_back(RVal);
    if (SLOPPY) this->setSLOPPY();
    this->setSAFE(SAFE);
    this->setTHISINIT(THISINIT);
  }


  void setLValTarget(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasLValTarget() { return 0 < this->args.size(); }
  IRISEXP getLValTarget() const { return this->args.at(0); }


  void setRVal(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasRVal() { return 1 < this->args.size(); }
  IRISEXP getRVal() const { return this->args.at(1); }


  void setSLOPPY() { setFlag("SLOPPY"); }
  void unsetSLOPPY() { removeFlag("SLOPPY"); }
  bool hasSLOPPY() { return hasFlag("SLOPPY"); }


  void setSAFE(bool value) { setFlag("SAFE", value); }
  void unsetSAFE() { removeFlag("SAFE"); }
  bool hasSAFE() { return hasFlag("SAFE"); }
  bool getSAFE() { return getFlagBoolean("SAFE"); }


  void setTHISINIT(bool value) { setFlag("THISINIT", value); }
  void unsetTHISINIT() { removeFlag("THISINIT"); }
  bool hasTHISINIT() { return hasFlag("THISINIT"); }
  bool getTHISINIT() { return getFlagBoolean("THISINIT"); }

};

class JSNUBDSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<JSNUBDSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSNUBD");
    auto res = std::shared_ptr<JSNUBDSEXP>(new JSNUBDSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSNUBDSEXP() {
    this->tag = "JSNUBD";
  }

};

class JSSloppyDeclSEXP : public IridiumSEXP {
private:

  JSSloppyDeclSEXP() { this->tag = "JSSloppyDecl"; }
  
public:
  static std::shared_ptr<JSSloppyDeclSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSSloppyDecl");
    auto res = std::shared_ptr<JSSloppyDeclSEXP>(new JSSloppyDeclSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSSloppyDeclSEXP(std::string NAME, bool JSLET, bool JSCONST, bool JSVAR) {
    this->tag = "JSSloppyDecl";
    this->setNAME(NAME);
    if (JSLET) this->setJSLET();
    if (JSCONST) this->setJSCONST();
    if (JSVAR) this->setJSVAR();
  }


  void setNAME(const std::string &value) { setFlag("NAME", value); }
  void unsetNAME() { removeFlag("NAME"); }
  bool hasNAME() { return hasFlag("NAME"); }
  std::string getNAME() { return getFlagString("NAME"); }


  void setJSLET() { setFlag("JSLET"); }
  void unsetJSLET() { removeFlag("JSLET"); }
  bool hasJSLET() { return hasFlag("JSLET"); }


  void setJSCONST() { setFlag("JSCONST"); }
  void unsetJSCONST() { removeFlag("JSCONST"); }
  bool hasJSCONST() { return hasFlag("JSCONST"); }


  void setJSVAR() { setFlag("JSVAR"); }
  void unsetJSVAR() { removeFlag("JSVAR"); }
  bool hasJSVAR() { return hasFlag("JSVAR"); }

};

class NumberSEXP : public IridiumSEXP {
private:

  NumberSEXP() { this->tag = "Number"; }
  
public:
  static std::shared_ptr<NumberSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Number");
    auto res = std::shared_ptr<NumberSEXP>(new NumberSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  NumberSEXP(double IridiumPrimitive) {
    this->tag = "Number";
    this->setIridiumPrimitive(IridiumPrimitive);
  }


  void setIridiumPrimitive(double value) { setFlag("IridiumPrimitive", value); }
  void unsetIridiumPrimitive() { removeFlag("IridiumPrimitive"); }
  bool hasIridiumPrimitive() { return hasFlag("IridiumPrimitive"); }
  double getIridiumPrimitive() { return getFlagDouble("IridiumPrimitive"); }

};

class JSClassSEXP : public IridiumSEXP {
private:

  JSClassSEXP() { this->tag = "JSClass"; }
  
public:
  static std::shared_ptr<JSClassSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSClass");
    auto res = std::shared_ptr<JSClassSEXP>(new JSClassSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSClassSEXP(IRISEXP NAME, IRISEXP Parent, IRISEXP Constructor, IRISEXP PropInit, IRISEXP MethodList, IRISEXP StaticMethodList, IRISEXP StaticPropInit, bool Derived, bool BrandPrototype, bool BrandConstructor) {
    this->tag = "JSClass";
    this->args.push_back(NAME);
    this->args.push_back(Parent);
    this->args.push_back(Constructor);
    this->args.push_back(PropInit);
    this->args.push_back(MethodList);
    this->args.push_back(StaticMethodList);
    this->args.push_back(StaticPropInit);
    if (Derived) this->setDerived();
    if (BrandPrototype) this->setBrandPrototype();
    if (BrandConstructor) this->setBrandConstructor();
  }


  void setNAME(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasNAME() { return 0 < this->args.size(); }
  IRISEXP getNAME() const { return this->args.at(0); }


  void setParent(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasParent() { return 1 < this->args.size(); }
  IRISEXP getParent() const { return this->args.at(1); }


  void setConstructor(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasConstructor() { return 2 < this->args.size(); }
  IRISEXP getConstructor() const { return this->args.at(2); }


  void setPropInit(const IRISEXP &obj) { this->args.at(3) = obj; }
  bool hasPropInit() { return 3 < this->args.size(); }
  IRISEXP getPropInit() const { return this->args.at(3); }


  void setMethodList(const IRISEXP &obj) { this->args.at(4) = obj; }
  bool hasMethodList() { return 4 < this->args.size(); }
  IRISEXP getMethodList() const { return this->args.at(4); }


  void setStaticMethodList(const IRISEXP &obj) { this->args.at(5) = obj; }
  bool hasStaticMethodList() { return 5 < this->args.size(); }
  IRISEXP getStaticMethodList() const { return this->args.at(5); }


  void setStaticPropInit(const IRISEXP &obj) { this->args.at(6) = obj; }
  bool hasStaticPropInit() { return 6 < this->args.size(); }
  IRISEXP getStaticPropInit() const { return this->args.at(6); }


  void setDerived() { setFlag("Derived"); }
  void unsetDerived() { removeFlag("Derived"); }
  bool hasDerived() { return hasFlag("Derived"); }


  void setBrandPrototype() { setFlag("BrandPrototype"); }
  void unsetBrandPrototype() { removeFlag("BrandPrototype"); }
  bool hasBrandPrototype() { return hasFlag("BrandPrototype"); }


  void setBrandConstructor() { setFlag("BrandConstructor"); }
  void unsetBrandConstructor() { removeFlag("BrandConstructor"); }
  bool hasBrandConstructor() { return hasFlag("BrandConstructor"); }

};

class JSCheckConstructorSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<JSCheckConstructorSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSCheckConstructor");
    auto res = std::shared_ptr<JSCheckConstructorSEXP>(new JSCheckConstructorSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSCheckConstructorSEXP() {
    this->tag = "JSCheckConstructor";
  }

};

class StackRejectSEXP : public IridiumSEXP {
private:

  StackRejectSEXP() { this->tag = "StackReject"; }
  
public:
  static std::shared_ptr<StackRejectSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "StackReject");
    auto res = std::shared_ptr<StackRejectSEXP>(new StackRejectSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  StackRejectSEXP(double NVAL) {
    this->tag = "StackReject";
    this->setNVAL(NVAL);
  }


  void setNVAL(double value) { setFlag("NVAL", value); }
  void unsetNVAL() { removeFlag("NVAL"); }
  bool hasNVAL() { return hasFlag("NVAL"); }
  double getNVAL() { return getFlagDouble("NVAL"); }

};

class ResolvePrivateEnvBindingSEXP : public IridiumSEXP {
private:

  ResolvePrivateEnvBindingSEXP() { this->tag = "ResolvePrivateEnvBinding"; }
  
public:
  static std::shared_ptr<ResolvePrivateEnvBindingSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "ResolvePrivateEnvBinding");
    auto res = std::shared_ptr<ResolvePrivateEnvBindingSEXP>(new ResolvePrivateEnvBindingSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  ResolvePrivateEnvBindingSEXP(std::string NAME, bool FULLY_RESOLVE) {
    this->tag = "ResolvePrivateEnvBinding";
    this->setNAME(NAME);
    if (FULLY_RESOLVE) this->setFULLY_RESOLVE();
  }


  void setNAME(const std::string &value) { setFlag("NAME", value); }
  void unsetNAME() { removeFlag("NAME"); }
  bool hasNAME() { return hasFlag("NAME"); }
  std::string getNAME() { return getFlagString("NAME"); }


  void setFULLY_RESOLVE() { setFlag("FULLY_RESOLVE"); }
  void unsetFULLY_RESOLVE() { removeFlag("FULLY_RESOLVE"); }
  bool hasFULLY_RESOLVE() { return hasFlag("FULLY_RESOLVE"); }

};

class PVTEnvReadSEXP : public IridiumSEXP {
private:

  PVTEnvReadSEXP() { this->tag = "PVTEnvRead"; }
  
public:
  static std::shared_ptr<PVTEnvReadSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "PVTEnvRead");
    auto res = std::shared_ptr<PVTEnvReadSEXP>(new PVTEnvReadSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  PVTEnvReadSEXP(IRISEXP Obj, bool SYMBOL, bool METHOD, bool FULLY_RESOLVE) {
    this->tag = "PVTEnvRead";
    this->args.push_back(Obj);
    if (SYMBOL) this->setSYMBOL();
    if (METHOD) this->setMETHOD();
    if (FULLY_RESOLVE) this->setFULLY_RESOLVE();
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }


  void setSYMBOL() { setFlag("SYMBOL"); }
  void unsetSYMBOL() { removeFlag("SYMBOL"); }
  bool hasSYMBOL() { return hasFlag("SYMBOL"); }


  void setMETHOD() { setFlag("METHOD"); }
  void unsetMETHOD() { removeFlag("METHOD"); }
  bool hasMETHOD() { return hasFlag("METHOD"); }


  void setFULLY_RESOLVE() { setFlag("FULLY_RESOLVE"); }
  void unsetFULLY_RESOLVE() { removeFlag("FULLY_RESOLVE"); }
  bool hasFULLY_RESOLVE() { return hasFlag("FULLY_RESOLVE"); }

};

class JSPrivateSEXP : public IridiumSEXP {
private:

  JSPrivateSEXP() { this->tag = "JSPrivate"; }
  
public:
  static std::shared_ptr<JSPrivateSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSPrivate");
    auto res = std::shared_ptr<JSPrivateSEXP>(new JSPrivateSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSPrivateSEXP(std::string IridiumPrimitive) {
    this->tag = "JSPrivate";
    this->setIridiumPrimitive(IridiumPrimitive);
  }


  void setIridiumPrimitive(const std::string &value) { setFlag("IridiumPrimitive", value); }
  void unsetIridiumPrimitive() { removeFlag("IridiumPrimitive"); }
  bool hasIridiumPrimitive() { return hasFlag("IridiumPrimitive"); }
  std::string getIridiumPrimitive() { return getFlagString("IridiumPrimitive"); }

};

class JSPrivateFieldWriteSEXP : public IridiumSEXP {
private:

  JSPrivateFieldWriteSEXP() { this->tag = "JSPrivateFieldWrite"; }
  
public:
  static std::shared_ptr<JSPrivateFieldWriteSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSPrivateFieldWrite");
    auto res = std::shared_ptr<JSPrivateFieldWriteSEXP>(new JSPrivateFieldWriteSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSPrivateFieldWriteSEXP(IRISEXP Obj, IRISEXP Field, IRISEXP Value, bool DECL) {
    this->tag = "JSPrivateFieldWrite";
    this->args.push_back(Obj);
    this->args.push_back(Field);
    this->args.push_back(Value);
    if (DECL) this->setDECL();
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }


  void setField(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasField() { return 1 < this->args.size(); }
  IRISEXP getField() const { return this->args.at(1); }


  void setValue(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasValue() { return 2 < this->args.size(); }
  IRISEXP getValue() const { return this->args.at(2); }


  void setDECL() { setFlag("DECL"); }
  void unsetDECL() { removeFlag("DECL"); }
  bool hasDECL() { return hasFlag("DECL"); }

};

class JSADDBRANDSEXP : public IridiumSEXP {
private:

  JSADDBRANDSEXP() { this->tag = "JSADDBRAND"; }
  
public:
  static std::shared_ptr<JSADDBRANDSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSADDBRAND");
    auto res = std::shared_ptr<JSADDBRANDSEXP>(new JSADDBRANDSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSADDBRANDSEXP(IRISEXP Obj, IRISEXP HomeObj) {
    this->tag = "JSADDBRAND";
    this->args.push_back(Obj);
    this->args.push_back(HomeObj);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }


  void setHomeObj(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasHomeObj() { return 1 < this->args.size(); }
  IRISEXP getHomeObj() const { return this->args.at(1); }

};

class JSPrivateFieldReadSEXP : public IridiumSEXP {
private:

  JSPrivateFieldReadSEXP() { this->tag = "JSPrivateFieldRead"; }
  
public:
  static std::shared_ptr<JSPrivateFieldReadSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSPrivateFieldRead");
    auto res = std::shared_ptr<JSPrivateFieldReadSEXP>(new JSPrivateFieldReadSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSPrivateFieldReadSEXP(IRISEXP Obj, IRISEXP Field) {
    this->tag = "JSPrivateFieldRead";
    this->args.push_back(Obj);
    this->args.push_back(Field);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }


  void setField(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasField() { return 1 < this->args.size(); }
  IRISEXP getField() const { return this->args.at(1); }

};

class PoolBindingSEXP : public IridiumSEXP {
private:

  PoolBindingSEXP() { this->tag = "PoolBinding"; }
  
public:
  static std::shared_ptr<PoolBindingSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "PoolBinding");
    auto res = std::shared_ptr<PoolBindingSEXP>(new PoolBindingSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  PoolBindingSEXP(IRISEXP Lambda, double StartBBIDX, double REFIDX) {
    this->tag = "PoolBinding";
    this->args.push_back(Lambda);
    this->setStartBBIDX(StartBBIDX);
    this->setREFIDX(REFIDX);
  }


  void setLambda(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasLambda() { return 0 < this->args.size(); }
  IRISEXP getLambda() const { return this->args.at(0); }


  void setStartBBIDX(double value) { setFlag("StartBBIDX", value); }
  void unsetStartBBIDX() { removeFlag("StartBBIDX"); }
  bool hasStartBBIDX() { return hasFlag("StartBBIDX"); }
  double getStartBBIDX() { return getFlagDouble("StartBBIDX"); }


  void setREFIDX(double value) { setFlag("REFIDX", value); }
  void unsetREFIDX() { removeFlag("REFIDX"); }
  bool hasREFIDX() { return hasFlag("REFIDX"); }
  double getREFIDX() { return getFlagDouble("REFIDX"); }

};

class ResolveContinueTargetSEXP : public IridiumSEXP {
private:

  ResolveContinueTargetSEXP() { this->tag = "ResolveContinueTarget"; }
  
public:
  static std::shared_ptr<ResolveContinueTargetSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "ResolveContinueTarget");
    auto res = std::shared_ptr<ResolveContinueTargetSEXP>(new ResolveContinueTargetSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  ResolveContinueTargetSEXP(std::string Label) {
    this->tag = "ResolveContinueTarget";
    this->setLabel(Label);
  }


  void setLabel(const std::string &value) { setFlag("Label", value); }
  void unsetLabel() { removeFlag("Label"); }
  bool hasLabel() { return hasFlag("Label"); }
  std::string getLabel() { return getFlagString("Label"); }

};

class ResolveBreakTargetSEXP : public IridiumSEXP {
private:

  ResolveBreakTargetSEXP() { this->tag = "ResolveBreakTarget"; }
  
public:
  static std::shared_ptr<ResolveBreakTargetSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "ResolveBreakTarget");
    auto res = std::shared_ptr<ResolveBreakTargetSEXP>(new ResolveBreakTargetSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  ResolveBreakTargetSEXP(std::string Label) {
    this->tag = "ResolveBreakTarget";
    this->setLabel(Label);
  }


  void setLabel(const std::string &value) { setFlag("Label", value); }
  void unsetLabel() { removeFlag("Label"); }
  bool hasLabel() { return hasFlag("Label"); }
  std::string getLabel() { return getFlagString("Label"); }

};

class JSForOfIteratorCloseSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<JSForOfIteratorCloseSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSForOfIteratorClose");
    auto res = std::shared_ptr<JSForOfIteratorCloseSEXP>(new JSForOfIteratorCloseSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSForOfIteratorCloseSEXP() {
    this->tag = "JSForOfIteratorClose";
  }

};

class PopCatchContextSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<PopCatchContextSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "PopCatchContext");
    auto res = std::shared_ptr<PopCatchContextSEXP>(new PopCatchContextSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  PopCatchContextSEXP() {
    this->tag = "PopCatchContext";
  }

};

class InvokeFinalizerSEXP : public IridiumSEXP {
private:

  InvokeFinalizerSEXP() { this->tag = "InvokeFinalizer"; }
  
public:
  static std::shared_ptr<InvokeFinalizerSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "InvokeFinalizer");
    auto res = std::shared_ptr<InvokeFinalizerSEXP>(new InvokeFinalizerSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  InvokeFinalizerSEXP(double IDX) {
    this->tag = "InvokeFinalizer";
    this->setIDX(IDX);
  }


  void setIDX(double value) { setFlag("IDX", value); }
  void unsetIDX() { removeFlag("IDX"); }
  bool hasIDX() { return hasFlag("IDX"); }
  double getIDX() { return getFlagDouble("IDX"); }

};

class JSForInStartSEXP : public IridiumSEXP {
private:

  JSForInStartSEXP() { this->tag = "JSForInStart"; }
  
public:
  static std::shared_ptr<JSForInStartSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSForInStart");
    auto res = std::shared_ptr<JSForInStartSEXP>(new JSForInStartSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSForInStartSEXP(IRISEXP Obj) {
    this->tag = "JSForInStart";
    this->args.push_back(Obj);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }

};

class JSForInNextSEXP : public IridiumSEXP {
private:

  JSForInNextSEXP() { this->tag = "JSForInNext"; }
  
public:
  static std::shared_ptr<JSForInNextSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSForInNext");
    auto res = std::shared_ptr<JSForInNextSEXP>(new JSForInNextSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSForInNextSEXP(IRISEXP IteratorObj) {
    this->tag = "JSForInNext";
    this->args.push_back(IteratorObj);
  }


  void setIteratorObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasIteratorObj() { return 0 < this->args.size(); }
  IRISEXP getIteratorObj() const { return this->args.at(0); }

};

class StackRetainSEXP : public IridiumSEXP {
private:

  StackRetainSEXP() { this->tag = "StackRetain"; }
  
public:
  static std::shared_ptr<StackRetainSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "StackRetain");
    auto res = std::shared_ptr<StackRetainSEXP>(new StackRetainSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  StackRetainSEXP(double NVAL, double NIP) {
    this->tag = "StackRetain";
    this->setNVAL(NVAL);
    this->setNIP(NIP);
  }


  void setNVAL(double value) { setFlag("NVAL", value); }
  void unsetNVAL() { removeFlag("NVAL"); }
  bool hasNVAL() { return hasFlag("NVAL"); }
  double getNVAL() { return getFlagDouble("NVAL"); }


  void setNIP(double value) { setFlag("NIP", value); }
  void unsetNIP() { removeFlag("NIP"); }
  bool hasNIP() { return hasFlag("NIP"); }
  double getNIP() { return getFlagDouble("NIP"); }

};

class StackPopSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<StackPopSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "StackPop");
    auto res = std::shared_ptr<StackPopSEXP>(new StackPopSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  StackPopSEXP() {
    this->tag = "StackPop";
  }

};

class JSForOfStartSEXP : public IridiumSEXP {
private:

  JSForOfStartSEXP() { this->tag = "JSForOfStart"; }
  
public:
  static std::shared_ptr<JSForOfStartSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSForOfStart");
    auto res = std::shared_ptr<JSForOfStartSEXP>(new JSForOfStartSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSForOfStartSEXP(IRISEXP Obj) {
    this->tag = "JSForOfStart";
    this->args.push_back(Obj);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }

};

class JSForOfNextSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<JSForOfNextSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSForOfNext");
    auto res = std::shared_ptr<JSForOfNextSEXP>(new JSForOfNextSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSForOfNextSEXP() {
    this->tag = "JSForOfNext";
  }

};

class PushCatchContextSEXP : public IridiumSEXP {
private:

  PushCatchContextSEXP() { this->tag = "PushCatchContext"; }
  
public:
  static std::shared_ptr<PushCatchContextSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "PushCatchContext");
    auto res = std::shared_ptr<PushCatchContextSEXP>(new PushCatchContextSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  PushCatchContextSEXP(double IDX) {
    this->tag = "PushCatchContext";
    this->setIDX(IDX);
  }


  void setIDX(double value) { setFlag("IDX", value); }
  void unsetIDX() { removeFlag("IDX"); }
  bool hasIDX() { return hasFlag("IDX"); }
  double getIDX() { return getFlagDouble("IDX"); }

};

class JSCatchContextSEXP : public IridiumSEXP {
private:

  JSCatchContextSEXP() { this->tag = "JSCatchContext"; }
  
public:
  static std::shared_ptr<JSCatchContextSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSCatchContext");
    auto res = std::shared_ptr<JSCatchContextSEXP>(new JSCatchContextSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSCatchContextSEXP(std::string NAME) {
    this->tag = "JSCatchContext";
    this->setNAME(NAME);
  }


  void setNAME(const std::string &value) { setFlag("NAME", value); }
  void unsetNAME() { removeFlag("NAME"); }
  bool hasNAME() { return hasFlag("NAME"); }
  std::string getNAME() { return getFlagString("NAME"); }

};

class ThrowSEXP : public IridiumSEXP {
private:

  ThrowSEXP() { this->tag = "Throw"; }
  
public:
  static std::shared_ptr<ThrowSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Throw");
    auto res = std::shared_ptr<ThrowSEXP>(new ThrowSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  ThrowSEXP(IRISEXP ThrowVal) {
    this->tag = "Throw";
    this->args.push_back(ThrowVal);
  }


  void setThrowVal(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasThrowVal() { return 0 < this->args.size(); }
  IRISEXP getThrowVal() const { return this->args.at(0); }

};

class RetSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<RetSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Ret");
    auto res = std::shared_ptr<RetSEXP>(new RetSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  RetSEXP() {
    this->tag = "Ret";
  }

};

class JSBinopSEXP : public IridiumSEXP {
private:

  JSBinopSEXP() { this->tag = "JSBinop"; }
  
public:
  static std::shared_ptr<JSBinopSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSBinop");
    auto res = std::shared_ptr<JSBinopSEXP>(new JSBinopSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSBinopSEXP(IRISEXP LBinop, IRISEXP RBinop, std::string OP) {
    this->tag = "JSBinop";
    this->args.push_back(LBinop);
    this->args.push_back(RBinop);
    this->setOP(OP);
  }


  void setLBinop(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasLBinop() { return 0 < this->args.size(); }
  IRISEXP getLBinop() const { return this->args.at(0); }


  void setRBinop(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasRBinop() { return 1 < this->args.size(); }
  IRISEXP getRBinop() const { return this->args.at(1); }


  void setOP(const std::string &value) { setFlag("OP", value); }
  void unsetOP() { removeFlag("OP"); }
  bool hasOP() { return hasFlag("OP"); }
  std::string getOP() { return getFlagString("OP"); }

};

class FieldWriteSEXP : public IridiumSEXP {
private:

  FieldWriteSEXP() { this->tag = "FieldWrite"; }
  
public:
  static std::shared_ptr<FieldWriteSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "FieldWrite");
    auto res = std::shared_ptr<FieldWriteSEXP>(new FieldWriteSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  FieldWriteSEXP(IRISEXP Obj, IRISEXP Field, IRISEXP Value) {
    this->tag = "FieldWrite";
    this->args.push_back(Obj);
    this->args.push_back(Field);
    this->args.push_back(Value);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }


  void setField(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasField() { return 1 < this->args.size(); }
  IRISEXP getField() const { return this->args.at(1); }


  void setValue(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasValue() { return 2 < this->args.size(); }
  IRISEXP getValue() const { return this->args.at(2); }

};

class JSUnopSEXP : public IridiumSEXP {
private:

  JSUnopSEXP() { this->tag = "JSUnop"; }
  
public:
  static std::shared_ptr<JSUnopSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSUnop");
    auto res = std::shared_ptr<JSUnopSEXP>(new JSUnopSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSUnopSEXP(IRISEXP Val, std::string OP) {
    this->tag = "JSUnop";
    this->args.push_back(Val);
    this->setOP(OP);
  }


  void setVal(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasVal() { return 0 < this->args.size(); }
  IRISEXP getVal() const { return this->args.at(0); }


  void setOP(const std::string &value) { setFlag("OP", value); }
  void unsetOP() { removeFlag("OP"); }
  bool hasOP() { return hasFlag("OP"); }
  std::string getOP() { return getFlagString("OP"); }

};

class UnopSEXP : public IridiumSEXP {
private:

  UnopSEXP() { this->tag = "Unop"; }
  
public:
  static std::shared_ptr<UnopSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Unop");
    auto res = std::shared_ptr<UnopSEXP>(new UnopSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  UnopSEXP(IRISEXP Val, std::string OP) {
    this->tag = "Unop";
    this->args.push_back(Val);
    this->setOP(OP);
  }


  void setVal(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasVal() { return 0 < this->args.size(); }
  IRISEXP getVal() const { return this->args.at(0); }


  void setOP(const std::string &value) { setFlag("OP", value); }
  void unsetOP() { removeFlag("OP"); }
  bool hasOP() { return hasFlag("OP"); }
  std::string getOP() { return getFlagString("OP"); }

};

class JSObjectSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<JSObjectSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSObject");
    auto res = std::shared_ptr<JSObjectSEXP>(new JSObjectSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSObjectSEXP() {
    this->tag = "JSObject";
  }

};

class BooleanSEXP : public IridiumSEXP {
private:

  BooleanSEXP() { this->tag = "Boolean"; }
  
public:
  static std::shared_ptr<BooleanSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Boolean");
    auto res = std::shared_ptr<BooleanSEXP>(new BooleanSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  BooleanSEXP(bool IridiumPrimitive) {
    this->tag = "Boolean";
    this->setIridiumPrimitive(IridiumPrimitive);
  }


  void setIridiumPrimitive(bool value) { setFlag("IridiumPrimitive", value); }
  void unsetIridiumPrimitive() { removeFlag("IridiumPrimitive"); }
  bool hasIridiumPrimitive() { return hasFlag("IridiumPrimitive"); }
  bool getIridiumPrimitive() { return getFlagBoolean("IridiumPrimitive"); }

};

class JSDefineObjPropSEXP : public IridiumSEXP {
private:

  JSDefineObjPropSEXP() { this->tag = "JSDefineObjProp"; }
  
public:
  static std::shared_ptr<JSDefineObjPropSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSDefineObjProp");
    auto res = std::shared_ptr<JSDefineObjPropSEXP>(new JSDefineObjPropSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSDefineObjPropSEXP(IRISEXP TargetObj, IRISEXP Key, IRISEXP Value) {
    this->tag = "JSDefineObjProp";
    this->args.push_back(TargetObj);
    this->args.push_back(Key);
    this->args.push_back(Value);
  }


  void setTargetObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasTargetObj() { return 0 < this->args.size(); }
  IRISEXP getTargetObj() const { return this->args.at(0); }


  void setKey(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasKey() { return 1 < this->args.size(); }
  IRISEXP getKey() const { return this->args.at(1); }


  void setValue(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasValue() { return 2 < this->args.size(); }
  IRISEXP getValue() const { return this->args.at(2); }

};

class JSArraySEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<JSArraySEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSArray");
    auto res = std::shared_ptr<JSArraySEXP>(new JSArraySEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSArraySEXP() {
    this->tag = "JSArray";
  }

};

class BinopSEXP : public IridiumSEXP {
private:

  BinopSEXP() { this->tag = "Binop"; }
  
public:
  static std::shared_ptr<BinopSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Binop");
    auto res = std::shared_ptr<BinopSEXP>(new BinopSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  BinopSEXP(IRISEXP LBinop, IRISEXP RBinop, std::string OP) {
    this->tag = "Binop";
    this->args.push_back(LBinop);
    this->args.push_back(RBinop);
    this->setOP(OP);
  }


  void setLBinop(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasLBinop() { return 0 < this->args.size(); }
  IRISEXP getLBinop() const { return this->args.at(0); }


  void setRBinop(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasRBinop() { return 1 < this->args.size(); }
  IRISEXP getRBinop() const { return this->args.at(1); }


  void setOP(const std::string &value) { setFlag("OP", value); }
  void unsetOP() { removeFlag("OP"); }
  bool hasOP() { return hasFlag("OP"); }
  std::string getOP() { return getFlagString("OP"); }

};

class NullSEXP : public IridiumSEXP {
private:

  NullSEXP() { this->tag = "Null"; }
  
public:
  static std::shared_ptr<NullSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Null");
    auto res = std::shared_ptr<NullSEXP>(new NullSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  NullSEXP(bool IridiumPrimitive) {
    this->tag = "Null";
    if (IridiumPrimitive) this->setIridiumPrimitive();
  }


  void setIridiumPrimitive() { setFlag("IridiumPrimitive"); }
  void unsetIridiumPrimitive() { removeFlag("IridiumPrimitive"); }
  bool hasIridiumPrimitive() { return hasFlag("IridiumPrimitive"); }

};

class JSComputedFieldReadSEXP : public IridiumSEXP {
private:

  JSComputedFieldReadSEXP() { this->tag = "JSComputedFieldRead"; }
  
public:
  static std::shared_ptr<JSComputedFieldReadSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSComputedFieldRead");
    auto res = std::shared_ptr<JSComputedFieldReadSEXP>(new JSComputedFieldReadSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSComputedFieldReadSEXP(IRISEXP Obj, IRISEXP Field) {
    this->tag = "JSComputedFieldRead";
    this->args.push_back(Obj);
    this->args.push_back(Field);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }


  void setField(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasField() { return 1 < this->args.size(); }
  IRISEXP getField() const { return this->args.at(1); }

};

class JSComputedFieldWriteSEXP : public IridiumSEXP {
private:

  JSComputedFieldWriteSEXP() { this->tag = "JSComputedFieldWrite"; }
  
public:
  static std::shared_ptr<JSComputedFieldWriteSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSComputedFieldWrite");
    auto res = std::shared_ptr<JSComputedFieldWriteSEXP>(new JSComputedFieldWriteSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSComputedFieldWriteSEXP(IRISEXP Obj, IRISEXP Field, IRISEXP Value) {
    this->tag = "JSComputedFieldWrite";
    this->args.push_back(Obj);
    this->args.push_back(Field);
    this->args.push_back(Value);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }


  void setField(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasField() { return 1 < this->args.size(); }
  IRISEXP getField() const { return this->args.at(1); }


  void setValue(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasValue() { return 2 < this->args.size(); }
  IRISEXP getValue() const { return this->args.at(2); }

};

class JSSuperFieldReadSEXP : public IridiumSEXP {
private:

  JSSuperFieldReadSEXP() { this->tag = "JSSuperFieldRead"; }
  
public:
  static std::shared_ptr<JSSuperFieldReadSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSSuperFieldRead");
    auto res = std::shared_ptr<JSSuperFieldReadSEXP>(new JSSuperFieldReadSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSSuperFieldReadSEXP(IRISEXP This, IRISEXP Super, IRISEXP Field) {
    this->tag = "JSSuperFieldRead";
    this->args.push_back(This);
    this->args.push_back(Super);
    this->args.push_back(Field);
  }


  void setThis(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasThis() { return 0 < this->args.size(); }
  IRISEXP getThis() const { return this->args.at(0); }


  void setSuper(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasSuper() { return 1 < this->args.size(); }
  IRISEXP getSuper() const { return this->args.at(1); }


  void setField(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasField() { return 2 < this->args.size(); }
  IRISEXP getField() const { return this->args.at(2); }

};

class JSSuperFieldWriteSEXP : public IridiumSEXP {
private:

  JSSuperFieldWriteSEXP() { this->tag = "JSSuperFieldWrite"; }
  
public:
  static std::shared_ptr<JSSuperFieldWriteSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSSuperFieldWrite");
    auto res = std::shared_ptr<JSSuperFieldWriteSEXP>(new JSSuperFieldWriteSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSSuperFieldWriteSEXP(IRISEXP This, IRISEXP Super, IRISEXP Field, IRISEXP Value) {
    this->tag = "JSSuperFieldWrite";
    this->args.push_back(This);
    this->args.push_back(Super);
    this->args.push_back(Field);
    this->args.push_back(Value);
  }


  void setThis(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasThis() { return 0 < this->args.size(); }
  IRISEXP getThis() const { return this->args.at(0); }


  void setSuper(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasSuper() { return 1 < this->args.size(); }
  IRISEXP getSuper() const { return this->args.at(1); }


  void setField(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasField() { return 2 < this->args.size(); }
  IRISEXP getField() const { return this->args.at(2); }


  void setValue(const IRISEXP &obj) { this->args.at(3) = obj; }
  bool hasValue() { return 3 < this->args.size(); }
  IRISEXP getValue() const { return this->args.at(3); }

};

class JSToObjectSEXP : public IridiumSEXP {
private:

  JSToObjectSEXP() { this->tag = "JSToObject"; }
  
public:
  static std::shared_ptr<JSToObjectSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSToObject");
    auto res = std::shared_ptr<JSToObjectSEXP>(new JSToObjectSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSToObjectSEXP(IRISEXP TargetObj) {
    this->tag = "JSToObject";
    this->args.push_back(TargetObj);
  }


  void setTargetObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasTargetObj() { return 0 < this->args.size(); }
  IRISEXP getTargetObj() const { return this->args.at(0); }

};

class JSAppendSEXP : public IridiumSEXP {
private:

  JSAppendSEXP() { this->tag = "JSAppend"; }
  
public:
  static std::shared_ptr<JSAppendSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSAppend");
    auto res = std::shared_ptr<JSAppendSEXP>(new JSAppendSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSAppendSEXP(IRISEXP TargetObj, IRISEXP InsertionIdx, IRISEXP SpreadObj) {
    this->tag = "JSAppend";
    this->args.push_back(TargetObj);
    this->args.push_back(InsertionIdx);
    this->args.push_back(SpreadObj);
  }


  void setTargetObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasTargetObj() { return 0 < this->args.size(); }
  IRISEXP getTargetObj() const { return this->args.at(0); }


  void setInsertionIdx(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasInsertionIdx() { return 1 < this->args.size(); }
  IRISEXP getInsertionIdx() const { return this->args.at(1); }


  void setSpreadObj(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasSpreadObj() { return 2 < this->args.size(); }
  IRISEXP getSpreadObj() const { return this->args.at(2); }

};

class JSDefineObjMethodSEXP : public IridiumSEXP {
private:

  JSDefineObjMethodSEXP() { this->tag = "JSDefineObjMethod"; }
  
public:
  static std::shared_ptr<JSDefineObjMethodSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSDefineObjMethod");
    auto res = std::shared_ptr<JSDefineObjMethodSEXP>(new JSDefineObjMethodSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSDefineObjMethodSEXP(IRISEXP TargetObj, IRISEXP Key, IRISEXP Value) {
    this->tag = "JSDefineObjMethod";
    this->args.push_back(TargetObj);
    this->args.push_back(Key);
    this->args.push_back(Value);
  }


  void setTargetObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasTargetObj() { return 0 < this->args.size(); }
  IRISEXP getTargetObj() const { return this->args.at(0); }


  void setKey(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasKey() { return 1 < this->args.size(); }
  IRISEXP getKey() const { return this->args.at(1); }


  void setValue(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasValue() { return 2 < this->args.size(); }
  IRISEXP getValue() const { return this->args.at(2); }

};

class JSCopyDataPropertiesSEXP : public IridiumSEXP {
private:

  JSCopyDataPropertiesSEXP() { this->tag = "JSCopyDataProperties"; }
  
public:
  static std::shared_ptr<JSCopyDataPropertiesSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSCopyDataProperties");
    auto res = std::shared_ptr<JSCopyDataPropertiesSEXP>(new JSCopyDataPropertiesSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSCopyDataPropertiesSEXP(IRISEXP ExclusionObj, IRISEXP SourceObj, IRISEXP TargetObj) {
    this->tag = "JSCopyDataProperties";
    this->args.push_back(ExclusionObj);
    this->args.push_back(SourceObj);
    this->args.push_back(TargetObj);
  }


  void setExclusionObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasExclusionObj() { return 0 < this->args.size(); }
  IRISEXP getExclusionObj() const { return this->args.at(0); }


  void setSourceObj(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasSourceObj() { return 1 < this->args.size(); }
  IRISEXP getSourceObj() const { return this->args.at(1); }


  void setTargetObj(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasTargetObj() { return 2 < this->args.size(); }
  IRISEXP getTargetObj() const { return this->args.at(2); }

};

class RegExpSEXP : public IridiumSEXP {
private:

  RegExpSEXP() { this->tag = "RegExp"; }
  
public:
  static std::shared_ptr<RegExpSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "RegExp");
    auto res = std::shared_ptr<RegExpSEXP>(new RegExpSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  RegExpSEXP(std::string EXP, std::string FLAGS) {
    this->tag = "RegExp";
    this->setEXP(EXP);
    this->setFLAGS(FLAGS);
  }


  void setEXP(const std::string &value) { setFlag("EXP", value); }
  void unsetEXP() { removeFlag("EXP"); }
  bool hasEXP() { return hasFlag("EXP"); }
  std::string getEXP() { return getFlagString("EXP"); }


  void setFLAGS(const std::string &value) { setFlag("FLAGS", value); }
  void unsetFLAGS() { removeFlag("FLAGS"); }
  bool hasFLAGS() { return hasFlag("FLAGS"); }
  std::string getFLAGS() { return getFlagString("FLAGS"); }

};

class UNOPDelMemberExprSEXP : public IridiumSEXP {
private:

  UNOPDelMemberExprSEXP() { this->tag = "UNOPDelMemberExpr"; }
  
public:
  static std::shared_ptr<UNOPDelMemberExprSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "UNOPDelMemberExpr");
    auto res = std::shared_ptr<UNOPDelMemberExprSEXP>(new UNOPDelMemberExprSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  UNOPDelMemberExprSEXP(IRISEXP Receiver, IRISEXP Field) {
    this->tag = "UNOPDelMemberExpr";
    this->args.push_back(Receiver);
    this->args.push_back(Field);
  }


  void setReceiver(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasReceiver() { return 0 < this->args.size(); }
  IRISEXP getReceiver() const { return this->args.at(0); }


  void setField(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasField() { return 1 < this->args.size(); }
  IRISEXP getField() const { return this->args.at(1); }

};

class UNOPDelVarSEXP : public IridiumSEXP {
private:

  UNOPDelVarSEXP() { this->tag = "UNOPDelVar"; }
  
public:
  static std::shared_ptr<UNOPDelVarSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "UNOPDelVar");
    auto res = std::shared_ptr<UNOPDelVarSEXP>(new UNOPDelVarSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  UNOPDelVarSEXP(std::string NAME) {
    this->tag = "UNOPDelVar";
    this->setNAME(NAME);
  }


  void setNAME(const std::string &value) { setFlag("NAME", value); }
  void unsetNAME() { removeFlag("NAME"); }
  bool hasNAME() { return hasFlag("NAME"); }
  std::string getNAME() { return getFlagString("NAME"); }

};

class JSTemplateSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<JSTemplateSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSTemplate");
    auto res = std::shared_ptr<JSTemplateSEXP>(new JSTemplateSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSTemplateSEXP() {
    this->tag = "JSTemplate";
  }

};

class BitIntSEXP : public IridiumSEXP {
private:

  BitIntSEXP() { this->tag = "BitInt"; }
  
public:
  static std::shared_ptr<BitIntSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "BitInt");
    auto res = std::shared_ptr<BitIntSEXP>(new BitIntSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  BitIntSEXP(std::string IridiumPrimitive) {
    this->tag = "BitInt";
    this->setIridiumPrimitive(IridiumPrimitive);
  }


  void setIridiumPrimitive(const std::string &value) { setFlag("IridiumPrimitive", value); }
  void unsetIridiumPrimitive() { removeFlag("IridiumPrimitive"); }
  bool hasIridiumPrimitive() { return hasFlag("IridiumPrimitive"); }
  std::string getIridiumPrimitive() { return getFlagString("IridiumPrimitive"); }

};

class JSSpreadSEXP : public IridiumSEXP {
private:

  JSSpreadSEXP() { this->tag = "JSSpread"; }
  
public:
  static std::shared_ptr<JSSpreadSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSSpread");
    auto res = std::shared_ptr<JSSpreadSEXP>(new JSSpreadSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSSpreadSEXP(IRISEXP Val) {
    this->tag = "JSSpread";
    this->args.push_back(Val);
  }


  void setVal(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasVal() { return 0 < this->args.size(); }
  IRISEXP getVal() const { return this->args.at(0); }

};

class AwaitSEXP : public IridiumSEXP {
private:

  AwaitSEXP() { this->tag = "Await"; }
  
public:
  static std::shared_ptr<AwaitSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Await");
    auto res = std::shared_ptr<AwaitSEXP>(new AwaitSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  AwaitSEXP(IRISEXP Obj) {
    this->tag = "Await";
    this->args.push_back(Obj);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }

};

class YieldSEXP : public IridiumSEXP {
private:

  YieldSEXP() { this->tag = "Yield"; }
  
public:
  static std::shared_ptr<YieldSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "Yield");
    auto res = std::shared_ptr<YieldSEXP>(new YieldSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  YieldSEXP(IRISEXP Obj, IRISEXP DoneTarget, IRISEXP NextValue) {
    this->tag = "Yield";
    this->args.push_back(Obj);
    this->args.push_back(DoneTarget);
    this->args.push_back(NextValue);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }


  void setDoneTarget(const IRISEXP &obj) { this->args.at(1) = obj; }
  bool hasDoneTarget() { return 1 < this->args.size(); }
  IRISEXP getDoneTarget() const { return this->args.at(1); }


  void setNextValue(const IRISEXP &obj) { this->args.at(2) = obj; }
  bool hasNextValue() { return 2 < this->args.size(); }
  IRISEXP getNextValue() const { return this->args.at(2); }

};

class JSInitialYieldSEXP : public IridiumSEXP {
private:
// default constructor and explicit one are the same, skipping...
public:
  static std::shared_ptr<JSInitialYieldSEXP> generateFrom(IRISEXP obj) {
    assert(obj->tag == "JSInitialYield");
    auto res = std::shared_ptr<JSInitialYieldSEXP>(new JSInitialYieldSEXP());

    res->tag   = obj->tag;
    res->args  = std::move(obj->args);
    res->flags = std::move(obj->flags);
    return res;
  }


  JSInitialYieldSEXP() {
    this->tag = "JSInitialYield";
  }

};
