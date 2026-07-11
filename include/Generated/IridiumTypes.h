// Generated: 2026-07-11 17:47:06
#pragma once
#include "Storage/Config.h"
#include "Storage/IridiumSEXP.h"
#include "Storage/IridiumPool.h"
#include "IridiumEnums.h"
#include <variant>
#include <span>
#include <cassert>
#include <stdexcept>

namespace IRI_GEN {
using IRI_STORAGE::IridiumPool;
using IRI_STORAGE::IRID;
using IRI_STORAGE::FlagValue;
  struct FileSEXP {
    IRID id;
    IridiumPool* pool;

    explicit FileSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::File) {
        throw std::runtime_error("Schema Cast Error: Expected File, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, bool JSScript, bool JSModule, bool TLA) {
      return p.add_node(IRI_GEN::IRI_TAG::File, {}, {JSScript ? FlagValue(JSScript) : FlagValue(std::monostate()), JSModule ? FlagValue(JSModule) : FlagValue(std::monostate()), TLA ? FlagValue(TLA) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 3;

    static constexpr uint32_t FLAG_IDX_JSScript = 0;
    static constexpr uint32_t FLAG_IDX_JSModule = 1;
    static constexpr uint32_t FLAG_IDX_TLA = 2;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    bool hasJSScript() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSScript)); }
    void setJSScript() { mutate_flag(FLAG_IDX_JSScript) = std::nullptr_t{}; }
    void clearJSScript() { mutate_flag(FLAG_IDX_JSScript) = std::monostate{}; }

    bool hasJSModule() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSModule)); }
    void setJSModule() { mutate_flag(FLAG_IDX_JSModule) = std::nullptr_t{}; }
    void clearJSModule() { mutate_flag(FLAG_IDX_JSModule) = std::monostate{}; }

    bool hasTLA() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_TLA)); }
    void setTLA() { mutate_flag(FLAG_IDX_TLA) = std::nullptr_t{}; }
    void clearTLA() { mutate_flag(FLAG_IDX_TLA) = std::monostate{}; }
  };

  struct ResolveEnvBindingSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ResolveEnvBindingSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::ResolveEnvBinding) {
        throw std::runtime_error("Schema Cast Error: Expected ResolveEnvBinding, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID NAME, bool ASW) {
      return p.add_node(IRI_GEN::IRI_TAG::ResolveEnvBinding, {}, {FlagValue(NAME), ASW ? FlagValue(ASW) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_NAME = 0;
    static constexpr uint32_t FLAG_IDX_ASW = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }

    bool hasASW() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_ASW)); }
    void setASW() { mutate_flag(FLAG_IDX_ASW) = std::nullptr_t{}; }
    void clearASW() { mutate_flag(FLAG_IDX_ASW) = std::monostate{}; }
  };

  struct ListSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ListSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::List) {
        throw std::runtime_error("Schema Cast Error: Expected List, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID TYPE) {
      return p.add_node(IRI_GEN::IRI_TAG::List, {}, {FlagValue(TYPE)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_TYPE = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getTYPE() const { return std::get<StringID>(get_flag(FLAG_IDX_TYPE)); }
    void setTYPE(StringID val) { mutate_flag(FLAG_IDX_TYPE) = val; }
    bool hasTYPE() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_TYPE)); }
    void clearTYPE() { mutate_flag(FLAG_IDX_TYPE) = std::monostate{}; }
  };

  struct JSImplicitBindingDeclarationSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSImplicitBindingDeclarationSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSImplicitBindingDeclaration) {
        throw std::runtime_error("Schema Cast Error: Expected JSImplicitBindingDeclaration, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Store, IRID Args, StringID NAME, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SKIPINIT, bool SAFE, bool THISINIT, double OPID) {
      return p.add_node(IRI_GEN::IRI_TAG::JSImplicitBindingDeclaration, {Store, Args}, {FlagValue(NAME), JSLET ? FlagValue(JSLET) : FlagValue(std::monostate()), JSCONST ? FlagValue(JSCONST) : FlagValue(std::monostate()), JSVAR ? FlagValue(JSVAR) : FlagValue(std::monostate()), SLOPPY ? FlagValue(SLOPPY) : FlagValue(std::monostate()), SKIPINIT ? FlagValue(SKIPINIT) : FlagValue(std::monostate()), FlagValue(SAFE), FlagValue(THISINIT), FlagValue(OPID)});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 9;

    static constexpr uint32_t FLAG_IDX_NAME = 0;
    static constexpr uint32_t FLAG_IDX_JSLET = 1;
    static constexpr uint32_t FLAG_IDX_JSCONST = 2;
    static constexpr uint32_t FLAG_IDX_JSVAR = 3;
    static constexpr uint32_t FLAG_IDX_SLOPPY = 4;
    static constexpr uint32_t FLAG_IDX_SKIPINIT = 5;
    static constexpr uint32_t FLAG_IDX_SAFE = 6;
    static constexpr uint32_t FLAG_IDX_THISINIT = 7;
    static constexpr uint32_t FLAG_IDX_OPID = 8;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Store() const { return pool->get_args(id)[0]; }
    bool hasArg_Store() const { return 0 < pool->get_args(id).size(); }
    void setArg_Store(IRID val) { assert(hasArg_Store() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Args() const { return pool->get_args(id)[1]; }
    bool hasArg_Args() const { return 1 < pool->get_args(id).size(); }
    void setArg_Args(IRID val) { assert(hasArg_Args() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }

    bool hasJSLET() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSLET)); }
    void setJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::nullptr_t{}; }
    void clearJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::monostate{}; }

    bool hasJSCONST() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSCONST)); }
    void setJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::nullptr_t{}; }
    void clearJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::monostate{}; }

    bool hasJSVAR() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSVAR)); }
    void setJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::nullptr_t{}; }
    void clearJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::monostate{}; }

    bool hasSLOPPY() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SLOPPY)); }
    void setSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::nullptr_t{}; }
    void clearSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::monostate{}; }

    bool hasSKIPINIT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SKIPINIT)); }
    void setSKIPINIT() { mutate_flag(FLAG_IDX_SKIPINIT) = std::nullptr_t{}; }
    void clearSKIPINIT() { mutate_flag(FLAG_IDX_SKIPINIT) = std::monostate{}; }

    bool getSAFE() const { return std::get<bool>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE(bool val) { mutate_flag(FLAG_IDX_SAFE) = val; }
    bool hasSAFE() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_SAFE)); }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool getTHISINIT() const { return std::get<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void setTHISINIT(bool val) { mutate_flag(FLAG_IDX_THISINIT) = val; }
    bool hasTHISINIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void clearTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::monostate{}; }

    double getOPID() const { return std::get<double>(get_flag(FLAG_IDX_OPID)); }
    void setOPID(double val) { mutate_flag(FLAG_IDX_OPID) = val; }
    bool hasOPID() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_OPID)); }
    void clearOPID() { mutate_flag(FLAG_IDX_OPID) = std::monostate{}; }
  };

  struct EnvReadSEXP {
    IRID id;
    IridiumPool* pool;

    explicit EnvReadSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::EnvRead) {
        throw std::runtime_error("Schema Cast Error: Expected EnvRead, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, bool SAFE, bool TAINTED) {
      return p.add_node(IRI_GEN::IRI_TAG::EnvRead, {Obj}, {SAFE ? FlagValue(SAFE) : FlagValue(std::monostate()), TAINTED ? FlagValue(TAINTED) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_SAFE = 0;
    static constexpr uint32_t FLAG_IDX_TAINTED = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    bool hasSAFE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::nullptr_t{}; }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool hasTAINTED() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_TAINTED)); }
    void setTAINTED() { mutate_flag(FLAG_IDX_TAINTED) = std::nullptr_t{}; }
    void clearTAINTED() { mutate_flag(FLAG_IDX_TAINTED) = std::monostate{}; }
  };

  struct TDZReadSEXP {
    IRID id;
    IridiumPool* pool;

    explicit TDZReadSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::TDZRead) {
        throw std::runtime_error("Schema Cast Error: Expected TDZRead, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, bool SAFE) {
      return p.add_node(IRI_GEN::IRI_TAG::TDZRead, {Obj}, {SAFE ? FlagValue(SAFE) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_SAFE = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    bool hasSAFE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::nullptr_t{}; }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }
  };

  struct StringSEXP {
    IRID id;
    IridiumPool* pool;

    explicit StringSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::String) {
        throw std::runtime_error("Schema Cast Error: Expected String, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID IridiumPrimitive) {
      return p.add_node(IRI_GEN::IRI_TAG::String, {}, {FlagValue(IridiumPrimitive)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_IridiumPrimitive = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getIridiumPrimitive() const { return std::get<StringID>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void setIridiumPrimitive(StringID val) { mutate_flag(FLAG_IDX_IridiumPrimitive) = val; }
    bool hasIridiumPrimitive() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void clearIridiumPrimitive() { mutate_flag(FLAG_IDX_IridiumPrimitive) = std::monostate{}; }
  };

  struct FieldReadSEXP {
    IRID id;
    IridiumPool* pool;

    explicit FieldReadSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::FieldRead) {
        throw std::runtime_error("Schema Cast Error: Expected FieldRead, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, IRID Field) {
      return p.add_node(IRI_GEN::IRI_TAG::FieldRead, {Obj, Field}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Field() const { return pool->get_args(id)[1]; }
    bool hasArg_Field() const { return 1 < pool->get_args(id).size(); }
    void setArg_Field(IRID val) { assert(hasArg_Field() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---

  };

  struct JSExplicitBindingDeclarationSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSExplicitBindingDeclarationSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSExplicitBindingDeclaration) {
        throw std::runtime_error("Schema Cast Error: Expected JSExplicitBindingDeclaration, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LValTarget, IRID RVal, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SAFE, bool THISINIT) {
      return p.add_node(IRI_GEN::IRI_TAG::JSExplicitBindingDeclaration, {LValTarget, RVal}, {JSLET ? FlagValue(JSLET) : FlagValue(std::monostate()), JSCONST ? FlagValue(JSCONST) : FlagValue(std::monostate()), JSVAR ? FlagValue(JSVAR) : FlagValue(std::monostate()), SLOPPY ? FlagValue(SLOPPY) : FlagValue(std::monostate()), FlagValue(SAFE), FlagValue(THISINIT)});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 6;

    static constexpr uint32_t FLAG_IDX_JSLET = 0;
    static constexpr uint32_t FLAG_IDX_JSCONST = 1;
    static constexpr uint32_t FLAG_IDX_JSVAR = 2;
    static constexpr uint32_t FLAG_IDX_SLOPPY = 3;
    static constexpr uint32_t FLAG_IDX_SAFE = 4;
    static constexpr uint32_t FLAG_IDX_THISINIT = 5;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LValTarget() const { return pool->get_args(id)[0]; }
    bool hasArg_LValTarget() const { return 0 < pool->get_args(id).size(); }
    void setArg_LValTarget(IRID val) { assert(hasArg_LValTarget() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RVal() const { return pool->get_args(id)[1]; }
    bool hasArg_RVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_RVal(IRID val) { assert(hasArg_RVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasJSLET() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSLET)); }
    void setJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::nullptr_t{}; }
    void clearJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::monostate{}; }

    bool hasJSCONST() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSCONST)); }
    void setJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::nullptr_t{}; }
    void clearJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::monostate{}; }

    bool hasJSVAR() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSVAR)); }
    void setJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::nullptr_t{}; }
    void clearJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::monostate{}; }

    bool hasSLOPPY() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SLOPPY)); }
    void setSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::nullptr_t{}; }
    void clearSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::monostate{}; }

    bool getSAFE() const { return std::get<bool>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE(bool val) { mutate_flag(FLAG_IDX_SAFE) = val; }
    bool hasSAFE() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_SAFE)); }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool getTHISINIT() const { return std::get<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void setTHISINIT(bool val) { mutate_flag(FLAG_IDX_THISINIT) = val; }
    bool hasTHISINIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void clearTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::monostate{}; }
  };

  struct JSExplicitBindingDeclarationNSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSExplicitBindingDeclarationNSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSExplicitBindingDeclarationN) {
        throw std::runtime_error("Schema Cast Error: Expected JSExplicitBindingDeclarationN, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LValTarget, IRID RVal, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SAFE, bool THISINIT) {
      return p.add_node(IRI_GEN::IRI_TAG::JSExplicitBindingDeclarationN, {LValTarget, RVal}, {JSLET ? FlagValue(JSLET) : FlagValue(std::monostate()), JSCONST ? FlagValue(JSCONST) : FlagValue(std::monostate()), JSVAR ? FlagValue(JSVAR) : FlagValue(std::monostate()), SLOPPY ? FlagValue(SLOPPY) : FlagValue(std::monostate()), FlagValue(SAFE), FlagValue(THISINIT)});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 6;

    static constexpr uint32_t FLAG_IDX_JSLET = 0;
    static constexpr uint32_t FLAG_IDX_JSCONST = 1;
    static constexpr uint32_t FLAG_IDX_JSVAR = 2;
    static constexpr uint32_t FLAG_IDX_SLOPPY = 3;
    static constexpr uint32_t FLAG_IDX_SAFE = 4;
    static constexpr uint32_t FLAG_IDX_THISINIT = 5;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LValTarget() const { return pool->get_args(id)[0]; }
    bool hasArg_LValTarget() const { return 0 < pool->get_args(id).size(); }
    void setArg_LValTarget(IRID val) { assert(hasArg_LValTarget() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RVal() const { return pool->get_args(id)[1]; }
    bool hasArg_RVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_RVal(IRID val) { assert(hasArg_RVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasJSLET() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSLET)); }
    void setJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::nullptr_t{}; }
    void clearJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::monostate{}; }

    bool hasJSCONST() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSCONST)); }
    void setJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::nullptr_t{}; }
    void clearJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::monostate{}; }

    bool hasJSVAR() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSVAR)); }
    void setJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::nullptr_t{}; }
    void clearJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::monostate{}; }

    bool hasSLOPPY() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SLOPPY)); }
    void setSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::nullptr_t{}; }
    void clearSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::monostate{}; }

    bool getSAFE() const { return std::get<bool>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE(bool val) { mutate_flag(FLAG_IDX_SAFE) = val; }
    bool hasSAFE() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_SAFE)); }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool getTHISINIT() const { return std::get<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void setTHISINIT(bool val) { mutate_flag(FLAG_IDX_THISINIT) = val; }
    bool hasTHISINIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void clearTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::monostate{}; }
  };

  struct JSExplicitBindingDeclarationXSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSExplicitBindingDeclarationXSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSExplicitBindingDeclarationX) {
        throw std::runtime_error("Schema Cast Error: Expected JSExplicitBindingDeclarationX, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LValTarget, IRID RVal, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SAFE, bool THISINIT) {
      return p.add_node(IRI_GEN::IRI_TAG::JSExplicitBindingDeclarationX, {LValTarget, RVal}, {JSLET ? FlagValue(JSLET) : FlagValue(std::monostate()), JSCONST ? FlagValue(JSCONST) : FlagValue(std::monostate()), JSVAR ? FlagValue(JSVAR) : FlagValue(std::monostate()), SLOPPY ? FlagValue(SLOPPY) : FlagValue(std::monostate()), FlagValue(SAFE), FlagValue(THISINIT)});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 6;

    static constexpr uint32_t FLAG_IDX_JSLET = 0;
    static constexpr uint32_t FLAG_IDX_JSCONST = 1;
    static constexpr uint32_t FLAG_IDX_JSVAR = 2;
    static constexpr uint32_t FLAG_IDX_SLOPPY = 3;
    static constexpr uint32_t FLAG_IDX_SAFE = 4;
    static constexpr uint32_t FLAG_IDX_THISINIT = 5;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LValTarget() const { return pool->get_args(id)[0]; }
    bool hasArg_LValTarget() const { return 0 < pool->get_args(id).size(); }
    void setArg_LValTarget(IRID val) { assert(hasArg_LValTarget() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RVal() const { return pool->get_args(id)[1]; }
    bool hasArg_RVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_RVal(IRID val) { assert(hasArg_RVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasJSLET() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSLET)); }
    void setJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::nullptr_t{}; }
    void clearJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::monostate{}; }

    bool hasJSCONST() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSCONST)); }
    void setJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::nullptr_t{}; }
    void clearJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::monostate{}; }

    bool hasJSVAR() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSVAR)); }
    void setJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::nullptr_t{}; }
    void clearJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::monostate{}; }

    bool hasSLOPPY() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SLOPPY)); }
    void setSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::nullptr_t{}; }
    void clearSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::monostate{}; }

    bool getSAFE() const { return std::get<bool>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE(bool val) { mutate_flag(FLAG_IDX_SAFE) = val; }
    bool hasSAFE() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_SAFE)); }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool getTHISINIT() const { return std::get<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void setTHISINIT(bool val) { mutate_flag(FLAG_IDX_THISINIT) = val; }
    bool hasTHISINIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void clearTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::monostate{}; }
  };

  struct CallSiteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit CallSiteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::CallSite) {
        throw std::runtime_error("Schema Cast Error: Expected CallSite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, bool CCall, bool ConstructorCall, bool PrivateCall, bool Import, bool Super, bool V8Intrinsic, bool TAILCALL, double JSDirectEval) {
      return p.add_node(IRI_GEN::IRI_TAG::CallSite, {}, {CCall ? FlagValue(CCall) : FlagValue(std::monostate()), ConstructorCall ? FlagValue(ConstructorCall) : FlagValue(std::monostate()), PrivateCall ? FlagValue(PrivateCall) : FlagValue(std::monostate()), Import ? FlagValue(Import) : FlagValue(std::monostate()), Super ? FlagValue(Super) : FlagValue(std::monostate()), V8Intrinsic ? FlagValue(V8Intrinsic) : FlagValue(std::monostate()), TAILCALL ? FlagValue(TAILCALL) : FlagValue(std::monostate()), FlagValue(JSDirectEval)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 8;

    static constexpr uint32_t FLAG_IDX_CCall = 0;
    static constexpr uint32_t FLAG_IDX_ConstructorCall = 1;
    static constexpr uint32_t FLAG_IDX_PrivateCall = 2;
    static constexpr uint32_t FLAG_IDX_Import = 3;
    static constexpr uint32_t FLAG_IDX_Super = 4;
    static constexpr uint32_t FLAG_IDX_V8Intrinsic = 5;
    static constexpr uint32_t FLAG_IDX_TAILCALL = 6;
    static constexpr uint32_t FLAG_IDX_JSDirectEval = 7;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    bool hasCCall() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_CCall)); }
    void setCCall() { mutate_flag(FLAG_IDX_CCall) = std::nullptr_t{}; }
    void clearCCall() { mutate_flag(FLAG_IDX_CCall) = std::monostate{}; }

    bool hasConstructorCall() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_ConstructorCall)); }
    void setConstructorCall() { mutate_flag(FLAG_IDX_ConstructorCall) = std::nullptr_t{}; }
    void clearConstructorCall() { mutate_flag(FLAG_IDX_ConstructorCall) = std::monostate{}; }

    bool hasPrivateCall() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_PrivateCall)); }
    void setPrivateCall() { mutate_flag(FLAG_IDX_PrivateCall) = std::nullptr_t{}; }
    void clearPrivateCall() { mutate_flag(FLAG_IDX_PrivateCall) = std::monostate{}; }

    bool hasImport() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_Import)); }
    void setImport() { mutate_flag(FLAG_IDX_Import) = std::nullptr_t{}; }
    void clearImport() { mutate_flag(FLAG_IDX_Import) = std::monostate{}; }

    bool hasSuper() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_Super)); }
    void setSuper() { mutate_flag(FLAG_IDX_Super) = std::nullptr_t{}; }
    void clearSuper() { mutate_flag(FLAG_IDX_Super) = std::monostate{}; }

    bool hasV8Intrinsic() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_V8Intrinsic)); }
    void setV8Intrinsic() { mutate_flag(FLAG_IDX_V8Intrinsic) = std::nullptr_t{}; }
    void clearV8Intrinsic() { mutate_flag(FLAG_IDX_V8Intrinsic) = std::monostate{}; }

    bool hasTAILCALL() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_TAILCALL)); }
    void setTAILCALL() { mutate_flag(FLAG_IDX_TAILCALL) = std::nullptr_t{}; }
    void clearTAILCALL() { mutate_flag(FLAG_IDX_TAILCALL) = std::monostate{}; }

    double getJSDirectEval() const { return std::get<double>(get_flag(FLAG_IDX_JSDirectEval)); }
    void setJSDirectEval(double val) { mutate_flag(FLAG_IDX_JSDirectEval) = val; }
    bool hasJSDirectEval() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_JSDirectEval)); }
    void clearJSDirectEval() { mutate_flag(FLAG_IDX_JSDirectEval) = std::monostate{}; }
  };

  struct ApplySEXP {
    IRID id;
    IridiumPool* pool;

    explicit ApplySEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Apply) {
        throw std::runtime_error("Schema Cast Error: Expected Apply, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Callee, IRID Context, IRID ArgList, bool ConstructorCall, bool Super, double JSDirectEval) {
      return p.add_node(IRI_GEN::IRI_TAG::Apply, {Callee, Context, ArgList}, {ConstructorCall ? FlagValue(ConstructorCall) : FlagValue(std::monostate()), Super ? FlagValue(Super) : FlagValue(std::monostate()), FlagValue(JSDirectEval)});
    }

    static constexpr uint32_t TOTAL_ARGS = 3;
    static constexpr uint32_t TOTAL_FLAGS = 3;

    static constexpr uint32_t FLAG_IDX_ConstructorCall = 0;
    static constexpr uint32_t FLAG_IDX_Super = 1;
    static constexpr uint32_t FLAG_IDX_JSDirectEval = 2;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Callee() const { return pool->get_args(id)[0]; }
    bool hasArg_Callee() const { return 0 < pool->get_args(id).size(); }
    void setArg_Callee(IRID val) { assert(hasArg_Callee() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Context() const { return pool->get_args(id)[1]; }
    bool hasArg_Context() const { return 1 < pool->get_args(id).size(); }
    void setArg_Context(IRID val) { assert(hasArg_Context() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_ArgList() const { return pool->get_args(id)[2]; }
    bool hasArg_ArgList() const { return 2 < pool->get_args(id).size(); }
    void setArg_ArgList(IRID val) { assert(hasArg_ArgList() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    // --- Flags ---
    bool hasConstructorCall() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_ConstructorCall)); }
    void setConstructorCall() { mutate_flag(FLAG_IDX_ConstructorCall) = std::nullptr_t{}; }
    void clearConstructorCall() { mutate_flag(FLAG_IDX_ConstructorCall) = std::monostate{}; }

    bool hasSuper() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_Super)); }
    void setSuper() { mutate_flag(FLAG_IDX_Super) = std::nullptr_t{}; }
    void clearSuper() { mutate_flag(FLAG_IDX_Super) = std::monostate{}; }

    double getJSDirectEval() const { return std::get<double>(get_flag(FLAG_IDX_JSDirectEval)); }
    void setJSDirectEval(double val) { mutate_flag(FLAG_IDX_JSDirectEval) = val; }
    bool hasJSDirectEval() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_JSDirectEval)); }
    void clearJSDirectEval() { mutate_flag(FLAG_IDX_JSDirectEval) = std::monostate{}; }
  };

  struct ReturnAsyncSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ReturnAsyncSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::ReturnAsync) {
        throw std::runtime_error("Schema Cast Error: Expected ReturnAsync, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID RetVal) {
      return p.add_node(IRI_GEN::IRI_TAG::ReturnAsync, {RetVal}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_RetVal() const { return pool->get_args(id)[0]; }
    bool hasArg_RetVal() const { return 0 < pool->get_args(id).size(); }
    void setArg_RetVal(IRID val) { assert(hasArg_RetVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---

  };

  struct BBSEXP {
    IRID id;
    IridiumPool* pool;

    explicit BBSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::BB) {
        throw std::runtime_error("Schema Cast Error: Expected BB, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, bool TopLevel, bool ClosureBoundary, bool Lexical, bool VARBoundary, bool TryBB, double IDX, double ScopeIDX) {
      return p.add_node(IRI_GEN::IRI_TAG::BB, {}, {TopLevel ? FlagValue(TopLevel) : FlagValue(std::monostate()), ClosureBoundary ? FlagValue(ClosureBoundary) : FlagValue(std::monostate()), Lexical ? FlagValue(Lexical) : FlagValue(std::monostate()), VARBoundary ? FlagValue(VARBoundary) : FlagValue(std::monostate()), TryBB ? FlagValue(TryBB) : FlagValue(std::monostate()), FlagValue(IDX), FlagValue(ScopeIDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 7;

    static constexpr uint32_t FLAG_IDX_TopLevel = 0;
    static constexpr uint32_t FLAG_IDX_ClosureBoundary = 1;
    static constexpr uint32_t FLAG_IDX_Lexical = 2;
    static constexpr uint32_t FLAG_IDX_VARBoundary = 3;
    static constexpr uint32_t FLAG_IDX_TryBB = 4;
    static constexpr uint32_t FLAG_IDX_IDX = 5;
    static constexpr uint32_t FLAG_IDX_ScopeIDX = 6;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    bool hasTopLevel() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_TopLevel)); }
    void setTopLevel() { mutate_flag(FLAG_IDX_TopLevel) = std::nullptr_t{}; }
    void clearTopLevel() { mutate_flag(FLAG_IDX_TopLevel) = std::monostate{}; }

    bool hasClosureBoundary() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_ClosureBoundary)); }
    void setClosureBoundary() { mutate_flag(FLAG_IDX_ClosureBoundary) = std::nullptr_t{}; }
    void clearClosureBoundary() { mutate_flag(FLAG_IDX_ClosureBoundary) = std::monostate{}; }

    bool hasLexical() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_Lexical)); }
    void setLexical() { mutate_flag(FLAG_IDX_Lexical) = std::nullptr_t{}; }
    void clearLexical() { mutate_flag(FLAG_IDX_Lexical) = std::monostate{}; }

    bool hasVARBoundary() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_VARBoundary)); }
    void setVARBoundary() { mutate_flag(FLAG_IDX_VARBoundary) = std::nullptr_t{}; }
    void clearVARBoundary() { mutate_flag(FLAG_IDX_VARBoundary) = std::monostate{}; }

    bool hasTryBB() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_TryBB)); }
    void setTryBB() { mutate_flag(FLAG_IDX_TryBB) = std::nullptr_t{}; }
    void clearTryBB() { mutate_flag(FLAG_IDX_TryBB) = std::monostate{}; }

    double getIDX() const { return std::get<double>(get_flag(FLAG_IDX_IDX)); }
    void setIDX(double val) { mutate_flag(FLAG_IDX_IDX) = val; }
    bool hasIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_IDX)); }
    void clearIDX() { mutate_flag(FLAG_IDX_IDX) = std::monostate{}; }

    double getScopeIDX() const { return std::get<double>(get_flag(FLAG_IDX_ScopeIDX)); }
    void setScopeIDX(double val) { mutate_flag(FLAG_IDX_ScopeIDX) = val; }
    bool hasScopeIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_ScopeIDX)); }
    void clearScopeIDX() { mutate_flag(FLAG_IDX_ScopeIDX) = std::monostate{}; }
  };

  struct ReturnSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ReturnSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Return) {
        throw std::runtime_error("Schema Cast Error: Expected Return, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj) {
      return p.add_node(IRI_GEN::IRI_TAG::Return, {Obj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---

  };

  struct UnresolvedReturnSEXP {
    IRID id;
    IridiumPool* pool;

    explicit UnresolvedReturnSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::UnresolvedReturn) {
        throw std::runtime_error("Schema Cast Error: Expected UnresolvedReturn, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::UnresolvedReturn, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct IfJumpSEXP {
    IRID id;
    IridiumPool* pool;

    explicit IfJumpSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::IfJump) {
        throw std::runtime_error("Schema Cast Error: Expected IfJump, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Test, bool NOT, double IDX) {
      return p.add_node(IRI_GEN::IRI_TAG::IfJump, {Test}, {NOT ? FlagValue(NOT) : FlagValue(std::monostate()), FlagValue(IDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_NOT = 0;
    static constexpr uint32_t FLAG_IDX_IDX = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Test() const { return pool->get_args(id)[0]; }
    bool hasArg_Test() const { return 0 < pool->get_args(id).size(); }
    void setArg_Test(IRID val) { assert(hasArg_Test() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    bool hasNOT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_NOT)); }
    void setNOT() { mutate_flag(FLAG_IDX_NOT) = std::nullptr_t{}; }
    void clearNOT() { mutate_flag(FLAG_IDX_NOT) = std::monostate{}; }

    double getIDX() const { return std::get<double>(get_flag(FLAG_IDX_IDX)); }
    void setIDX(double val) { mutate_flag(FLAG_IDX_IDX) = val; }
    bool hasIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_IDX)); }
    void clearIDX() { mutate_flag(FLAG_IDX_IDX) = std::monostate{}; }
  };

  struct IfElseJumpSEXP {
    IRID id;
    IridiumPool* pool;

    explicit IfElseJumpSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::IfElseJump) {
        throw std::runtime_error("Schema Cast Error: Expected IfElseJump, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Test, bool NOT, double TRUE, double FALSE) {
      return p.add_node(IRI_GEN::IRI_TAG::IfElseJump, {Test}, {NOT ? FlagValue(NOT) : FlagValue(std::monostate()), FlagValue(TRUE), FlagValue(FALSE)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 3;

    static constexpr uint32_t FLAG_IDX_NOT = 0;
    static constexpr uint32_t FLAG_IDX_TRUE = 1;
    static constexpr uint32_t FLAG_IDX_FALSE = 2;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Test() const { return pool->get_args(id)[0]; }
    bool hasArg_Test() const { return 0 < pool->get_args(id).size(); }
    void setArg_Test(IRID val) { assert(hasArg_Test() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    bool hasNOT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_NOT)); }
    void setNOT() { mutate_flag(FLAG_IDX_NOT) = std::nullptr_t{}; }
    void clearNOT() { mutate_flag(FLAG_IDX_NOT) = std::monostate{}; }

    double getTRUE() const { return std::get<double>(get_flag(FLAG_IDX_TRUE)); }
    void setTRUE(double val) { mutate_flag(FLAG_IDX_TRUE) = val; }
    bool hasTRUE() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_TRUE)); }
    void clearTRUE() { mutate_flag(FLAG_IDX_TRUE) = std::monostate{}; }

    double getFALSE() const { return std::get<double>(get_flag(FLAG_IDX_FALSE)); }
    void setFALSE(double val) { mutate_flag(FLAG_IDX_FALSE) = val; }
    bool hasFALSE() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_FALSE)); }
    void clearFALSE() { mutate_flag(FLAG_IDX_FALSE) = std::monostate{}; }
  };

  struct GotoSEXP {
    IRID id;
    IridiumPool* pool;

    explicit GotoSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Goto) {
        throw std::runtime_error("Schema Cast Error: Expected Goto, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, double IDX) {
      return p.add_node(IRI_GEN::IRI_TAG::Goto, {}, {FlagValue(IDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_IDX = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    double getIDX() const { return std::get<double>(get_flag(FLAG_IDX_IDX)); }
    void setIDX(double val) { mutate_flag(FLAG_IDX_IDX) = val; }
    bool hasIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_IDX)); }
    void clearIDX() { mutate_flag(FLAG_IDX_IDX) = std::monostate{}; }
  };

  struct JSFuncDeclSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSFuncDeclSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSFuncDecl) {
        throw std::runtime_error("Schema Cast Error: Expected JSFuncDecl, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LValTarget, IRID RVal, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SAFE, bool THISINIT) {
      return p.add_node(IRI_GEN::IRI_TAG::JSFuncDecl, {LValTarget, RVal}, {JSLET ? FlagValue(JSLET) : FlagValue(std::monostate()), JSCONST ? FlagValue(JSCONST) : FlagValue(std::monostate()), JSVAR ? FlagValue(JSVAR) : FlagValue(std::monostate()), SLOPPY ? FlagValue(SLOPPY) : FlagValue(std::monostate()), FlagValue(SAFE), FlagValue(THISINIT)});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 6;

    static constexpr uint32_t FLAG_IDX_JSLET = 0;
    static constexpr uint32_t FLAG_IDX_JSCONST = 1;
    static constexpr uint32_t FLAG_IDX_JSVAR = 2;
    static constexpr uint32_t FLAG_IDX_SLOPPY = 3;
    static constexpr uint32_t FLAG_IDX_SAFE = 4;
    static constexpr uint32_t FLAG_IDX_THISINIT = 5;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LValTarget() const { return pool->get_args(id)[0]; }
    bool hasArg_LValTarget() const { return 0 < pool->get_args(id).size(); }
    void setArg_LValTarget(IRID val) { assert(hasArg_LValTarget() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RVal() const { return pool->get_args(id)[1]; }
    bool hasArg_RVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_RVal(IRID val) { assert(hasArg_RVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasJSLET() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSLET)); }
    void setJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::nullptr_t{}; }
    void clearJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::monostate{}; }

    bool hasJSCONST() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSCONST)); }
    void setJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::nullptr_t{}; }
    void clearJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::monostate{}; }

    bool hasJSVAR() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSVAR)); }
    void setJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::nullptr_t{}; }
    void clearJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::monostate{}; }

    bool hasSLOPPY() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SLOPPY)); }
    void setSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::nullptr_t{}; }
    void clearSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::monostate{}; }

    bool getSAFE() const { return std::get<bool>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE(bool val) { mutate_flag(FLAG_IDX_SAFE) = val; }
    bool hasSAFE() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_SAFE)); }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool getTHISINIT() const { return std::get<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void setTHISINIT(bool val) { mutate_flag(FLAG_IDX_THISINIT) = val; }
    bool hasTHISINIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void clearTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::monostate{}; }
  };

  struct LambdaSEXP {
    IRID id;
    IridiumPool* pool;

    explicit LambdaSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Lambda) {
        throw std::runtime_error("Schema Cast Error: Expected Lambda, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID NAME, bool CNAME, bool SETNAME, double StartBBIDX) {
      return p.add_node(IRI_GEN::IRI_TAG::Lambda, {}, {FlagValue(NAME), FlagValue(CNAME), FlagValue(SETNAME), FlagValue(StartBBIDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 4;

    static constexpr uint32_t FLAG_IDX_NAME = 0;
    static constexpr uint32_t FLAG_IDX_CNAME = 1;
    static constexpr uint32_t FLAG_IDX_SETNAME = 2;
    static constexpr uint32_t FLAG_IDX_StartBBIDX = 3;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }

    bool getCNAME() const { return std::get<bool>(get_flag(FLAG_IDX_CNAME)); }
    void setCNAME(bool val) { mutate_flag(FLAG_IDX_CNAME) = val; }
    bool hasCNAME() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_CNAME)); }
    void clearCNAME() { mutate_flag(FLAG_IDX_CNAME) = std::monostate{}; }

    bool getSETNAME() const { return std::get<bool>(get_flag(FLAG_IDX_SETNAME)); }
    void setSETNAME(bool val) { mutate_flag(FLAG_IDX_SETNAME) = val; }
    bool hasSETNAME() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_SETNAME)); }
    void clearSETNAME() { mutate_flag(FLAG_IDX_SETNAME) = std::monostate{}; }

    double getStartBBIDX() const { return std::get<double>(get_flag(FLAG_IDX_StartBBIDX)); }
    void setStartBBIDX(double val) { mutate_flag(FLAG_IDX_StartBBIDX) = val; }
    bool hasStartBBIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_StartBBIDX)); }
    void clearStartBBIDX() { mutate_flag(FLAG_IDX_StartBBIDX) = std::monostate{}; }
  };

  struct NOPSEXP {
    IRID id;
    IridiumPool* pool;

    explicit NOPSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::NOP) {
        throw std::runtime_error("Schema Cast Error: Expected NOP, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::NOP, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct BBContainerSEXP {
    IRID id;
    IridiumPool* pool;

    explicit BBContainerSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::BBContainer) {
        throw std::runtime_error("Schema Cast Error: Expected BBContainer, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Bindings, IRID BB, StringID NAME, bool ARGUMENTS, bool ASYNC, bool STRICT, bool GENERATOR, bool PROTO, bool NEW, bool SCALL, bool SOBJ, bool HOME, bool DERIVED, bool TopLevel, double ECMAArgs, double StartBBIDX, double ScopeIDX, double ContainerFlagID) {
      return p.add_node(IRI_GEN::IRI_TAG::BBContainer, {Bindings, BB}, {FlagValue(NAME), ARGUMENTS ? FlagValue(ARGUMENTS) : FlagValue(std::monostate()), ASYNC ? FlagValue(ASYNC) : FlagValue(std::monostate()), STRICT ? FlagValue(STRICT) : FlagValue(std::monostate()), GENERATOR ? FlagValue(GENERATOR) : FlagValue(std::monostate()), PROTO ? FlagValue(PROTO) : FlagValue(std::monostate()), NEW ? FlagValue(NEW) : FlagValue(std::monostate()), SCALL ? FlagValue(SCALL) : FlagValue(std::monostate()), SOBJ ? FlagValue(SOBJ) : FlagValue(std::monostate()), HOME ? FlagValue(HOME) : FlagValue(std::monostate()), DERIVED ? FlagValue(DERIVED) : FlagValue(std::monostate()), TopLevel ? FlagValue(TopLevel) : FlagValue(std::monostate()), FlagValue(ECMAArgs), FlagValue(StartBBIDX), FlagValue(ScopeIDX), FlagValue(ContainerFlagID)});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 16;

    static constexpr uint32_t FLAG_IDX_NAME = 0;
    static constexpr uint32_t FLAG_IDX_ARGUMENTS = 1;
    static constexpr uint32_t FLAG_IDX_ASYNC = 2;
    static constexpr uint32_t FLAG_IDX_STRICT = 3;
    static constexpr uint32_t FLAG_IDX_GENERATOR = 4;
    static constexpr uint32_t FLAG_IDX_PROTO = 5;
    static constexpr uint32_t FLAG_IDX_NEW = 6;
    static constexpr uint32_t FLAG_IDX_SCALL = 7;
    static constexpr uint32_t FLAG_IDX_SOBJ = 8;
    static constexpr uint32_t FLAG_IDX_HOME = 9;
    static constexpr uint32_t FLAG_IDX_DERIVED = 10;
    static constexpr uint32_t FLAG_IDX_TopLevel = 11;
    static constexpr uint32_t FLAG_IDX_ECMAArgs = 12;
    static constexpr uint32_t FLAG_IDX_StartBBIDX = 13;
    static constexpr uint32_t FLAG_IDX_ScopeIDX = 14;
    static constexpr uint32_t FLAG_IDX_ContainerFlagID = 15;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Bindings() const { return pool->get_args(id)[0]; }
    bool hasArg_Bindings() const { return 0 < pool->get_args(id).size(); }
    void setArg_Bindings(IRID val) { assert(hasArg_Bindings() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_BB() const { return pool->get_args(id)[1]; }
    bool hasArg_BB() const { return 1 < pool->get_args(id).size(); }
    void setArg_BB(IRID val) { assert(hasArg_BB() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }

    bool hasARGUMENTS() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_ARGUMENTS)); }
    void setARGUMENTS() { mutate_flag(FLAG_IDX_ARGUMENTS) = std::nullptr_t{}; }
    void clearARGUMENTS() { mutate_flag(FLAG_IDX_ARGUMENTS) = std::monostate{}; }

    bool hasASYNC() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_ASYNC)); }
    void setASYNC() { mutate_flag(FLAG_IDX_ASYNC) = std::nullptr_t{}; }
    void clearASYNC() { mutate_flag(FLAG_IDX_ASYNC) = std::monostate{}; }

    bool hasSTRICT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_STRICT)); }
    void setSTRICT() { mutate_flag(FLAG_IDX_STRICT) = std::nullptr_t{}; }
    void clearSTRICT() { mutate_flag(FLAG_IDX_STRICT) = std::monostate{}; }

    bool hasGENERATOR() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_GENERATOR)); }
    void setGENERATOR() { mutate_flag(FLAG_IDX_GENERATOR) = std::nullptr_t{}; }
    void clearGENERATOR() { mutate_flag(FLAG_IDX_GENERATOR) = std::monostate{}; }

    bool hasPROTO() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_PROTO)); }
    void setPROTO() { mutate_flag(FLAG_IDX_PROTO) = std::nullptr_t{}; }
    void clearPROTO() { mutate_flag(FLAG_IDX_PROTO) = std::monostate{}; }

    bool hasNEW() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_NEW)); }
    void setNEW() { mutate_flag(FLAG_IDX_NEW) = std::nullptr_t{}; }
    void clearNEW() { mutate_flag(FLAG_IDX_NEW) = std::monostate{}; }

    bool hasSCALL() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SCALL)); }
    void setSCALL() { mutate_flag(FLAG_IDX_SCALL) = std::nullptr_t{}; }
    void clearSCALL() { mutate_flag(FLAG_IDX_SCALL) = std::monostate{}; }

    bool hasSOBJ() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SOBJ)); }
    void setSOBJ() { mutate_flag(FLAG_IDX_SOBJ) = std::nullptr_t{}; }
    void clearSOBJ() { mutate_flag(FLAG_IDX_SOBJ) = std::monostate{}; }

    bool hasHOME() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_HOME)); }
    void setHOME() { mutate_flag(FLAG_IDX_HOME) = std::nullptr_t{}; }
    void clearHOME() { mutate_flag(FLAG_IDX_HOME) = std::monostate{}; }

    bool hasDERIVED() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_DERIVED)); }
    void setDERIVED() { mutate_flag(FLAG_IDX_DERIVED) = std::nullptr_t{}; }
    void clearDERIVED() { mutate_flag(FLAG_IDX_DERIVED) = std::monostate{}; }

    bool hasTopLevel() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_TopLevel)); }
    void setTopLevel() { mutate_flag(FLAG_IDX_TopLevel) = std::nullptr_t{}; }
    void clearTopLevel() { mutate_flag(FLAG_IDX_TopLevel) = std::monostate{}; }

    double getECMAArgs() const { return std::get<double>(get_flag(FLAG_IDX_ECMAArgs)); }
    void setECMAArgs(double val) { mutate_flag(FLAG_IDX_ECMAArgs) = val; }
    bool hasECMAArgs() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_ECMAArgs)); }
    void clearECMAArgs() { mutate_flag(FLAG_IDX_ECMAArgs) = std::monostate{}; }

    double getStartBBIDX() const { return std::get<double>(get_flag(FLAG_IDX_StartBBIDX)); }
    void setStartBBIDX(double val) { mutate_flag(FLAG_IDX_StartBBIDX) = val; }
    bool hasStartBBIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_StartBBIDX)); }
    void clearStartBBIDX() { mutate_flag(FLAG_IDX_StartBBIDX) = std::monostate{}; }

    double getScopeIDX() const { return std::get<double>(get_flag(FLAG_IDX_ScopeIDX)); }
    void setScopeIDX(double val) { mutate_flag(FLAG_IDX_ScopeIDX) = val; }
    bool hasScopeIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_ScopeIDX)); }
    void clearScopeIDX() { mutate_flag(FLAG_IDX_ScopeIDX) = std::monostate{}; }

    double getContainerFlagID() const { return std::get<double>(get_flag(FLAG_IDX_ContainerFlagID)); }
    void setContainerFlagID(double val) { mutate_flag(FLAG_IDX_ContainerFlagID) = val; }
    bool hasContainerFlagID() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_ContainerFlagID)); }
    void clearContainerFlagID() { mutate_flag(FLAG_IDX_ContainerFlagID) = std::monostate{}; }
  };

  struct BindingsSEXP {
    IRID id;
    IridiumPool* pool;

    explicit BindingsSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Bindings) {
        throw std::runtime_error("Schema Cast Error: Expected Bindings, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LocalBindings, IRID RemoteBindings, IRID Lambdas) {
      return p.add_node(IRI_GEN::IRI_TAG::Bindings, {LocalBindings, RemoteBindings, Lambdas}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 3;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LocalBindings() const { return pool->get_args(id)[0]; }
    bool hasArg_LocalBindings() const { return 0 < pool->get_args(id).size(); }
    void setArg_LocalBindings(IRID val) { assert(hasArg_LocalBindings() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RemoteBindings() const { return pool->get_args(id)[1]; }
    bool hasArg_RemoteBindings() const { return 1 < pool->get_args(id).size(); }
    void setArg_RemoteBindings(IRID val) { assert(hasArg_RemoteBindings() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_Lambdas() const { return pool->get_args(id)[2]; }
    bool hasArg_Lambdas() const { return 2 < pool->get_args(id).size(); }
    void setArg_Lambdas(IRID val) { assert(hasArg_Lambdas() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    // --- Flags ---

  };

  struct StarExportSEXP {
    IRID id;
    IridiumPool* pool;

    explicit StarExportSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::StarExport) {
        throw std::runtime_error("Schema Cast Error: Expected StarExport, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, double MODULEREQIDX) {
      return p.add_node(IRI_GEN::IRI_TAG::StarExport, {}, {FlagValue(MODULEREQIDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_MODULEREQIDX = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    double getMODULEREQIDX() const { return std::get<double>(get_flag(FLAG_IDX_MODULEREQIDX)); }
    void setMODULEREQIDX(double val) { mutate_flag(FLAG_IDX_MODULEREQIDX) = val; }
    bool hasMODULEREQIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_MODULEREQIDX)); }
    void clearMODULEREQIDX() { mutate_flag(FLAG_IDX_MODULEREQIDX) = std::monostate{}; }
  };

  struct StaticImportSEXP {
    IRID id;
    IridiumPool* pool;

    explicit StaticImportSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::StaticImport) {
        throw std::runtime_error("Schema Cast Error: Expected StaticImport, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID StorageLocation, StringID FIELD, bool NSIMPORT, double MODULEREQIDX) {
      return p.add_node(IRI_GEN::IRI_TAG::StaticImport, {StorageLocation}, {FlagValue(FIELD), NSIMPORT ? FlagValue(NSIMPORT) : FlagValue(std::monostate()), FlagValue(MODULEREQIDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 3;

    static constexpr uint32_t FLAG_IDX_FIELD = 0;
    static constexpr uint32_t FLAG_IDX_NSIMPORT = 1;
    static constexpr uint32_t FLAG_IDX_MODULEREQIDX = 2;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_StorageLocation() const { return pool->get_args(id)[0]; }
    bool hasArg_StorageLocation() const { return 0 < pool->get_args(id).size(); }
    void setArg_StorageLocation(IRID val) { assert(hasArg_StorageLocation() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    StringID getFIELD() const { return std::get<StringID>(get_flag(FLAG_IDX_FIELD)); }
    void setFIELD(StringID val) { mutate_flag(FLAG_IDX_FIELD) = val; }
    bool hasFIELD() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_FIELD)); }
    void clearFIELD() { mutate_flag(FLAG_IDX_FIELD) = std::monostate{}; }

    bool hasNSIMPORT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_NSIMPORT)); }
    void setNSIMPORT() { mutate_flag(FLAG_IDX_NSIMPORT) = std::nullptr_t{}; }
    void clearNSIMPORT() { mutate_flag(FLAG_IDX_NSIMPORT) = std::monostate{}; }

    double getMODULEREQIDX() const { return std::get<double>(get_flag(FLAG_IDX_MODULEREQIDX)); }
    void setMODULEREQIDX(double val) { mutate_flag(FLAG_IDX_MODULEREQIDX) = val; }
    bool hasMODULEREQIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_MODULEREQIDX)); }
    void clearMODULEREQIDX() { mutate_flag(FLAG_IDX_MODULEREQIDX) = std::monostate{}; }
  };

  struct LocalStaticExportSEXP {
    IRID id;
    IridiumPool* pool;

    explicit LocalStaticExportSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::LocalStaticExport) {
        throw std::runtime_error("Schema Cast Error: Expected LocalStaticExport, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID StorageLocation, StringID LOCALNAME, StringID EXPORTNAME) {
      return p.add_node(IRI_GEN::IRI_TAG::LocalStaticExport, {StorageLocation}, {FlagValue(LOCALNAME), FlagValue(EXPORTNAME)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_LOCALNAME = 0;
    static constexpr uint32_t FLAG_IDX_EXPORTNAME = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_StorageLocation() const { return pool->get_args(id)[0]; }
    bool hasArg_StorageLocation() const { return 0 < pool->get_args(id).size(); }
    void setArg_StorageLocation(IRID val) { assert(hasArg_StorageLocation() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    StringID getLOCALNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_LOCALNAME)); }
    void setLOCALNAME(StringID val) { mutate_flag(FLAG_IDX_LOCALNAME) = val; }
    bool hasLOCALNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_LOCALNAME)); }
    void clearLOCALNAME() { mutate_flag(FLAG_IDX_LOCALNAME) = std::monostate{}; }

    StringID getEXPORTNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_EXPORTNAME)); }
    void setEXPORTNAME(StringID val) { mutate_flag(FLAG_IDX_EXPORTNAME) = val; }
    bool hasEXPORTNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_EXPORTNAME)); }
    void clearEXPORTNAME() { mutate_flag(FLAG_IDX_EXPORTNAME) = std::monostate{}; }
  };

  struct NamedReexportSEXP {
    IRID id;
    IridiumPool* pool;

    explicit NamedReexportSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::NamedReexport) {
        throw std::runtime_error("Schema Cast Error: Expected NamedReexport, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID LOCALNAME, StringID EXPORTNAME, double MODULEREQIDX) {
      return p.add_node(IRI_GEN::IRI_TAG::NamedReexport, {}, {FlagValue(LOCALNAME), FlagValue(EXPORTNAME), FlagValue(MODULEREQIDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 3;

    static constexpr uint32_t FLAG_IDX_LOCALNAME = 0;
    static constexpr uint32_t FLAG_IDX_EXPORTNAME = 1;
    static constexpr uint32_t FLAG_IDX_MODULEREQIDX = 2;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getLOCALNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_LOCALNAME)); }
    void setLOCALNAME(StringID val) { mutate_flag(FLAG_IDX_LOCALNAME) = val; }
    bool hasLOCALNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_LOCALNAME)); }
    void clearLOCALNAME() { mutate_flag(FLAG_IDX_LOCALNAME) = std::monostate{}; }

    StringID getEXPORTNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_EXPORTNAME)); }
    void setEXPORTNAME(StringID val) { mutate_flag(FLAG_IDX_EXPORTNAME) = val; }
    bool hasEXPORTNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_EXPORTNAME)); }
    void clearEXPORTNAME() { mutate_flag(FLAG_IDX_EXPORTNAME) = std::monostate{}; }

    double getMODULEREQIDX() const { return std::get<double>(get_flag(FLAG_IDX_MODULEREQIDX)); }
    void setMODULEREQIDX(double val) { mutate_flag(FLAG_IDX_MODULEREQIDX) = val; }
    bool hasMODULEREQIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_MODULEREQIDX)); }
    void clearMODULEREQIDX() { mutate_flag(FLAG_IDX_MODULEREQIDX) = std::monostate{}; }
  };

  struct ModuleRequestSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ModuleRequestSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::ModuleRequest) {
        throw std::runtime_error("Schema Cast Error: Expected ModuleRequest, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID SOURCE, double REQIDX) {
      return p.add_node(IRI_GEN::IRI_TAG::ModuleRequest, {}, {FlagValue(SOURCE), FlagValue(REQIDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_SOURCE = 0;
    static constexpr uint32_t FLAG_IDX_REQIDX = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getSOURCE() const { return std::get<StringID>(get_flag(FLAG_IDX_SOURCE)); }
    void setSOURCE(StringID val) { mutate_flag(FLAG_IDX_SOURCE) = val; }
    bool hasSOURCE() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_SOURCE)); }
    void clearSOURCE() { mutate_flag(FLAG_IDX_SOURCE) = std::monostate{}; }

    double getREQIDX() const { return std::get<double>(get_flag(FLAG_IDX_REQIDX)); }
    void setREQIDX(double val) { mutate_flag(FLAG_IDX_REQIDX) = val; }
    bool hasREQIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_REQIDX)); }
    void clearREQIDX() { mutate_flag(FLAG_IDX_REQIDX) = std::monostate{}; }
  };

  struct EnvBindingSEXP {
    IRID id;
    IridiumPool* pool;

    explicit EnvBindingSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::EnvBinding) {
        throw std::runtime_error("Schema Cast Error: Expected EnvBinding, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID NAME, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double REFIDX, double SCOPE, double NEXT, double LINK) {
      return p.add_node(IRI_GEN::IRI_TAG::EnvBinding, {}, {FlagValue(NAME), JSARG ? FlagValue(JSARG) : FlagValue(std::monostate()), JSRESTARG ? FlagValue(JSRESTARG) : FlagValue(std::monostate()), JSLET ? FlagValue(JSLET) : FlagValue(std::monostate()), JSCONST ? FlagValue(JSCONST) : FlagValue(std::monostate()), JSVAR ? FlagValue(JSVAR) : FlagValue(std::monostate()), FlagValue(REFIDX), FlagValue(SCOPE), FlagValue(NEXT), FlagValue(LINK)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 10;

    static constexpr uint32_t FLAG_IDX_NAME = 0;
    static constexpr uint32_t FLAG_IDX_JSARG = 1;
    static constexpr uint32_t FLAG_IDX_JSRESTARG = 2;
    static constexpr uint32_t FLAG_IDX_JSLET = 3;
    static constexpr uint32_t FLAG_IDX_JSCONST = 4;
    static constexpr uint32_t FLAG_IDX_JSVAR = 5;
    static constexpr uint32_t FLAG_IDX_REFIDX = 6;
    static constexpr uint32_t FLAG_IDX_SCOPE = 7;
    static constexpr uint32_t FLAG_IDX_NEXT = 8;
    static constexpr uint32_t FLAG_IDX_LINK = 9;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }

    bool hasJSARG() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSARG)); }
    void setJSARG() { mutate_flag(FLAG_IDX_JSARG) = std::nullptr_t{}; }
    void clearJSARG() { mutate_flag(FLAG_IDX_JSARG) = std::monostate{}; }

    bool hasJSRESTARG() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSRESTARG)); }
    void setJSRESTARG() { mutate_flag(FLAG_IDX_JSRESTARG) = std::nullptr_t{}; }
    void clearJSRESTARG() { mutate_flag(FLAG_IDX_JSRESTARG) = std::monostate{}; }

    bool hasJSLET() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSLET)); }
    void setJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::nullptr_t{}; }
    void clearJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::monostate{}; }

    bool hasJSCONST() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSCONST)); }
    void setJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::nullptr_t{}; }
    void clearJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::monostate{}; }

    bool hasJSVAR() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSVAR)); }
    void setJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::nullptr_t{}; }
    void clearJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::monostate{}; }

    double getREFIDX() const { return std::get<double>(get_flag(FLAG_IDX_REFIDX)); }
    void setREFIDX(double val) { mutate_flag(FLAG_IDX_REFIDX) = val; }
    bool hasREFIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_REFIDX)); }
    void clearREFIDX() { mutate_flag(FLAG_IDX_REFIDX) = std::monostate{}; }

    double getSCOPE() const { return std::get<double>(get_flag(FLAG_IDX_SCOPE)); }
    void setSCOPE(double val) { mutate_flag(FLAG_IDX_SCOPE) = val; }
    bool hasSCOPE() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_SCOPE)); }
    void clearSCOPE() { mutate_flag(FLAG_IDX_SCOPE) = std::monostate{}; }

    double getNEXT() const { return std::get<double>(get_flag(FLAG_IDX_NEXT)); }
    void setNEXT(double val) { mutate_flag(FLAG_IDX_NEXT) = val; }
    bool hasNEXT() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_NEXT)); }
    void clearNEXT() { mutate_flag(FLAG_IDX_NEXT) = std::monostate{}; }

    double getLINK() const { return std::get<double>(get_flag(FLAG_IDX_LINK)); }
    void setLINK(double val) { mutate_flag(FLAG_IDX_LINK) = val; }
    bool hasLINK() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_LINK)); }
    void clearLINK() { mutate_flag(FLAG_IDX_LINK) = std::monostate{}; }
  };

  struct RemoteEnvBindingSEXP {
    IRID id;
    IridiumPool* pool;

    explicit RemoteEnvBindingSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::RemoteEnvBinding) {
        throw std::runtime_error("Schema Cast Error: Expected RemoteEnvBinding, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID ParentReference, bool MODULE, bool MODULEI, bool MODULENSI, double REFIDX, double LINK) {
      return p.add_node(IRI_GEN::IRI_TAG::RemoteEnvBinding, {ParentReference}, {MODULE ? FlagValue(MODULE) : FlagValue(std::monostate()), MODULEI ? FlagValue(MODULEI) : FlagValue(std::monostate()), MODULENSI ? FlagValue(MODULENSI) : FlagValue(std::monostate()), FlagValue(REFIDX), FlagValue(LINK)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 5;

    static constexpr uint32_t FLAG_IDX_MODULE = 0;
    static constexpr uint32_t FLAG_IDX_MODULEI = 1;
    static constexpr uint32_t FLAG_IDX_MODULENSI = 2;
    static constexpr uint32_t FLAG_IDX_REFIDX = 3;
    static constexpr uint32_t FLAG_IDX_LINK = 4;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_ParentReference() const { return pool->get_args(id)[0]; }
    bool hasArg_ParentReference() const { return 0 < pool->get_args(id).size(); }
    void setArg_ParentReference(IRID val) { assert(hasArg_ParentReference() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    bool hasMODULE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_MODULE)); }
    void setMODULE() { mutate_flag(FLAG_IDX_MODULE) = std::nullptr_t{}; }
    void clearMODULE() { mutate_flag(FLAG_IDX_MODULE) = std::monostate{}; }

    bool hasMODULEI() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_MODULEI)); }
    void setMODULEI() { mutate_flag(FLAG_IDX_MODULEI) = std::nullptr_t{}; }
    void clearMODULEI() { mutate_flag(FLAG_IDX_MODULEI) = std::monostate{}; }

    bool hasMODULENSI() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_MODULENSI)); }
    void setMODULENSI() { mutate_flag(FLAG_IDX_MODULENSI) = std::nullptr_t{}; }
    void clearMODULENSI() { mutate_flag(FLAG_IDX_MODULENSI) = std::monostate{}; }

    double getREFIDX() const { return std::get<double>(get_flag(FLAG_IDX_REFIDX)); }
    void setREFIDX(double val) { mutate_flag(FLAG_IDX_REFIDX) = val; }
    bool hasREFIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_REFIDX)); }
    void clearREFIDX() { mutate_flag(FLAG_IDX_REFIDX) = std::monostate{}; }

    double getLINK() const { return std::get<double>(get_flag(FLAG_IDX_LINK)); }
    void setLINK(double val) { mutate_flag(FLAG_IDX_LINK) = val; }
    bool hasLINK() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_LINK)); }
    void clearLINK() { mutate_flag(FLAG_IDX_LINK) = std::monostate{}; }
  };

  struct GlobalBindingSEXP {
    IRID id;
    IridiumPool* pool;

    explicit GlobalBindingSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::GlobalBinding) {
        throw std::runtime_error("Schema Cast Error: Expected GlobalBinding, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID NAME, double LINK) {
      return p.add_node(IRI_GEN::IRI_TAG::GlobalBinding, {}, {FlagValue(NAME), FlagValue(LINK)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_NAME = 0;
    static constexpr uint32_t FLAG_IDX_LINK = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }

    double getLINK() const { return std::get<double>(get_flag(FLAG_IDX_LINK)); }
    void setLINK(double val) { mutate_flag(FLAG_IDX_LINK) = val; }
    bool hasLINK() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_LINK)); }
    void clearLINK() { mutate_flag(FLAG_IDX_LINK) = std::monostate{}; }
  };

  struct ScriptBindingSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ScriptBindingSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::ScriptBinding) {
        throw std::runtime_error("Schema Cast Error: Expected ScriptBinding, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID NAME, bool JSLET, bool JSCONST, bool JSVAR, double LINK) {
      return p.add_node(IRI_GEN::IRI_TAG::ScriptBinding, {}, {FlagValue(NAME), JSLET ? FlagValue(JSLET) : FlagValue(std::monostate()), JSCONST ? FlagValue(JSCONST) : FlagValue(std::monostate()), JSVAR ? FlagValue(JSVAR) : FlagValue(std::monostate()), FlagValue(LINK)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 5;

    static constexpr uint32_t FLAG_IDX_NAME = 0;
    static constexpr uint32_t FLAG_IDX_JSLET = 1;
    static constexpr uint32_t FLAG_IDX_JSCONST = 2;
    static constexpr uint32_t FLAG_IDX_JSVAR = 3;
    static constexpr uint32_t FLAG_IDX_LINK = 4;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }

    bool hasJSLET() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSLET)); }
    void setJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::nullptr_t{}; }
    void clearJSLET() { mutate_flag(FLAG_IDX_JSLET) = std::monostate{}; }

    bool hasJSCONST() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSCONST)); }
    void setJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::nullptr_t{}; }
    void clearJSCONST() { mutate_flag(FLAG_IDX_JSCONST) = std::monostate{}; }

    bool hasJSVAR() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_JSVAR)); }
    void setJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::nullptr_t{}; }
    void clearJSVAR() { mutate_flag(FLAG_IDX_JSVAR) = std::monostate{}; }

    double getLINK() const { return std::get<double>(get_flag(FLAG_IDX_LINK)); }
    void setLINK(double val) { mutate_flag(FLAG_IDX_LINK) = val; }
    bool hasLINK() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_LINK)); }
    void clearLINK() { mutate_flag(FLAG_IDX_LINK) = std::monostate{}; }
  };

  struct EnvWriteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit EnvWriteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::EnvWrite) {
        throw std::runtime_error("Schema Cast Error: Expected EnvWrite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LValTarget, IRID RVal, bool SLOPPY, bool SAFE, bool THISINIT, bool CINIT) {
      return p.add_node(IRI_GEN::IRI_TAG::EnvWrite, {LValTarget, RVal}, {SLOPPY ? FlagValue(SLOPPY) : FlagValue(std::monostate()), FlagValue(SAFE), FlagValue(THISINIT), FlagValue(CINIT)});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 4;

    static constexpr uint32_t FLAG_IDX_SLOPPY = 0;
    static constexpr uint32_t FLAG_IDX_SAFE = 1;
    static constexpr uint32_t FLAG_IDX_THISINIT = 2;
    static constexpr uint32_t FLAG_IDX_CINIT = 3;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LValTarget() const { return pool->get_args(id)[0]; }
    bool hasArg_LValTarget() const { return 0 < pool->get_args(id).size(); }
    void setArg_LValTarget(IRID val) { assert(hasArg_LValTarget() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RVal() const { return pool->get_args(id)[1]; }
    bool hasArg_RVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_RVal(IRID val) { assert(hasArg_RVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasSLOPPY() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SLOPPY)); }
    void setSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::nullptr_t{}; }
    void clearSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::monostate{}; }

    bool getSAFE() const { return std::get<bool>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE(bool val) { mutate_flag(FLAG_IDX_SAFE) = val; }
    bool hasSAFE() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_SAFE)); }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool getTHISINIT() const { return std::get<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void setTHISINIT(bool val) { mutate_flag(FLAG_IDX_THISINIT) = val; }
    bool hasTHISINIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void clearTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::monostate{}; }

    bool getCINIT() const { return std::get<bool>(get_flag(FLAG_IDX_CINIT)); }
    void setCINIT(bool val) { mutate_flag(FLAG_IDX_CINIT) = val; }
    bool hasCINIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_CINIT)); }
    void clearCINIT() { mutate_flag(FLAG_IDX_CINIT) = std::monostate{}; }
  };

  struct SiblingSpecialWriteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit SiblingSpecialWriteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::SiblingSpecialWrite) {
        throw std::runtime_error("Schema Cast Error: Expected SiblingSpecialWrite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LValTarget, IRID RVal, bool SLOPPY, bool SAFE, bool THISINIT, bool CINIT, double ScopeIDX) {
      return p.add_node(IRI_GEN::IRI_TAG::SiblingSpecialWrite, {LValTarget, RVal}, {SLOPPY ? FlagValue(SLOPPY) : FlagValue(std::monostate()), FlagValue(SAFE), FlagValue(THISINIT), FlagValue(CINIT), FlagValue(ScopeIDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 5;

    static constexpr uint32_t FLAG_IDX_SLOPPY = 0;
    static constexpr uint32_t FLAG_IDX_SAFE = 1;
    static constexpr uint32_t FLAG_IDX_THISINIT = 2;
    static constexpr uint32_t FLAG_IDX_CINIT = 3;
    static constexpr uint32_t FLAG_IDX_ScopeIDX = 4;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LValTarget() const { return pool->get_args(id)[0]; }
    bool hasArg_LValTarget() const { return 0 < pool->get_args(id).size(); }
    void setArg_LValTarget(IRID val) { assert(hasArg_LValTarget() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RVal() const { return pool->get_args(id)[1]; }
    bool hasArg_RVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_RVal(IRID val) { assert(hasArg_RVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasSLOPPY() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SLOPPY)); }
    void setSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::nullptr_t{}; }
    void clearSLOPPY() { mutate_flag(FLAG_IDX_SLOPPY) = std::monostate{}; }

    bool getSAFE() const { return std::get<bool>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE(bool val) { mutate_flag(FLAG_IDX_SAFE) = val; }
    bool hasSAFE() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_SAFE)); }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool getTHISINIT() const { return std::get<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void setTHISINIT(bool val) { mutate_flag(FLAG_IDX_THISINIT) = val; }
    bool hasTHISINIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_THISINIT)); }
    void clearTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::monostate{}; }

    bool getCINIT() const { return std::get<bool>(get_flag(FLAG_IDX_CINIT)); }
    void setCINIT(bool val) { mutate_flag(FLAG_IDX_CINIT) = val; }
    bool hasCINIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_CINIT)); }
    void clearCINIT() { mutate_flag(FLAG_IDX_CINIT) = std::monostate{}; }

    double getScopeIDX() const { return std::get<double>(get_flag(FLAG_IDX_ScopeIDX)); }
    void setScopeIDX(double val) { mutate_flag(FLAG_IDX_ScopeIDX) = val; }
    bool hasScopeIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_ScopeIDX)); }
    void clearScopeIDX() { mutate_flag(FLAG_IDX_ScopeIDX) = std::monostate{}; }
  };

  struct JSNUBDSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSNUBDSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSNUBD) {
        throw std::runtime_error("Schema Cast Error: Expected JSNUBD, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::JSNUBD, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct NumberSEXP {
    IRID id;
    IridiumPool* pool;

    explicit NumberSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Number) {
        throw std::runtime_error("Schema Cast Error: Expected Number, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, double IridiumPrimitive) {
      return p.add_node(IRI_GEN::IRI_TAG::Number, {}, {FlagValue(IridiumPrimitive)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_IridiumPrimitive = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    double getIridiumPrimitive() const { return std::get<double>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void setIridiumPrimitive(double val) { mutate_flag(FLAG_IDX_IridiumPrimitive) = val; }
    bool hasIridiumPrimitive() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void clearIridiumPrimitive() { mutate_flag(FLAG_IDX_IridiumPrimitive) = std::monostate{}; }
  };

  struct JSClassSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSClassSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSClass) {
        throw std::runtime_error("Schema Cast Error: Expected JSClass, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Parent, IRID Constructor, StringID NAME, bool DERIVED) {
      return p.add_node(IRI_GEN::IRI_TAG::JSClass, {Parent, Constructor}, {FlagValue(NAME), DERIVED ? FlagValue(DERIVED) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_NAME = 0;
    static constexpr uint32_t FLAG_IDX_DERIVED = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Parent() const { return pool->get_args(id)[0]; }
    bool hasArg_Parent() const { return 0 < pool->get_args(id).size(); }
    void setArg_Parent(IRID val) { assert(hasArg_Parent() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Constructor() const { return pool->get_args(id)[1]; }
    bool hasArg_Constructor() const { return 1 < pool->get_args(id).size(); }
    void setArg_Constructor(IRID val) { assert(hasArg_Constructor() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }

    bool hasDERIVED() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_DERIVED)); }
    void setDERIVED() { mutate_flag(FLAG_IDX_DERIVED) = std::nullptr_t{}; }
    void clearDERIVED() { mutate_flag(FLAG_IDX_DERIVED) = std::monostate{}; }
  };

  struct JSCheckConstructorSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSCheckConstructorSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSCheckConstructor) {
        throw std::runtime_error("Schema Cast Error: Expected JSCheckConstructor, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::JSCheckConstructor, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct ResolvePrivateEnvBindingSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ResolvePrivateEnvBindingSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::ResolvePrivateEnvBinding) {
        throw std::runtime_error("Schema Cast Error: Expected ResolvePrivateEnvBinding, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID NAME, bool FULLY_RESOLVE) {
      return p.add_node(IRI_GEN::IRI_TAG::ResolvePrivateEnvBinding, {}, {FlagValue(NAME), FULLY_RESOLVE ? FlagValue(FULLY_RESOLVE) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_NAME = 0;
    static constexpr uint32_t FLAG_IDX_FULLY_RESOLVE = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }

    bool hasFULLY_RESOLVE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_FULLY_RESOLVE)); }
    void setFULLY_RESOLVE() { mutate_flag(FLAG_IDX_FULLY_RESOLVE) = std::nullptr_t{}; }
    void clearFULLY_RESOLVE() { mutate_flag(FLAG_IDX_FULLY_RESOLVE) = std::monostate{}; }
  };

  struct PVTEnvReadSEXP {
    IRID id;
    IridiumPool* pool;

    explicit PVTEnvReadSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::PVTEnvRead) {
        throw std::runtime_error("Schema Cast Error: Expected PVTEnvRead, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, bool SYMBOL, bool METHOD, bool FULLY_RESOLVE) {
      return p.add_node(IRI_GEN::IRI_TAG::PVTEnvRead, {Obj}, {SYMBOL ? FlagValue(SYMBOL) : FlagValue(std::monostate()), METHOD ? FlagValue(METHOD) : FlagValue(std::monostate()), FULLY_RESOLVE ? FlagValue(FULLY_RESOLVE) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 3;

    static constexpr uint32_t FLAG_IDX_SYMBOL = 0;
    static constexpr uint32_t FLAG_IDX_METHOD = 1;
    static constexpr uint32_t FLAG_IDX_FULLY_RESOLVE = 2;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    bool hasSYMBOL() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SYMBOL)); }
    void setSYMBOL() { mutate_flag(FLAG_IDX_SYMBOL) = std::nullptr_t{}; }
    void clearSYMBOL() { mutate_flag(FLAG_IDX_SYMBOL) = std::monostate{}; }

    bool hasMETHOD() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_METHOD)); }
    void setMETHOD() { mutate_flag(FLAG_IDX_METHOD) = std::nullptr_t{}; }
    void clearMETHOD() { mutate_flag(FLAG_IDX_METHOD) = std::monostate{}; }

    bool hasFULLY_RESOLVE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_FULLY_RESOLVE)); }
    void setFULLY_RESOLVE() { mutate_flag(FLAG_IDX_FULLY_RESOLVE) = std::nullptr_t{}; }
    void clearFULLY_RESOLVE() { mutate_flag(FLAG_IDX_FULLY_RESOLVE) = std::monostate{}; }
  };

  struct JSPrivateSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSPrivateSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSPrivate) {
        throw std::runtime_error("Schema Cast Error: Expected JSPrivate, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID IridiumPrimitive) {
      return p.add_node(IRI_GEN::IRI_TAG::JSPrivate, {}, {FlagValue(IridiumPrimitive)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_IridiumPrimitive = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getIridiumPrimitive() const { return std::get<StringID>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void setIridiumPrimitive(StringID val) { mutate_flag(FLAG_IDX_IridiumPrimitive) = val; }
    bool hasIridiumPrimitive() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void clearIridiumPrimitive() { mutate_flag(FLAG_IDX_IridiumPrimitive) = std::monostate{}; }
  };

  struct JSPrivateFieldWriteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSPrivateFieldWriteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSPrivateFieldWrite) {
        throw std::runtime_error("Schema Cast Error: Expected JSPrivateFieldWrite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, IRID Field, IRID Value, bool DECL) {
      return p.add_node(IRI_GEN::IRI_TAG::JSPrivateFieldWrite, {Obj, Field, Value}, {DECL ? FlagValue(DECL) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 3;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_DECL = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Field() const { return pool->get_args(id)[1]; }
    bool hasArg_Field() const { return 1 < pool->get_args(id).size(); }
    void setArg_Field(IRID val) { assert(hasArg_Field() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_Value() const { return pool->get_args(id)[2]; }
    bool hasArg_Value() const { return 2 < pool->get_args(id).size(); }
    void setArg_Value(IRID val) { assert(hasArg_Value() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    // --- Flags ---
    bool hasDECL() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_DECL)); }
    void setDECL() { mutate_flag(FLAG_IDX_DECL) = std::nullptr_t{}; }
    void clearDECL() { mutate_flag(FLAG_IDX_DECL) = std::monostate{}; }
  };

  struct JSADDBRANDSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSADDBRANDSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSADDBRAND) {
        throw std::runtime_error("Schema Cast Error: Expected JSADDBRAND, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, IRID HomeObj) {
      return p.add_node(IRI_GEN::IRI_TAG::JSADDBRAND, {Obj, HomeObj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_HomeObj() const { return pool->get_args(id)[1]; }
    bool hasArg_HomeObj() const { return 1 < pool->get_args(id).size(); }
    void setArg_HomeObj(IRID val) { assert(hasArg_HomeObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---

  };

  struct JSPrivateFieldReadSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSPrivateFieldReadSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSPrivateFieldRead) {
        throw std::runtime_error("Schema Cast Error: Expected JSPrivateFieldRead, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, IRID Field) {
      return p.add_node(IRI_GEN::IRI_TAG::JSPrivateFieldRead, {Obj, Field}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Field() const { return pool->get_args(id)[1]; }
    bool hasArg_Field() const { return 1 < pool->get_args(id).size(); }
    void setArg_Field(IRID val) { assert(hasArg_Field() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---

  };

  struct PoolBindingSEXP {
    IRID id;
    IridiumPool* pool;

    explicit PoolBindingSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::PoolBinding) {
        throw std::runtime_error("Schema Cast Error: Expected PoolBinding, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Lambda, double REFIDX) {
      return p.add_node(IRI_GEN::IRI_TAG::PoolBinding, {Lambda}, {FlagValue(REFIDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_REFIDX = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Lambda() const { return pool->get_args(id)[0]; }
    bool hasArg_Lambda() const { return 0 < pool->get_args(id).size(); }
    void setArg_Lambda(IRID val) { assert(hasArg_Lambda() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    double getREFIDX() const { return std::get<double>(get_flag(FLAG_IDX_REFIDX)); }
    void setREFIDX(double val) { mutate_flag(FLAG_IDX_REFIDX) = val; }
    bool hasREFIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_REFIDX)); }
    void clearREFIDX() { mutate_flag(FLAG_IDX_REFIDX) = std::monostate{}; }
  };

  struct ResolveContinueTargetSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ResolveContinueTargetSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::ResolveContinueTarget) {
        throw std::runtime_error("Schema Cast Error: Expected ResolveContinueTarget, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID Label) {
      return p.add_node(IRI_GEN::IRI_TAG::ResolveContinueTarget, {}, {FlagValue(Label)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_Label = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getLabel() const { return std::get<StringID>(get_flag(FLAG_IDX_Label)); }
    void setLabel(StringID val) { mutate_flag(FLAG_IDX_Label) = val; }
    bool hasLabel() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_Label)); }
    void clearLabel() { mutate_flag(FLAG_IDX_Label) = std::monostate{}; }
  };

  struct ResolveBreakTargetSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ResolveBreakTargetSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::ResolveBreakTarget) {
        throw std::runtime_error("Schema Cast Error: Expected ResolveBreakTarget, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID Label) {
      return p.add_node(IRI_GEN::IRI_TAG::ResolveBreakTarget, {}, {FlagValue(Label)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_Label = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getLabel() const { return std::get<StringID>(get_flag(FLAG_IDX_Label)); }
    void setLabel(StringID val) { mutate_flag(FLAG_IDX_Label) = val; }
    bool hasLabel() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_Label)); }
    void clearLabel() { mutate_flag(FLAG_IDX_Label) = std::monostate{}; }
  };

  struct JSForOfIteratorCloseSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSForOfIteratorCloseSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSForOfIteratorClose) {
        throw std::runtime_error("Schema Cast Error: Expected JSForOfIteratorClose, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::JSForOfIteratorClose, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct PopCatchContextSEXP {
    IRID id;
    IridiumPool* pool;

    explicit PopCatchContextSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::PopCatchContext) {
        throw std::runtime_error("Schema Cast Error: Expected PopCatchContext, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::PopCatchContext, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct PopFinalizerReturnTargetSEXP {
    IRID id;
    IridiumPool* pool;

    explicit PopFinalizerReturnTargetSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::PopFinalizerReturnTarget) {
        throw std::runtime_error("Schema Cast Error: Expected PopFinalizerReturnTarget, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::PopFinalizerReturnTarget, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct InvokeFinalizerSEXP {
    IRID id;
    IridiumPool* pool;

    explicit InvokeFinalizerSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::InvokeFinalizer) {
        throw std::runtime_error("Schema Cast Error: Expected InvokeFinalizer, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, double IDX) {
      return p.add_node(IRI_GEN::IRI_TAG::InvokeFinalizer, {}, {FlagValue(IDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_IDX = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    double getIDX() const { return std::get<double>(get_flag(FLAG_IDX_IDX)); }
    void setIDX(double val) { mutate_flag(FLAG_IDX_IDX) = val; }
    bool hasIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_IDX)); }
    void clearIDX() { mutate_flag(FLAG_IDX_IDX) = std::monostate{}; }
  };

  struct JSForInStartSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSForInStartSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSForInStart) {
        throw std::runtime_error("Schema Cast Error: Expected JSForInStart, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj) {
      return p.add_node(IRI_GEN::IRI_TAG::JSForInStart, {Obj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---

  };

  struct JSForInNextSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSForInNextSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSForInNext) {
        throw std::runtime_error("Schema Cast Error: Expected JSForInNext, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID IteratorObj) {
      return p.add_node(IRI_GEN::IRI_TAG::JSForInNext, {IteratorObj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_IteratorObj() const { return pool->get_args(id)[0]; }
    bool hasArg_IteratorObj() const { return 0 < pool->get_args(id).size(); }
    void setArg_IteratorObj(IRID val) { assert(hasArg_IteratorObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---

  };

  struct JSForOfStartSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSForOfStartSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSForOfStart) {
        throw std::runtime_error("Schema Cast Error: Expected JSForOfStart, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, bool AWAIT) {
      return p.add_node(IRI_GEN::IRI_TAG::JSForOfStart, {Obj}, {FlagValue(AWAIT)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_AWAIT = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    bool getAWAIT() const { return std::get<bool>(get_flag(FLAG_IDX_AWAIT)); }
    void setAWAIT(bool val) { mutate_flag(FLAG_IDX_AWAIT) = val; }
    bool hasAWAIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_AWAIT)); }
    void clearAWAIT() { mutate_flag(FLAG_IDX_AWAIT) = std::monostate{}; }
  };

  struct JSForOfNextSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSForOfNextSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSForOfNext) {
        throw std::runtime_error("Schema Cast Error: Expected JSForOfNext, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, bool AWAIT) {
      return p.add_node(IRI_GEN::IRI_TAG::JSForOfNext, {Obj}, {FlagValue(AWAIT)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_AWAIT = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    bool getAWAIT() const { return std::get<bool>(get_flag(FLAG_IDX_AWAIT)); }
    void setAWAIT(bool val) { mutate_flag(FLAG_IDX_AWAIT) = val; }
    bool hasAWAIT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_AWAIT)); }
    void clearAWAIT() { mutate_flag(FLAG_IDX_AWAIT) = std::monostate{}; }
  };

  struct PushCatchContextSEXP {
    IRID id;
    IridiumPool* pool;

    explicit PushCatchContextSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::PushCatchContext) {
        throw std::runtime_error("Schema Cast Error: Expected PushCatchContext, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, double IDX) {
      return p.add_node(IRI_GEN::IRI_TAG::PushCatchContext, {}, {FlagValue(IDX)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_IDX = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    double getIDX() const { return std::get<double>(get_flag(FLAG_IDX_IDX)); }
    void setIDX(double val) { mutate_flag(FLAG_IDX_IDX) = val; }
    bool hasIDX() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_IDX)); }
    void clearIDX() { mutate_flag(FLAG_IDX_IDX) = std::monostate{}; }
  };

  struct JSCatchContextSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSCatchContextSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSCatchContext) {
        throw std::runtime_error("Schema Cast Error: Expected JSCatchContext, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID NAME) {
      return p.add_node(IRI_GEN::IRI_TAG::JSCatchContext, {}, {FlagValue(NAME)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_NAME = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }
  };

  struct ThrowSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ThrowSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Throw) {
        throw std::runtime_error("Schema Cast Error: Expected Throw, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID ThrowVal) {
      return p.add_node(IRI_GEN::IRI_TAG::Throw, {ThrowVal}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_ThrowVal() const { return pool->get_args(id)[0]; }
    bool hasArg_ThrowVal() const { return 0 < pool->get_args(id).size(); }
    void setArg_ThrowVal(IRID val) { assert(hasArg_ThrowVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---

  };

  struct RetSEXP {
    IRID id;
    IridiumPool* pool;

    explicit RetSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Ret) {
        throw std::runtime_error("Schema Cast Error: Expected Ret, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::Ret, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct JSBinopSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSBinopSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSBinop) {
        throw std::runtime_error("Schema Cast Error: Expected JSBinop, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LBinop, IRID RBinop, StringID OP) {
      return p.add_node(IRI_GEN::IRI_TAG::JSBinop, {LBinop, RBinop}, {FlagValue(OP)});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_OP = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LBinop() const { return pool->get_args(id)[0]; }
    bool hasArg_LBinop() const { return 0 < pool->get_args(id).size(); }
    void setArg_LBinop(IRID val) { assert(hasArg_LBinop() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RBinop() const { return pool->get_args(id)[1]; }
    bool hasArg_RBinop() const { return 1 < pool->get_args(id).size(); }
    void setArg_RBinop(IRID val) { assert(hasArg_RBinop() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    StringID getOP() const { return std::get<StringID>(get_flag(FLAG_IDX_OP)); }
    void setOP(StringID val) { mutate_flag(FLAG_IDX_OP) = val; }
    bool hasOP() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_OP)); }
    void clearOP() { mutate_flag(FLAG_IDX_OP) = std::monostate{}; }
  };

  struct FieldWriteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit FieldWriteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::FieldWrite) {
        throw std::runtime_error("Schema Cast Error: Expected FieldWrite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, IRID Field, IRID Value) {
      return p.add_node(IRI_GEN::IRI_TAG::FieldWrite, {Obj, Field, Value}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 3;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Field() const { return pool->get_args(id)[1]; }
    bool hasArg_Field() const { return 1 < pool->get_args(id).size(); }
    void setArg_Field(IRID val) { assert(hasArg_Field() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_Value() const { return pool->get_args(id)[2]; }
    bool hasArg_Value() const { return 2 < pool->get_args(id).size(); }
    void setArg_Value(IRID val) { assert(hasArg_Value() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    // --- Flags ---

  };

  struct JSUnopSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSUnopSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSUnop) {
        throw std::runtime_error("Schema Cast Error: Expected JSUnop, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Val, StringID OP) {
      return p.add_node(IRI_GEN::IRI_TAG::JSUnop, {Val}, {FlagValue(OP)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_OP = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Val() const { return pool->get_args(id)[0]; }
    bool hasArg_Val() const { return 0 < pool->get_args(id).size(); }
    void setArg_Val(IRID val) { assert(hasArg_Val() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    StringID getOP() const { return std::get<StringID>(get_flag(FLAG_IDX_OP)); }
    void setOP(StringID val) { mutate_flag(FLAG_IDX_OP) = val; }
    bool hasOP() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_OP)); }
    void clearOP() { mutate_flag(FLAG_IDX_OP) = std::monostate{}; }
  };

  struct UnopSEXP {
    IRID id;
    IridiumPool* pool;

    explicit UnopSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Unop) {
        throw std::runtime_error("Schema Cast Error: Expected Unop, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Val, StringID OP) {
      return p.add_node(IRI_GEN::IRI_TAG::Unop, {Val}, {FlagValue(OP)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_OP = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Val() const { return pool->get_args(id)[0]; }
    bool hasArg_Val() const { return 0 < pool->get_args(id).size(); }
    void setArg_Val(IRID val) { assert(hasArg_Val() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    StringID getOP() const { return std::get<StringID>(get_flag(FLAG_IDX_OP)); }
    void setOP(StringID val) { mutate_flag(FLAG_IDX_OP) = val; }
    bool hasOP() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_OP)); }
    void clearOP() { mutate_flag(FLAG_IDX_OP) = std::monostate{}; }
  };

  struct JSObjectSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSObjectSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSObject) {
        throw std::runtime_error("Schema Cast Error: Expected JSObject, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::JSObject, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct BooleanSEXP {
    IRID id;
    IridiumPool* pool;

    explicit BooleanSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Boolean) {
        throw std::runtime_error("Schema Cast Error: Expected Boolean, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, bool IridiumPrimitive) {
      return p.add_node(IRI_GEN::IRI_TAG::Boolean, {}, {FlagValue(IridiumPrimitive)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_IridiumPrimitive = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    bool getIridiumPrimitive() const { return std::get<bool>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void setIridiumPrimitive(bool val) { mutate_flag(FLAG_IDX_IridiumPrimitive) = val; }
    bool hasIridiumPrimitive() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void clearIridiumPrimitive() { mutate_flag(FLAG_IDX_IridiumPrimitive) = std::monostate{}; }
  };

  struct JSDefineObjPropSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSDefineObjPropSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSDefineObjProp) {
        throw std::runtime_error("Schema Cast Error: Expected JSDefineObjProp, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID TargetObj, IRID Key, IRID Value) {
      return p.add_node(IRI_GEN::IRI_TAG::JSDefineObjProp, {TargetObj, Key, Value}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 3;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_TargetObj() const { return pool->get_args(id)[0]; }
    bool hasArg_TargetObj() const { return 0 < pool->get_args(id).size(); }
    void setArg_TargetObj(IRID val) { assert(hasArg_TargetObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Key() const { return pool->get_args(id)[1]; }
    bool hasArg_Key() const { return 1 < pool->get_args(id).size(); }
    void setArg_Key(IRID val) { assert(hasArg_Key() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_Value() const { return pool->get_args(id)[2]; }
    bool hasArg_Value() const { return 2 < pool->get_args(id).size(); }
    void setArg_Value(IRID val) { assert(hasArg_Value() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    // --- Flags ---

  };

  struct JSArraySEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSArraySEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSArray) {
        throw std::runtime_error("Schema Cast Error: Expected JSArray, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::JSArray, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct BinopSEXP {
    IRID id;
    IridiumPool* pool;

    explicit BinopSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Binop) {
        throw std::runtime_error("Schema Cast Error: Expected Binop, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LBinop, IRID RBinop, StringID OP) {
      return p.add_node(IRI_GEN::IRI_TAG::Binop, {LBinop, RBinop}, {FlagValue(OP)});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_OP = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LBinop() const { return pool->get_args(id)[0]; }
    bool hasArg_LBinop() const { return 0 < pool->get_args(id).size(); }
    void setArg_LBinop(IRID val) { assert(hasArg_LBinop() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RBinop() const { return pool->get_args(id)[1]; }
    bool hasArg_RBinop() const { return 1 < pool->get_args(id).size(); }
    void setArg_RBinop(IRID val) { assert(hasArg_RBinop() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    StringID getOP() const { return std::get<StringID>(get_flag(FLAG_IDX_OP)); }
    void setOP(StringID val) { mutate_flag(FLAG_IDX_OP) = val; }
    bool hasOP() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_OP)); }
    void clearOP() { mutate_flag(FLAG_IDX_OP) = std::monostate{}; }
  };

  struct NullSEXP {
    IRID id;
    IridiumPool* pool;

    explicit NullSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Null) {
        throw std::runtime_error("Schema Cast Error: Expected Null, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, bool IridiumPrimitive) {
      return p.add_node(IRI_GEN::IRI_TAG::Null, {}, {IridiumPrimitive ? FlagValue(IridiumPrimitive) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_IridiumPrimitive = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    bool hasIridiumPrimitive() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void setIridiumPrimitive() { mutate_flag(FLAG_IDX_IridiumPrimitive) = std::nullptr_t{}; }
    void clearIridiumPrimitive() { mutate_flag(FLAG_IDX_IridiumPrimitive) = std::monostate{}; }
  };

  struct JSComputedFieldReadSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSComputedFieldReadSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSComputedFieldRead) {
        throw std::runtime_error("Schema Cast Error: Expected JSComputedFieldRead, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, IRID Field, bool SAFE) {
      return p.add_node(IRI_GEN::IRI_TAG::JSComputedFieldRead, {Obj, Field}, {SAFE ? FlagValue(SAFE) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_SAFE = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Field() const { return pool->get_args(id)[1]; }
    bool hasArg_Field() const { return 1 < pool->get_args(id).size(); }
    void setArg_Field(IRID val) { assert(hasArg_Field() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasSAFE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::nullptr_t{}; }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }
  };

  struct JSComputedFieldWriteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSComputedFieldWriteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSComputedFieldWrite) {
        throw std::runtime_error("Schema Cast Error: Expected JSComputedFieldWrite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, IRID Field, IRID Value, bool SAFE) {
      return p.add_node(IRI_GEN::IRI_TAG::JSComputedFieldWrite, {Obj, Field, Value}, {SAFE ? FlagValue(SAFE) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 3;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_SAFE = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Field() const { return pool->get_args(id)[1]; }
    bool hasArg_Field() const { return 1 < pool->get_args(id).size(); }
    void setArg_Field(IRID val) { assert(hasArg_Field() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_Value() const { return pool->get_args(id)[2]; }
    bool hasArg_Value() const { return 2 < pool->get_args(id).size(); }
    void setArg_Value(IRID val) { assert(hasArg_Value() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    // --- Flags ---
    bool hasSAFE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::nullptr_t{}; }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }
  };

  struct JSSuperFieldReadSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSSuperFieldReadSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSSuperFieldRead) {
        throw std::runtime_error("Schema Cast Error: Expected JSSuperFieldRead, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID This, IRID Super, IRID Field) {
      return p.add_node(IRI_GEN::IRI_TAG::JSSuperFieldRead, {This, Super, Field}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 3;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_This() const { return pool->get_args(id)[0]; }
    bool hasArg_This() const { return 0 < pool->get_args(id).size(); }
    void setArg_This(IRID val) { assert(hasArg_This() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Super() const { return pool->get_args(id)[1]; }
    bool hasArg_Super() const { return 1 < pool->get_args(id).size(); }
    void setArg_Super(IRID val) { assert(hasArg_Super() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_Field() const { return pool->get_args(id)[2]; }
    bool hasArg_Field() const { return 2 < pool->get_args(id).size(); }
    void setArg_Field(IRID val) { assert(hasArg_Field() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    // --- Flags ---

  };

  struct JSSuperFieldWriteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSSuperFieldWriteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSSuperFieldWrite) {
        throw std::runtime_error("Schema Cast Error: Expected JSSuperFieldWrite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID This, IRID Super, IRID Field, IRID Value) {
      return p.add_node(IRI_GEN::IRI_TAG::JSSuperFieldWrite, {This, Super, Field, Value}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 4;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_This() const { return pool->get_args(id)[0]; }
    bool hasArg_This() const { return 0 < pool->get_args(id).size(); }
    void setArg_This(IRID val) { assert(hasArg_This() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Super() const { return pool->get_args(id)[1]; }
    bool hasArg_Super() const { return 1 < pool->get_args(id).size(); }
    void setArg_Super(IRID val) { assert(hasArg_Super() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_Field() const { return pool->get_args(id)[2]; }
    bool hasArg_Field() const { return 2 < pool->get_args(id).size(); }
    void setArg_Field(IRID val) { assert(hasArg_Field() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    IRID getArg_Value() const { return pool->get_args(id)[3]; }
    bool hasArg_Value() const { return 3 < pool->get_args(id).size(); }
    void setArg_Value(IRID val) { assert(hasArg_Value() && "Tried to set missing ARG"); pool->update_arg_inplace(id,3,val); }

    // --- Flags ---

  };

  struct JSToObjectSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSToObjectSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSToObject) {
        throw std::runtime_error("Schema Cast Error: Expected JSToObject, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID TargetObj) {
      return p.add_node(IRI_GEN::IRI_TAG::JSToObject, {TargetObj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_TargetObj() const { return pool->get_args(id)[0]; }
    bool hasArg_TargetObj() const { return 0 < pool->get_args(id).size(); }
    void setArg_TargetObj(IRID val) { assert(hasArg_TargetObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---

  };

  struct JSAppendSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSAppendSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSAppend) {
        throw std::runtime_error("Schema Cast Error: Expected JSAppend, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID TargetObj, IRID InsertionIdx, IRID SpreadObj) {
      return p.add_node(IRI_GEN::IRI_TAG::JSAppend, {TargetObj, InsertionIdx, SpreadObj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 3;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_TargetObj() const { return pool->get_args(id)[0]; }
    bool hasArg_TargetObj() const { return 0 < pool->get_args(id).size(); }
    void setArg_TargetObj(IRID val) { assert(hasArg_TargetObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_InsertionIdx() const { return pool->get_args(id)[1]; }
    bool hasArg_InsertionIdx() const { return 1 < pool->get_args(id).size(); }
    void setArg_InsertionIdx(IRID val) { assert(hasArg_InsertionIdx() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_SpreadObj() const { return pool->get_args(id)[2]; }
    bool hasArg_SpreadObj() const { return 2 < pool->get_args(id).size(); }
    void setArg_SpreadObj(IRID val) { assert(hasArg_SpreadObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    // --- Flags ---

  };

  struct JSDefineObjMethodSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSDefineObjMethodSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSDefineObjMethod) {
        throw std::runtime_error("Schema Cast Error: Expected JSDefineObjMethod, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID TargetObj, IRID Key, IRID Value, bool NOENUM, bool METHOD, bool GET, bool SET) {
      return p.add_node(IRI_GEN::IRI_TAG::JSDefineObjMethod, {TargetObj, Key, Value}, {NOENUM ? FlagValue(NOENUM) : FlagValue(std::monostate()), METHOD ? FlagValue(METHOD) : FlagValue(std::monostate()), GET ? FlagValue(GET) : FlagValue(std::monostate()), SET ? FlagValue(SET) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 3;
    static constexpr uint32_t TOTAL_FLAGS = 4;

    static constexpr uint32_t FLAG_IDX_NOENUM = 0;
    static constexpr uint32_t FLAG_IDX_METHOD = 1;
    static constexpr uint32_t FLAG_IDX_GET = 2;
    static constexpr uint32_t FLAG_IDX_SET = 3;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_TargetObj() const { return pool->get_args(id)[0]; }
    bool hasArg_TargetObj() const { return 0 < pool->get_args(id).size(); }
    void setArg_TargetObj(IRID val) { assert(hasArg_TargetObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Key() const { return pool->get_args(id)[1]; }
    bool hasArg_Key() const { return 1 < pool->get_args(id).size(); }
    void setArg_Key(IRID val) { assert(hasArg_Key() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_Value() const { return pool->get_args(id)[2]; }
    bool hasArg_Value() const { return 2 < pool->get_args(id).size(); }
    void setArg_Value(IRID val) { assert(hasArg_Value() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    // --- Flags ---
    bool hasNOENUM() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_NOENUM)); }
    void setNOENUM() { mutate_flag(FLAG_IDX_NOENUM) = std::nullptr_t{}; }
    void clearNOENUM() { mutate_flag(FLAG_IDX_NOENUM) = std::monostate{}; }

    bool hasMETHOD() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_METHOD)); }
    void setMETHOD() { mutate_flag(FLAG_IDX_METHOD) = std::nullptr_t{}; }
    void clearMETHOD() { mutate_flag(FLAG_IDX_METHOD) = std::monostate{}; }

    bool hasGET() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_GET)); }
    void setGET() { mutate_flag(FLAG_IDX_GET) = std::nullptr_t{}; }
    void clearGET() { mutate_flag(FLAG_IDX_GET) = std::monostate{}; }

    bool hasSET() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SET)); }
    void setSET() { mutate_flag(FLAG_IDX_SET) = std::nullptr_t{}; }
    void clearSET() { mutate_flag(FLAG_IDX_SET) = std::monostate{}; }
  };

  struct JSCopyDataPropertiesSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSCopyDataPropertiesSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSCopyDataProperties) {
        throw std::runtime_error("Schema Cast Error: Expected JSCopyDataProperties, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID ExclusionObj, IRID SourceObj, IRID TargetObj) {
      return p.add_node(IRI_GEN::IRI_TAG::JSCopyDataProperties, {ExclusionObj, SourceObj, TargetObj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 3;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_ExclusionObj() const { return pool->get_args(id)[0]; }
    bool hasArg_ExclusionObj() const { return 0 < pool->get_args(id).size(); }
    void setArg_ExclusionObj(IRID val) { assert(hasArg_ExclusionObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_SourceObj() const { return pool->get_args(id)[1]; }
    bool hasArg_SourceObj() const { return 1 < pool->get_args(id).size(); }
    void setArg_SourceObj(IRID val) { assert(hasArg_SourceObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    IRID getArg_TargetObj() const { return pool->get_args(id)[2]; }
    bool hasArg_TargetObj() const { return 2 < pool->get_args(id).size(); }
    void setArg_TargetObj(IRID val) { assert(hasArg_TargetObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,2,val); }

    // --- Flags ---

  };

  struct RegExpSEXP {
    IRID id;
    IridiumPool* pool;

    explicit RegExpSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::RegExp) {
        throw std::runtime_error("Schema Cast Error: Expected RegExp, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID EXP, StringID FLAGS) {
      return p.add_node(IRI_GEN::IRI_TAG::RegExp, {}, {FlagValue(EXP), FlagValue(FLAGS)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_EXP = 0;
    static constexpr uint32_t FLAG_IDX_FLAGS = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getEXP() const { return std::get<StringID>(get_flag(FLAG_IDX_EXP)); }
    void setEXP(StringID val) { mutate_flag(FLAG_IDX_EXP) = val; }
    bool hasEXP() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_EXP)); }
    void clearEXP() { mutate_flag(FLAG_IDX_EXP) = std::monostate{}; }

    StringID getFLAGS() const { return std::get<StringID>(get_flag(FLAG_IDX_FLAGS)); }
    void setFLAGS(StringID val) { mutate_flag(FLAG_IDX_FLAGS) = val; }
    bool hasFLAGS() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_FLAGS)); }
    void clearFLAGS() { mutate_flag(FLAG_IDX_FLAGS) = std::monostate{}; }
  };

  struct UNOPDelMemberExprSEXP {
    IRID id;
    IridiumPool* pool;

    explicit UNOPDelMemberExprSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::UNOPDelMemberExpr) {
        throw std::runtime_error("Schema Cast Error: Expected UNOPDelMemberExpr, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Receiver, IRID Field) {
      return p.add_node(IRI_GEN::IRI_TAG::UNOPDelMemberExpr, {Receiver, Field}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Receiver() const { return pool->get_args(id)[0]; }
    bool hasArg_Receiver() const { return 0 < pool->get_args(id).size(); }
    void setArg_Receiver(IRID val) { assert(hasArg_Receiver() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_Field() const { return pool->get_args(id)[1]; }
    bool hasArg_Field() const { return 1 < pool->get_args(id).size(); }
    void setArg_Field(IRID val) { assert(hasArg_Field() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---

  };

  struct UNOPDelVarSEXP {
    IRID id;
    IridiumPool* pool;

    explicit UNOPDelVarSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::UNOPDelVar) {
        throw std::runtime_error("Schema Cast Error: Expected UNOPDelVar, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID NAME) {
      return p.add_node(IRI_GEN::IRI_TAG::UNOPDelVar, {}, {FlagValue(NAME)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_NAME = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getNAME() const { return std::get<StringID>(get_flag(FLAG_IDX_NAME)); }
    void setNAME(StringID val) { mutate_flag(FLAG_IDX_NAME) = val; }
    bool hasNAME() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_NAME)); }
    void clearNAME() { mutate_flag(FLAG_IDX_NAME) = std::monostate{}; }
  };

  struct JSTemplateSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSTemplateSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSTemplate) {
        throw std::runtime_error("Schema Cast Error: Expected JSTemplate, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::JSTemplate, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct JSBigIntSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSBigIntSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSBigInt) {
        throw std::runtime_error("Schema Cast Error: Expected JSBigInt, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, StringID IridiumPrimitive) {
      return p.add_node(IRI_GEN::IRI_TAG::JSBigInt, {}, {FlagValue(IridiumPrimitive)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_IridiumPrimitive = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    StringID getIridiumPrimitive() const { return std::get<StringID>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void setIridiumPrimitive(StringID val) { mutate_flag(FLAG_IDX_IridiumPrimitive) = val; }
    bool hasIridiumPrimitive() const { return std::holds_alternative<StringID>(get_flag(FLAG_IDX_IridiumPrimitive)); }
    void clearIridiumPrimitive() { mutate_flag(FLAG_IDX_IridiumPrimitive) = std::monostate{}; }
  };

  struct AwaitSEXP {
    IRID id;
    IridiumPool* pool;

    explicit AwaitSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Await) {
        throw std::runtime_error("Schema Cast Error: Expected Await, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj) {
      return p.add_node(IRI_GEN::IRI_TAG::Await, {Obj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---

  };

  struct YieldSEXP {
    IRID id;
    IridiumPool* pool;

    explicit YieldSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::Yield) {
        throw std::runtime_error("Schema Cast Error: Expected Yield, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj) {
      return p.add_node(IRI_GEN::IRI_TAG::Yield, {Obj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---

  };

  struct JSInitialYieldSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSInitialYieldSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSInitialYield) {
        throw std::runtime_error("Schema Cast Error: Expected JSInitialYield, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::JSInitialYield, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct IDOPSEXP {
    IRID id;
    IridiumPool* pool;

    explicit IDOPSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::IDOP) {
        throw std::runtime_error("Schema Cast Error: Expected IDOP, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, bool PREFIX, bool INCREMENT) {
      return p.add_node(IRI_GEN::IRI_TAG::IDOP, {Obj}, {FlagValue(PREFIX), FlagValue(INCREMENT)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_PREFIX = 0;
    static constexpr uint32_t FLAG_IDX_INCREMENT = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    bool getPREFIX() const { return std::get<bool>(get_flag(FLAG_IDX_PREFIX)); }
    void setPREFIX(bool val) { mutate_flag(FLAG_IDX_PREFIX) = val; }
    bool hasPREFIX() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_PREFIX)); }
    void clearPREFIX() { mutate_flag(FLAG_IDX_PREFIX) = std::monostate{}; }

    bool getINCREMENT() const { return std::get<bool>(get_flag(FLAG_IDX_INCREMENT)); }
    void setINCREMENT(bool val) { mutate_flag(FLAG_IDX_INCREMENT) = val; }
    bool hasINCREMENT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_INCREMENT)); }
    void clearINCREMENT() { mutate_flag(FLAG_IDX_INCREMENT) = std::monostate{}; }
  };

  struct JSIDOPSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSIDOPSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSIDOP) {
        throw std::runtime_error("Schema Cast Error: Expected JSIDOP, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj, bool PREFIX, bool INCREMENT) {
      return p.add_node(IRI_GEN::IRI_TAG::JSIDOP, {Obj}, {FlagValue(PREFIX), FlagValue(INCREMENT)});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_PREFIX = 0;
    static constexpr uint32_t FLAG_IDX_INCREMENT = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---
    bool getPREFIX() const { return std::get<bool>(get_flag(FLAG_IDX_PREFIX)); }
    void setPREFIX(bool val) { mutate_flag(FLAG_IDX_PREFIX) = val; }
    bool hasPREFIX() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_PREFIX)); }
    void clearPREFIX() { mutate_flag(FLAG_IDX_PREFIX) = std::monostate{}; }

    bool getINCREMENT() const { return std::get<bool>(get_flag(FLAG_IDX_INCREMENT)); }
    void setINCREMENT(bool val) { mutate_flag(FLAG_IDX_INCREMENT) = val; }
    bool hasINCREMENT() const { return std::holds_alternative<bool>(get_flag(FLAG_IDX_INCREMENT)); }
    void clearINCREMENT() { mutate_flag(FLAG_IDX_INCREMENT) = std::monostate{}; }
  };

  struct StackToHeapSEXP {
    IRID id;
    IridiumPool* pool;

    explicit StackToHeapSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::StackToHeap) {
        throw std::runtime_error("Schema Cast Error: Expected StackToHeap, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::StackToHeap, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct LoopInitPreludeEndSEXP {
    IRID id;
    IridiumPool* pool;

    explicit LoopInitPreludeEndSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::LoopInitPreludeEnd) {
        throw std::runtime_error("Schema Cast Error: Expected LoopInitPreludeEnd, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::LoopInitPreludeEnd, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct JSSetHomeSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSSetHomeSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSSetHome) {
        throw std::runtime_error("Schema Cast Error: Expected JSSetHome, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID HomeObj, IRID FuncObj) {
      return p.add_node(IRI_GEN::IRI_TAG::JSSetHome, {HomeObj, FuncObj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_HomeObj() const { return pool->get_args(id)[0]; }
    bool hasArg_HomeObj() const { return 0 < pool->get_args(id).size(); }
    void setArg_HomeObj(IRID val) { assert(hasArg_HomeObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_FuncObj() const { return pool->get_args(id)[1]; }
    bool hasArg_FuncObj() const { return 1 < pool->get_args(id).size(); }
    void setArg_FuncObj(IRID val) { assert(hasArg_FuncObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---

  };

  struct JSSetNameSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSSetNameSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSSetName) {
        throw std::runtime_error("Schema Cast Error: Expected JSSetName, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID obj, IRID name) {
      return p.add_node(IRI_GEN::IRI_TAG::JSSetName, {obj, name}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_obj() const { return pool->get_args(id)[0]; }
    bool hasArg_obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_obj(IRID val) { assert(hasArg_obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_name() const { return pool->get_args(id)[1]; }
    bool hasArg_name() const { return 1 < pool->get_args(id).size(); }
    void setArg_name(IRID val) { assert(hasArg_name() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---

  };

  struct JSSetPrototypeOfSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSSetPrototypeOfSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSSetPrototypeOf) {
        throw std::runtime_error("Schema Cast Error: Expected JSSetPrototypeOf, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID TargetObj, IRID ProtoValue) {
      return p.add_node(IRI_GEN::IRI_TAG::JSSetPrototypeOf, {TargetObj, ProtoValue}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_TargetObj() const { return pool->get_args(id)[0]; }
    bool hasArg_TargetObj() const { return 0 < pool->get_args(id).size(); }
    void setArg_TargetObj(IRID val) { assert(hasArg_TargetObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_ProtoValue() const { return pool->get_args(id)[1]; }
    bool hasArg_ProtoValue() const { return 1 < pool->get_args(id).size(); }
    void setArg_ProtoValue(IRID val) { assert(hasArg_ProtoValue() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---

  };

  struct GWriteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit GWriteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::GWrite) {
        throw std::runtime_error("Schema Cast Error: Expected GWrite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LValTarget, IRID RVal, bool INIT, bool SAFE, bool DECLVAR, bool DECLFUN) {
      return p.add_node(IRI_GEN::IRI_TAG::GWrite, {LValTarget, RVal}, {INIT ? FlagValue(INIT) : FlagValue(std::monostate()), SAFE ? FlagValue(SAFE) : FlagValue(std::monostate()), DECLVAR ? FlagValue(DECLVAR) : FlagValue(std::monostate()), DECLFUN ? FlagValue(DECLFUN) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 4;

    static constexpr uint32_t FLAG_IDX_INIT = 0;
    static constexpr uint32_t FLAG_IDX_SAFE = 1;
    static constexpr uint32_t FLAG_IDX_DECLVAR = 2;
    static constexpr uint32_t FLAG_IDX_DECLFUN = 3;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LValTarget() const { return pool->get_args(id)[0]; }
    bool hasArg_LValTarget() const { return 0 < pool->get_args(id).size(); }
    void setArg_LValTarget(IRID val) { assert(hasArg_LValTarget() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RVal() const { return pool->get_args(id)[1]; }
    bool hasArg_RVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_RVal(IRID val) { assert(hasArg_RVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasINIT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_INIT)); }
    void setINIT() { mutate_flag(FLAG_IDX_INIT) = std::nullptr_t{}; }
    void clearINIT() { mutate_flag(FLAG_IDX_INIT) = std::monostate{}; }

    bool hasSAFE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::nullptr_t{}; }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool hasDECLVAR() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_DECLVAR)); }
    void setDECLVAR() { mutate_flag(FLAG_IDX_DECLVAR) = std::nullptr_t{}; }
    void clearDECLVAR() { mutate_flag(FLAG_IDX_DECLVAR) = std::monostate{}; }

    bool hasDECLFUN() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_DECLFUN)); }
    void setDECLFUN() { mutate_flag(FLAG_IDX_DECLFUN) = std::nullptr_t{}; }
    void clearDECLFUN() { mutate_flag(FLAG_IDX_DECLFUN) = std::monostate{}; }
  };

  struct LWriteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit LWriteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::LWrite) {
        throw std::runtime_error("Schema Cast Error: Expected LWrite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LValTarget, IRID RVal, bool INIT, bool SAFE, bool THISINIT) {
      return p.add_node(IRI_GEN::IRI_TAG::LWrite, {LValTarget, RVal}, {INIT ? FlagValue(INIT) : FlagValue(std::monostate()), SAFE ? FlagValue(SAFE) : FlagValue(std::monostate()), THISINIT ? FlagValue(THISINIT) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 3;

    static constexpr uint32_t FLAG_IDX_INIT = 0;
    static constexpr uint32_t FLAG_IDX_SAFE = 1;
    static constexpr uint32_t FLAG_IDX_THISINIT = 2;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LValTarget() const { return pool->get_args(id)[0]; }
    bool hasArg_LValTarget() const { return 0 < pool->get_args(id).size(); }
    void setArg_LValTarget(IRID val) { assert(hasArg_LValTarget() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RVal() const { return pool->get_args(id)[1]; }
    bool hasArg_RVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_RVal(IRID val) { assert(hasArg_RVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasINIT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_INIT)); }
    void setINIT() { mutate_flag(FLAG_IDX_INIT) = std::nullptr_t{}; }
    void clearINIT() { mutate_flag(FLAG_IDX_INIT) = std::monostate{}; }

    bool hasSAFE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::nullptr_t{}; }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool hasTHISINIT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_THISINIT)); }
    void setTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::nullptr_t{}; }
    void clearTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::monostate{}; }
  };

  struct RWriteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit RWriteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::RWrite) {
        throw std::runtime_error("Schema Cast Error: Expected RWrite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LValTarget, IRID RVal, bool INIT, bool SAFE, bool THISINIT) {
      return p.add_node(IRI_GEN::IRI_TAG::RWrite, {LValTarget, RVal}, {INIT ? FlagValue(INIT) : FlagValue(std::monostate()), SAFE ? FlagValue(SAFE) : FlagValue(std::monostate()), THISINIT ? FlagValue(THISINIT) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 3;

    static constexpr uint32_t FLAG_IDX_INIT = 0;
    static constexpr uint32_t FLAG_IDX_SAFE = 1;
    static constexpr uint32_t FLAG_IDX_THISINIT = 2;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LValTarget() const { return pool->get_args(id)[0]; }
    bool hasArg_LValTarget() const { return 0 < pool->get_args(id).size(); }
    void setArg_LValTarget(IRID val) { assert(hasArg_LValTarget() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RVal() const { return pool->get_args(id)[1]; }
    bool hasArg_RVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_RVal(IRID val) { assert(hasArg_RVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasINIT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_INIT)); }
    void setINIT() { mutate_flag(FLAG_IDX_INIT) = std::nullptr_t{}; }
    void clearINIT() { mutate_flag(FLAG_IDX_INIT) = std::monostate{}; }

    bool hasSAFE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::nullptr_t{}; }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }

    bool hasTHISINIT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_THISINIT)); }
    void setTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::nullptr_t{}; }
    void clearTHISINIT() { mutate_flag(FLAG_IDX_THISINIT) = std::monostate{}; }
  };

  struct MWriteSEXP {
    IRID id;
    IridiumPool* pool;

    explicit MWriteSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::MWrite) {
        throw std::runtime_error("Schema Cast Error: Expected MWrite, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID LValTarget, IRID RVal, bool INIT, bool SAFE) {
      return p.add_node(IRI_GEN::IRI_TAG::MWrite, {LValTarget, RVal}, {INIT ? FlagValue(INIT) : FlagValue(std::monostate()), SAFE ? FlagValue(SAFE) : FlagValue(std::monostate())});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 2;

    static constexpr uint32_t FLAG_IDX_INIT = 0;
    static constexpr uint32_t FLAG_IDX_SAFE = 1;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_LValTarget() const { return pool->get_args(id)[0]; }
    bool hasArg_LValTarget() const { return 0 < pool->get_args(id).size(); }
    void setArg_LValTarget(IRID val) { assert(hasArg_LValTarget() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_RVal() const { return pool->get_args(id)[1]; }
    bool hasArg_RVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_RVal(IRID val) { assert(hasArg_RVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---
    bool hasINIT() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_INIT)); }
    void setINIT() { mutate_flag(FLAG_IDX_INIT) = std::nullptr_t{}; }
    void clearINIT() { mutate_flag(FLAG_IDX_INIT) = std::monostate{}; }

    bool hasSAFE() const { return !std::holds_alternative<std::monostate>(get_flag(FLAG_IDX_SAFE)); }
    void setSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::nullptr_t{}; }
    void clearSAFE() { mutate_flag(FLAG_IDX_SAFE) = std::monostate{}; }
  };

  struct DCTRRetSEXP {
    IRID id;
    IridiumPool* pool;

    explicit DCTRRetSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::DCTRRet) {
        throw std::runtime_error("Schema Cast Error: Expected DCTRRet, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID userObj, IRID thisObj) {
      return p.add_node(IRI_GEN::IRI_TAG::DCTRRet, {userObj, thisObj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_userObj() const { return pool->get_args(id)[0]; }
    bool hasArg_userObj() const { return 0 < pool->get_args(id).size(); }
    void setArg_userObj(IRID val) { assert(hasArg_userObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_thisObj() const { return pool->get_args(id)[1]; }
    bool hasArg_thisObj() const { return 1 < pool->get_args(id).size(); }
    void setArg_thisObj(IRID val) { assert(hasArg_thisObj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---

  };

  struct ToNumericSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ToNumericSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::ToNumeric) {
        throw std::runtime_error("Schema Cast Error: Expected ToNumeric, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID Obj) {
      return p.add_node(IRI_GEN::IRI_TAG::ToNumeric, {Obj}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 1;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_Obj() const { return pool->get_args(id)[0]; }
    bool hasArg_Obj() const { return 0 < pool->get_args(id).size(); }
    void setArg_Obj(IRID val) { assert(hasArg_Obj() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    // --- Flags ---

  };

  struct JSCTXSEXP {
    IRID id;
    IridiumPool* pool;

    explicit JSCTXSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::JSCTX) {
        throw std::runtime_error("Schema Cast Error: Expected JSCTX, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, double OPID) {
      return p.add_node(IRI_GEN::IRI_TAG::JSCTX, {}, {FlagValue(OPID)});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 1;

    static constexpr uint32_t FLAG_IDX_OPID = 0;

    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---
    double getOPID() const { return std::get<double>(get_flag(FLAG_IDX_OPID)); }
    void setOPID(double val) { mutate_flag(FLAG_IDX_OPID) = val; }
    bool hasOPID() const { return std::holds_alternative<double>(get_flag(FLAG_IDX_OPID)); }
    void clearOPID() { mutate_flag(FLAG_IDX_OPID) = std::monostate{}; }
  };

  struct QJSModuleInitSEXP {
    IRID id;
    IridiumPool* pool;

    explicit QJSModuleInitSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::QJSModuleInit) {
        throw std::runtime_error("Schema Cast Error: Expected QJSModuleInit, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::QJSModuleInit, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct CompoundAssnSEXP {
    IRID id;
    IridiumPool* pool;

    explicit CompoundAssnSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::CompoundAssn) {
        throw std::runtime_error("Schema Cast Error: Expected CompoundAssn, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p) {
      return p.add_node(IRI_GEN::IRI_TAG::CompoundAssn, {}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 0;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---


    // --- Flags ---

  };

  struct ThisINITSEXP {
    IRID id;
    IridiumPool* pool;

    explicit ThisINITSEXP(IRID n, IridiumPool& p) : id(n), pool(&p) {
      if (pool->operator[](n).tag != IRI_GEN::ThisINIT) {
        throw std::runtime_error("Schema Cast Error: Expected ThisINIT, but got " + IRI_GEN::dump_tag(pool->operator[](n).tag));
      }
    }

    static IRID create(IridiumPool& p, IRID oldVal, IRID newVal) {
      return p.add_node(IRI_GEN::IRI_TAG::ThisINIT, {oldVal, newVal}, {});
    }

    static constexpr uint32_t TOTAL_ARGS = 2;
    static constexpr uint32_t TOTAL_FLAGS = 0;



    // --- Helpers ---
    inline FlagValue& mutate_flag(uint32_t idx) {
        return pool->get_flags_m(id)[idx];
    }
    inline const FlagValue& get_flag(uint32_t idx) const {
        return pool->get_flags(id)[idx];
    }

    // --- Arguments ---
    IRID getArg_oldVal() const { return pool->get_args(id)[0]; }
    bool hasArg_oldVal() const { return 0 < pool->get_args(id).size(); }
    void setArg_oldVal(IRID val) { assert(hasArg_oldVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,0,val); }

    IRID getArg_newVal() const { return pool->get_args(id)[1]; }
    bool hasArg_newVal() const { return 1 < pool->get_args(id).size(); }
    void setArg_newVal(IRID val) { assert(hasArg_newVal() && "Tried to set missing ARG"); pool->update_arg_inplace(id,1,val); }

    // --- Flags ---

  };

} // namespace IRI_GEN