#include "clock.h"
#include <iostream>

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
  if (!other_) return false;
  if ((otherInterface_ == gearBottom || otherInterface_ == gearTop || otherInterface_ == gearEdge) && !dynamic_cast<Gear*>(other_))  return false;
  if ((otherInterface_ == clockBase) && !dynamic_cast<Backplate*>(other_)) return false;
  if ((otherInterface_ == handEnd) &&!dynamic_cast<Hand*>(other_)) return false;
  if (myInterface_ == clockBase && otherInterface_ == gearBottom) return true;
  if (myInterface_ == handEnd && (otherInterface_ == gearEdge || otherInterface_ == gearTop )) return true;
  if (myInterface_ == gearTop && (otherInterface_ == handEnd && otherInterface_ == gearBottom)) return true;
  if (myInterface_ == gearBottom && (otherInterface_ == gearTop || otherInterface_ == clockBase)) return true;
  if (myInterface_ == gearEdge && (otherInterface_ == gearEdge || otherInterface_ == handEnd)) return true;
  //  if (myInterface_ != gearBottom && otherInterface_ == empty) return true; // removed; links to empty won't have a connection at all
  return false;
      
}

bool Component::isOK() {
  for (deque<Connection>::iterator it = connections_.begin() ; it != connections_.end() ; it++) {
    if (!it->isOK()) return false;
  }
  return true;
}

void Component::link(Component* other,interfaceType myInterface, interfaceType otherInterface) {
  Connection outConnection(other,myInterface,otherInterface);
  Connection inConnection(this,otherInterface,myInterface);
  connections_.push_back(outConnection);
  other->connections_.push_back(inConnection);
}

bool Component::hasLinkToBase(deque<Component*> *sofar) {
  if (sofar == NULL) {
    sofar = new deque<Component*>;
  }
  for (deque<Component*>::iterator it = sofar->begin() ; it != sofar->end() ; it++) {
    if (this == *it) return false; // Have gone in a loop, don't repeat ourselves.
  }
  sofar->push_back(this);
  for (deque<Connection>::iterator it = connections_.begin() ; it != connections_.end() ; it++) {
    if (it->otherInterface() == clockBase) return true;
    if (it->otherInterface() != empty && it->otherComponent()->hasLinkToBase(sofar)) return true;
  }
  return false;
}

bool Gear::hasLoop() {
  Gear *theGear = this;
  bool hasbottom = true;
  while (hasbottom) {
    hasbottom = false;
    deque<Connection> cList = theGear->connections_;
    for (deque<Connection>::iterator it = cList.begin() ; it != cList.end() ; it++) {
      if (it->myInterface() == gearBottom) {
	hasbottom = true;
	if (it->otherInterface() == gearTop) {
	  theGear = dynamic_cast<Gear*>(it->otherComponent());
	  if (!theGear) {
	    cout << "Thought we should be casting to a gear, but failed.  Probably an error!  hasLoop() returns true" << endl;
	    return true;
	  }
	  if (theGear == this) return true; // found a loop
	  break;
	}
	if (it->otherInterface() == clockBase) {
	  return false; // Found our way down to a clockBase -- no loop as long as everything is ok
	}
      }
    }
  }
  return false;
}



bool Gear::isOK() {
  unsigned int nGearBottom = 0;
  unsigned int nGearTop = 0;
  for (deque<Connection>::iterator it = connections_.begin() ; it != connections_.end() ; it++) {
    if (!it->isOK()) return false;
    if (it->myInterface() == gearTop) nGearTop++;
    if (it->myInterface() == gearBottom) nGearBottom++;
  }
  if (hasLoop()) return false;
  return (nGearBottom <= 1 && nGearTop <= 1);
}

Clock::Clock() {
}

bool Clock::isOK() {
  for (deque<Hand>::iterator it =hands_.begin() ; it != hands_.end() ; it++) {
    if (!it->isOK()) return false;
  }
  for (deque<Gear>::iterator it =gears_.begin() ; it != gears_.end() ; it++) {
    if (!it->isOK()) return false;
  }
  return backplate_.isOK();
}
