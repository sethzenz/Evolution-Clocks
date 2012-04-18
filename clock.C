#include "clock.h"

using namespace EvolvingClocks;

Connection::Connection() {
  other_ = NULL;
  myInterface_ = empty;
  otherInterface_ = empty;
}

Connection::Connection(Component* other,interfaceType myInterface, interfaceType otherInterface) {
  other_ = other;
  myInterface_ = myInterface;
  otherInterface_ = otherInterface;
}

bool Connection::isOK() {
  if (other_ == NULL) return false;
  if (myInterface_ == clockBase && otherInterface_ == gearBottom) return true;
  if (myInterface_ == handEnd && (otherInterface_ == gearEdge || otherInterface_ == gearTop )) return true;
  if (myInterface_ == gearTop && (otherInterface_ == handEnd && otherInterface_ == gearBottom)) return true;
  if (myInterface_ == gearBottom && (otherInterface_ == gearTop || otherInterface_ == clockBase)) return true;
  if (myInterface_ == gearEdge && (otherInterface_ == gearEdge || otherInterface_ == handEnd)) return true;
  if (myInterface_ != gearBottom && otherInterface_ == empty) return true;
  return false;
      
}

bool Component::isOK() {
  for (deque<Connection>::iterator it = connections_.begin() ; it != connections_.end() ; it++) {
    if (!it->isOK()) return false;
  }
  return true;
}

bool Gear::isOK() {
  // ?
}

bool Clock::isOK() {
  unsigned int nbackplate = 0;
  for (deque<Component>::iterator it =components_.begin() ; it != components_.end() ; it++) {
    if (!it->isOK()) return false;
    if (dynamic_cast<Backplate*>(&*it)) nbackplate++;
    if (dynamic_cast<Gear*>(&*it)) {
      // 
    }
  }
  if (nbackplate != 1) return false;
  return true;
}
