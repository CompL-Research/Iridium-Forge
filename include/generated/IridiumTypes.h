// Generated: 2025-08-23 15:47:59
#pragma once
#include "Iridium/Globals.h"
#include "Iridium/IridiumSEXP.h"
class FileSEXP : public IridiumSEXP {
public:

  FileSEXP(IRISEXP obj) {
    assert(obj->tag == "File");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  FileSEXP(bool JSScript, bool JSModule) {
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
public:

  ResolveEnvBindingSEXP(IRISEXP obj) {
    assert(obj->tag == "ResolveEnvBinding");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  ResolveEnvBindingSEXP(std::string NAME, bool ASW) {
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
public:

  ListSEXP(IRISEXP obj) {
    assert(obj->tag == "List");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  ListSEXP(std::string TYPE) {
    this->setTYPE(TYPE);
  }


  void setTYPE(const std::string &value) { setFlag("TYPE", value); }
  void unsetTYPE() { removeFlag("TYPE"); }
  bool hasTYPE() { return hasFlag("TYPE"); }
  std::string getTYPE() { return getFlagString("TYPE"); }

};

class JSImplicitBindingDeclarationSEXP : public IridiumSEXP {
public:

  JSImplicitBindingDeclarationSEXP(IRISEXP obj) {
    assert(obj->tag == "JSImplicitBindingDeclaration");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  JSImplicitBindingDeclarationSEXP(IRISEXP Store, IRISEXP Args, std::string NAME, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SKIPINIT, bool SAFE, bool THISINIT, double OPID) {
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
public:

  EnvReadSEXP(IRISEXP obj) {
    assert(obj->tag == "EnvRead");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  EnvReadSEXP(IRISEXP Obj) {
    this->args.push_back(Obj);
  }


  void setObj(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasObj() { return 0 < this->args.size(); }
  IRISEXP getObj() const { return this->args.at(0); }

};

class IfJumpSEXP : public IridiumSEXP {
public:

  IfJumpSEXP(IRISEXP obj) {
    assert(obj->tag == "IfJump");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  IfJumpSEXP(IRISEXP Test, bool NOT, double IDX) {
    this->args.push_back(Test);
    if (NOT) this->setNOT();
    this->setIDX(IDX);
  }


  void setTest(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasTest() { return 0 < this->args.size(); }
  IRISEXP getTest() const { return this->args.at(0); }


  void setNOT() { setFlag("NOT"); }
  void unsetNOT() { removeFlag("NOT"); }
  bool hasNOT() { return hasFlag("NOT"); }


  void setIDX(double value) { setFlag("IDX", value); }
  void unsetIDX() { removeFlag("IDX"); }
  bool hasIDX() { return hasFlag("IDX"); }
  double getIDX() { return getFlagDouble("IDX"); }

};

class StringSEXP : public IridiumSEXP {
public:

  StringSEXP(IRISEXP obj) {
    assert(obj->tag == "String");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  StringSEXP(std::string IridiumPrimitive) {
    this->setIridiumPrimitive(IridiumPrimitive);
  }


  void setIridiumPrimitive(const std::string &value) { setFlag("IridiumPrimitive", value); }
  void unsetIridiumPrimitive() { removeFlag("IridiumPrimitive"); }
  bool hasIridiumPrimitive() { return hasFlag("IridiumPrimitive"); }
  std::string getIridiumPrimitive() { return getFlagString("IridiumPrimitive"); }

};

class FieldReadSEXP : public IridiumSEXP {
public:

  FieldReadSEXP(IRISEXP obj) {
    assert(obj->tag == "FieldRead");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  FieldReadSEXP(IRISEXP Obj, IRISEXP Field) {
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
public:

  JSExplicitBindingDeclarationSEXP(IRISEXP obj) {
    assert(obj->tag == "JSExplicitBindingDeclaration");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  JSExplicitBindingDeclarationSEXP(IRISEXP LValTarget, IRISEXP RVal, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SAFE, bool THISINIT) {
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
public:

  CallSiteSEXP(IRISEXP obj) {
    assert(obj->tag == "CallSite");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  CallSiteSEXP(bool CCall, bool ConstructorCall, bool PrivateCall, bool Import, bool Super, bool JSDirectEval) {
    if (CCall) this->setCCall();
    if (ConstructorCall) this->setConstructorCall();
    if (PrivateCall) this->setPrivateCall();
    if (Import) this->setImport();
    if (Super) this->setSuper();
    if (JSDirectEval) this->setJSDirectEval();
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


  void setJSDirectEval() { setFlag("JSDirectEval"); }
  void unsetJSDirectEval() { removeFlag("JSDirectEval"); }
  bool hasJSDirectEval() { return hasFlag("JSDirectEval"); }

};

class ReturnAsyncSEXP : public IridiumSEXP {
public:

  ReturnAsyncSEXP(IRISEXP obj) {
    assert(obj->tag == "ReturnAsync");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  ReturnAsyncSEXP(IRISEXP RetVal) {
    this->args.push_back(RetVal);
  }


  void setRetVal(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasRetVal() { return 0 < this->args.size(); }
  IRISEXP getRetVal() const { return this->args.at(0); }

};

class BBSEXP : public IridiumSEXP {
public:

  BBSEXP(IRISEXP obj) {
    assert(obj->tag == "BB");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  BBSEXP(bool TopLevel, bool ClosureBoundary, bool Lexical, double IDX, double ScopeIDX) {
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
public:

  ReturnSEXP(IRISEXP obj) {
    assert(obj->tag == "Return");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  ReturnSEXP(bool ModuleEarlyReturn) {
    if (ModuleEarlyReturn) this->setModuleEarlyReturn();
  }


  void setModuleEarlyReturn() { setFlag("ModuleEarlyReturn"); }
  void unsetModuleEarlyReturn() { removeFlag("ModuleEarlyReturn"); }
  bool hasModuleEarlyReturn() { return hasFlag("ModuleEarlyReturn"); }

};

class IfElseJumpSEXP : public IridiumSEXP {
public:

  IfElseJumpSEXP(IRISEXP obj) {
    assert(obj->tag == "IfElseJump");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  IfElseJumpSEXP(IRISEXP Test, double TRUE, double FALSE) {
    this->args.push_back(Test);
    this->setTRUE(TRUE);
    this->setFALSE(FALSE);
  }


  void setTest(const IRISEXP &obj) { this->args.at(0) = obj; }
  bool hasTest() { return 0 < this->args.size(); }
  IRISEXP getTest() const { return this->args.at(0); }


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
public:

  GotoSEXP(IRISEXP obj) {
    assert(obj->tag == "Goto");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  GotoSEXP(double IDX) {
    this->setIDX(IDX);
  }


  void setIDX(double value) { setFlag("IDX", value); }
  void unsetIDX() { removeFlag("IDX"); }
  bool hasIDX() { return hasFlag("IDX"); }
  double getIDX() { return getFlagDouble("IDX"); }

};

class JSFuncDeclSEXP : public IridiumSEXP {
public:

  JSFuncDeclSEXP(IRISEXP obj) {
    assert(obj->tag == "JSFuncDecl");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  JSFuncDeclSEXP(IRISEXP LValTarget, IRISEXP RVal, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SAFE, bool THISINIT) {
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
public:

  LambdaSEXP(IRISEXP obj) {
    assert(obj->tag == "Lambda");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  LambdaSEXP(double StartBBIDX) {
    this->setStartBBIDX(StartBBIDX);
  }


  void setStartBBIDX(double value) { setFlag("StartBBIDX", value); }
  void unsetStartBBIDX() { removeFlag("StartBBIDX"); }
  bool hasStartBBIDX() { return hasFlag("StartBBIDX"); }
  double getStartBBIDX() { return getFlagDouble("StartBBIDX"); }

};

class NOPSEXP : public IridiumSEXP {
public:

  NOPSEXP(IRISEXP obj) {
    assert(obj->tag == "NOP");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  NOPSEXP() {

  }

};

class BBContainerSEXP : public IridiumSEXP {
public:

  BBContainerSEXP(IRISEXP obj) {
    assert(obj->tag == "BBContainer");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  BBContainerSEXP(IRISEXP Bindings, IRISEXP BB, bool ARGUMENTS, bool ASYNC, bool STRICT, bool GENERATOR, bool PROTO, bool NEW, bool SCALL, bool SOBJ, bool HOME, bool DERIVED, bool TopLevel, double ECMAArgs, double StartBBIDX, double ScopeIDX, double ContainerFlagID) {
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
public:

  BindingsSEXP(IRISEXP obj) {
    assert(obj->tag == "Bindings");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  BindingsSEXP(IRISEXP LocalBindings, IRISEXP RemoteBindings, IRISEXP Lambdas, double ParentScope) {
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
public:

  StarExportSEXP(IRISEXP obj) {
    assert(obj->tag == "StarExport");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  StarExportSEXP(double MODULEREQIDX) {
    this->setMODULEREQIDX(MODULEREQIDX);
  }


  void setMODULEREQIDX(double value) { setFlag("MODULEREQIDX", value); }
  void unsetMODULEREQIDX() { removeFlag("MODULEREQIDX"); }
  bool hasMODULEREQIDX() { return hasFlag("MODULEREQIDX"); }
  double getMODULEREQIDX() { return getFlagDouble("MODULEREQIDX"); }

};

class StaticImportSEXP : public IridiumSEXP {
public:

  StaticImportSEXP(IRISEXP obj) {
    assert(obj->tag == "StaticImport");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  StaticImportSEXP(IRISEXP StorageLocation, std::string FIELD, double MODULEREQIDX) {
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
public:

  LocalStaticExportSEXP(IRISEXP obj) {
    assert(obj->tag == "LocalStaticExport");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  LocalStaticExportSEXP(IRISEXP StorageLocation, std::string LOCALNAME, std::string EXPORTNAME) {
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
public:

  NamedReexportSEXP(IRISEXP obj) {
    assert(obj->tag == "NamedReexport");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  NamedReexportSEXP(std::string EXPORTNAME, double MODULEREQIDX) {
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
public:

  ModuleRequestSEXP(IRISEXP obj) {
    assert(obj->tag == "ModuleRequest");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  ModuleRequestSEXP(std::string SOURCE, double REQIDX) {
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
public:

  EnvBindingSEXP(IRISEXP obj) {
    assert(obj->tag == "EnvBinding");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  EnvBindingSEXP(std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT) {
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
public:

  RemoteEnvBindingSEXP(IRISEXP obj) {
    assert(obj->tag == "RemoteEnvBinding");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  RemoteEnvBindingSEXP(IRISEXP ParentReference, bool NSIMPORT, double REFIDX) {
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
public:

  GlobalBindingSEXP(IRISEXP obj) {
    assert(obj->tag == "GlobalBinding");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  GlobalBindingSEXP(std::string NAME) {
    this->setNAME(NAME);
  }


  void setNAME(const std::string &value) { setFlag("NAME", value); }
  void unsetNAME() { removeFlag("NAME"); }
  bool hasNAME() { return hasFlag("NAME"); }
  std::string getNAME() { return getFlagString("NAME"); }

};

class EnvWriteSEXP : public IridiumSEXP {
public:

  EnvWriteSEXP(IRISEXP obj) {
    assert(obj->tag == "EnvWrite");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  EnvWriteSEXP(IRISEXP LValTarget, IRISEXP RVal, bool SLOPPY, bool SAFE, bool THISINIT) {
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
public:

  JSNUBDSEXP(IRISEXP obj) {
    assert(obj->tag == "JSNUBD");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  JSNUBDSEXP() {

  }

};

class JSSloppyDeclSEXP : public IridiumSEXP {
public:

  JSSloppyDeclSEXP(IRISEXP obj) {
    assert(obj->tag == "JSSloppyDecl");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  JSSloppyDeclSEXP(std::string NAME, bool JSLET, bool JSCONST, bool JSVAR) {
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
public:

  NumberSEXP(IRISEXP obj) {
    assert(obj->tag == "Number");
    
    this->tag   = obj->tag;
    this->args  = std::move(obj->args);
    this->flags = std::move(obj->flags);
  }


  NumberSEXP(double IridiumPrimitive) {
    this->setIridiumPrimitive(IridiumPrimitive);
  }


  void setIridiumPrimitive(double value) { setFlag("IridiumPrimitive", value); }
  void unsetIridiumPrimitive() { removeFlag("IridiumPrimitive"); }
  bool hasIridiumPrimitive() { return hasFlag("IridiumPrimitive"); }
  double getIridiumPrimitive() { return getFlagDouble("IridiumPrimitive"); }

};
