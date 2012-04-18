#include "clock.h"

using namespace EvolvingClocks;

void Component::setName(string name) {
  name_ = name;
}

string Component::getName() {
  return name_;
}
