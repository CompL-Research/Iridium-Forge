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
  throw std::runtime_error("Unhandled Iridium Tag: " + tag);
}