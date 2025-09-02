#include "Iridium/Structure/BindingsView.h"
#include "generated/IridiumTypes.h"

BindingsView::BindingsView(std::shared_ptr<BindingsSEXP> target) : targetContainer(target) 
{

}


void BindingsView::removeBinding(IRISEXP binding, bool remote)
{
  if (!remote)
  {
    // Ensure the binding exists
    auto & localBindings = targetContainer->getLocalBindings()->args;
    assert(std::find(localBindings.begin(), localBindings.end(), binding) != localBindings.end());

    // Things that point to the current binding, should now point to the next of the deleted binding
  }
  else
  {
    throw std::runtime_error("TODO, handle remote bindings deletion");
  }
}