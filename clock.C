#include "clock.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <sstream>

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

bool Connection::isOK(bool verbose) {
  if (!other_) return false;
  if ((otherInterface_ == gearBottom || otherInterface_ == gearTop || otherInterface_ == gearEdge) && !dynamic_cast<Gear*>(other_))  return false;
  if ((otherInterface_ == clockBase) && !dynamic_cast<Backplate*>(other_)) return false;
  if ((otherInterface_ == handEnd) &&!dynamic_cast<Hand*>(other_)) return false;
  if (myInterface_ == clockBase && (otherInterface_ == gearBottom || otherInterface_ == handEnd)) return true;
  if (myInterface_ == handEnd && (otherInterface_ == gearEdge || otherInterface_ == gearTop || otherInterface_ == clockBase )) return true;
  if (myInterface_ == gearTop && (otherInterface_ == handEnd || otherInterface_ == gearBottom)) return true;
  if (myInterface_ == gearBottom && (otherInterface_ == gearTop || otherInterface_ == clockBase)) return true;
  if (myInterface_ == gearEdge && (otherInterface_ == gearEdge || otherInterface_ == handEnd)) return true;
  //  if (myInterface_ != gearBottom && otherInterface_ == empty) return true; // removed; links to empty won't have a connection at all
  return false;
      
}

bool Component::isOK(bool verbose) {
  for (deque<Connection>::iterator it = connections_.begin() ; it != connections_.end() ; it++) {
    if (verbose) cout << "Connection " << it->myInterface() << "-" << it->otherInterface() << " isOK=" << it->isOK() << endl;
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

void Component::delink(Component* other) {
  deque<Connection>::iterator it;
  for (connections_.begin() ; it != connections_.end() ; it++) {
    if (other == it->otherComponent()) break;
  }
  if (it == connections_.end()) {
    cout << "Can't find forward link, so no delinking possible" << endl;
    return;
  }
  deque<Connection>::iterator oit;
  for (it->otherComponent()->connections_.begin() ; oit != it->otherComponent()->connections_.end() ; oit++) {
    if (oit->otherComponent() == this) break;
  }
  if (oit == it->otherComponent()->connections_.end()) {
    cout << "Can't find backward link, so no delinking possible" << endl;
    return;
  }
  it->otherComponent()->connections_.erase(oit);
  connections_.erase(it);
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

bool Gear::isOK(bool verbose) {
  unsigned int nGearBottom = 0;
  unsigned int nGearTop = 0;
  for (deque<Connection>::iterator it = connections_.begin() ; it != connections_.end() ; it++) {
    if (verbose) cout << "Connection " << it->myInterface() << "-" << it->otherInterface() << " isOK=" << it->isOK() << endl;
    if (!it->isOK()) return false;
    if (it->myInterface() == gearTop) nGearTop++;
    if (it->myInterface() == gearBottom) nGearBottom++;
  }
  if (nGearBottom > 1 || nGearTop > 1) return false;
  if (hasLoop()) return false;
  return true;
}

Clock::Clock(clockDesign design) {
  if (design==basicPendulum) {
    hands_.push_back(Hand());
    gears_.push_back(Gear());
    hands_[0].link(&backplate_,handEnd,clockBase);
    hands_[0].link(&(gears_[0]),handEnd,gearTop);
  }
}

bool Clock::isOK(bool verbose) {
  for (deque<Hand>::iterator it =hands_.begin() ; it != hands_.end() ; it++) {
    if (verbose) cout << it->description() << " isOK=" << it->isOK() << endl;
    if (!it->isOK()) return false;
  }
  for (deque<Gear>::iterator it =gears_.begin() ; it != gears_.end() ; it++) {
    if (verbose) cout << it->description() << " isOK="<< it->isOK() << endl;
    if (!it->isOK()) return false;
  }
  if (verbose) cout << backplate_.description() << " isOK="<< backplate_.isOK() << endl;
  return backplate_.isOK();
}

unsigned int Component::nTargetsOfType(interfaceType iType) {
  unsigned int total = 0;
  for (deque<Connection>::iterator it = connections_.begin() ; it != connections_.end() ; it++) {
    if (it->otherInterface() == iType) total++;
  }
  //   cout << " returning " << total << " for iType=" << iType << endl;
  return total;
}

unsigned int Component::nConnectionsOfType(interfaceType iType) {
  unsigned int total = 0;
  for (deque<Connection>::iterator it = connections_.begin() ; it != connections_.end() ; it++) {
    if (it->myInterface() == iType) total++;
  }
  //  cout << " returning " << total << " for iType=" << iType << endl;
  return total;
}

unsigned int Component::freeConnectionsOfType(interfaceType iType) {
  if (maxConnectionsOfType(iType) > nConnectionsOfType(iType)) {
    return (maxConnectionsOfType(iType)  - nConnectionsOfType(iType));
  } else {
    return 0;
  }
}
  

deque<interfaceType> Component::freeConnectionTypes() {
  deque<interfaceType> answer;
  for (int i = empty ; i < INTERFACE_MAX ; i++) {
    if (freeConnectionsOfType(interfaceType(i)) > 0) answer.push_back(interfaceType(i));
  }
  return answer;
}

unsigned int Hand::maxConnectionsOfType(interfaceType iType) {
  if (iType == handEnd) return 2;
  return 0;
}

unsigned int Gear::maxConnectionsOfType(interfaceType iType) {
  if (iType == gearTop || iType == gearBottom) return 1;
  if (iType == gearEdge) return teeth_;
  return 0;
}

unsigned int Backplate::maxConnectionsOfType(interfaceType iType) {
  if (iType == clockBase) return 1000;
  return 0;
}

Gear::Gear() {
  //  radius_ = 0.001*((rand()%1000)+100);
  teeth_ = (rand()%30)+6;
}

Hand::Hand() {
  length_ = 0.01*((rand()%100)+10);
}

string Gear::description() {
  stringstream s;
  s << "gear t=" << teeth_;
  return s.str();
}

string Hand::description() {
  stringstream s;
  s << "hand l=" << length_;
  return s.str();
}

deque<float> Clock::periods() {
  deque<float> result;
  for (deque<Hand>::iterator it = hands_.begin() ; it != hands_.end() ; it++) {
    cout << it->description() << " " << it->nTargetsOfType(clockBase) << " " << it->nTargetsOfType(gearEdge) << " " << it->nTargetsOfType(gearTop)  << endl;
    if ((it->nTargetsOfType(clockBase)==1 || it->nTargetsOfType(gearEdge)) && it->nTargetsOfType(gearTop) == 1) {
      result.push_back(it->period());
    }
  }
  return result;
}
