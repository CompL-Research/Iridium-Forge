#include "Iridium/Globals.h"
#include "Iridium/IridiumTypes.h"

template <typename T, typename... Args>
IRISEXP make_iriexp(Args&&... args) {
    return std::static_pointer_cast<IridiumSEXP>(
        std::make_shared<T>(std::forward<Args>(args)...)
    );
}

IRISEXP specializeSEXP(IRISEXP obj) 
{
  auto & tag = obj->tag;
  if (tag == "File")
  {
    return make_iriexp<FileSEXP>(obj);
  }
  if (tag == "ResolveEnvBinding")
  {
    return make_iriexp<ResolveEnvBindingSEXP>(obj);
  }
  if (tag == "List")
  {
    return make_iriexp<ListSEXP>(obj);
  }
  if (tag == "JSImplicitBindingDeclaration")
  {
    return make_iriexp<JSImplicitBindingDeclarationSEXP>(obj);
  }
  if (tag == "EnvRead")
  {
    return make_iriexp<EnvReadSEXP>(obj);
  }
  if (tag == "IfJump")
  {
    return make_iriexp<IfJumpSEXP>(obj);
  }

  if (tag == "String")
  {
    return make_iriexp<StringSEXP>(obj);
  }

  if (tag == "FieldRead")
  {
    return make_iriexp<FieldReadSEXP>(obj);
  }

  if (tag == "JSExplicitBindingDeclaration")
  {
    return make_iriexp<JSExplicitBindingDeclarationSEXP>(obj);
  }

  if (tag == "CallSite")
  {
    return make_iriexp<CallSiteSEXP>(obj);
  }

  if (tag == "ReturnAsync")
  {
    return make_iriexp<ReturnAsyncSEXP>(obj);
  }

  if (tag == "BB")
  {
    return make_iriexp<BBSEXP>(obj);
  }

  if (tag == "Return")
  {
    return make_iriexp<ReturnSEXP>(obj);
  }

  if (tag == "IfElseJump")
  {
    return make_iriexp<IfElseJumpSEXP>(obj);
  }

  if (tag == "Goto")
  {
    return make_iriexp<GotoSEXP>(obj);
  }

  if (tag == "JSFuncDecl")
  {
    return make_iriexp<JSFuncDeclSEXP>(obj);
  }

  if (tag == "Lambda")
  {
    return make_iriexp<LambdaSEXP>(obj);
  }

  if (tag == "NOP")
  {
    return make_iriexp<NOPSEXP>(obj);
  }

  if (tag == "BBContainer")
  {
    return make_iriexp<BBContainerSEXP>(obj);
  }

  if (tag == "Bindings")
  {
    return make_iriexp<BindingsSEXP>(obj);
  }

  throw std::runtime_error("Unhandled Iridium Tag: " + tag);
}

BBSEXPFLAGS getBBFlag(std::shared_ptr<BBSEXP> b) 
{
  if (b->hasTopLevel()) return BBSEXPFLAGS::TopLevel;
  if (b->hasClosureBoundary()) return BBSEXPFLAGS::ClosureBoundary;
  if (b->hasLexical()) return BBSEXPFLAGS::Lexical;
  throw std::runtime_error("Failed to get a valid flag from a BBSEXP");
}

void setBBFlag(std::shared_ptr<BBSEXP> b, BBSEXPFLAGS flagToSet) 
{
  b->unsetTopLevel();
  b->unsetClosureBoundary();
  b->unsetLexical();
  if (flagToSet == BBSEXPFLAGS::TopLevel) return b->setTopLevel();
  if (flagToSet == BBSEXPFLAGS::ClosureBoundary) return b->setClosureBoundary();
  if (flagToSet == BBSEXPFLAGS::Lexical) return b->setLexical();
  throw std::runtime_error("Impossible case reached setBBFlag");
}

std::shared_ptr<NOPSEXP> makeNOPSEXP()
{
  auto sexp = std::make_shared<IridiumSEXP>();
  sexp->tag = "NOP";
  return std::make_shared<NOPSEXP>(sexp);
}

std::shared_ptr<ListSEXP> makeListSEXP()
{
  auto sexp = std::make_shared<IridiumSEXP>();
  sexp->tag = "List";
  return std::make_shared<ListSEXP>(sexp);
}

std::shared_ptr<BindingsSEXP> makeBindingsSEXP(double parentScope)
{
  auto sexp = std::make_shared<IridiumSEXP>();
  sexp->tag = "Bindings";
  sexp->args.push_back(makeListSEXP());
  sexp->args.push_back(makeListSEXP());
  sexp->args.push_back(makeListSEXP());
  sexp->setFlag("ParentScope", parentScope);
  return std::make_shared<BindingsSEXP>(sexp);
}


std::shared_ptr<BBContainerSEXP> makeBBContainerSEXP(double parentScope)
{
  auto sexp = std::make_shared<IridiumSEXP>();
  sexp->tag = "BBContainer";
  sexp->args.push_back(makeBindingsSEXP(parentScope));
  sexp->args.push_back(makeListSEXP());

  return std::make_shared<BBContainerSEXP>(sexp);
}