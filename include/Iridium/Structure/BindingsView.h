#pragma once
#include "generated/IridiumTypes.h"
#include "Iridium/Globals.h"

class BindingsView
{
private:
  std::shared_ptr<BindingsSEXP> targetContainer;

public:
  BindingsView(std::shared_ptr<BindingsSEXP> target);

  void removeBinding(IRISEXP binding, bool remote = false);

};