#include "clock.h"
#include <iostream>
//#include <stdlib.h>
//#include <time.h>
#include <sstream>
#include <map>

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

bool interfaceAllowed(interfaceType a,interfaceType b) {
  if (a == clockBase && (b == gearBottom || b == handEnd)) return true;
  if (a == handEnd && (b == gearEdge || b == gearTop || b == clockBase )) return true;
  if (a == gearTop && (b == handEnd || b == gearBottom)) return true;
  if (a == gearBottom && (b == gearTop || b == clockBase)) return true;
  if (a == gearEdge && (b == gearEdge || b == handEnd)) return true;
  return false;
}

bool Connection::isOK(bool verbose) {
  if (!other_) return false;
  if ((otherInterface_ == gearBottom || otherInterface_ == gearTop || otherInterface_ == gearEdge) && !dynamic_cast<Gear*>(other_))  return false;
  if ((otherInterface_ == clockBase) && !dynamic_cast<Backplate*>(other_)) return false;
  if ((otherInterface_ == handEnd) &&!dynamic_cast<Hand*>(other_)) return false;
  return interfaceAllowed(myInterface_,otherInterface_);
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

bool Component::hasLinkToBaseExcluding(idType notVia) {
  deque<idType> sofar;
  sofar.push_back(notVia);
  return hasLinkToBase(sofar);
}

bool Component::hasLinkToBase() {
  deque<idType> sofar;
  return hasLinkToBase(sofar);
}

bool Component::hasLinkToBase(deque<idType> sofar) {
  for (deque<idType>::iterator it = sofar.begin() ; it != sofar.end() ; it++) {
    if (identifier() == *it) return false; // Have gone in a loop, don't repeat ourselves.
  }
  sofar.push_back(identifier());
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

deque<Component*> Clock::freeComponents() {
  deque<Component*> answer;
  answer.push_back(&backplate_);
  for (deque<Hand>::iterator it = hands_.begin() ; it != hands_.end() ; it++) {
    if (it->hasFreeConnections()) {
      answer.push_back(&*it);
    }
  }
  for (deque<Gear>::iterator it = gears_.begin() ; it != gears_.end() ; it++) {
    if (it->hasFreeConnections() ) {
      answer.push_back(&*it);
    }
  }
  return answer;
}

Component* Clock::randomFreeComponent() {
  deque<Component*> theList = freeComponents();
  return theList[(rand()%theList.size())];
}

Clock::Clock(int nRandom) {
  for (int i = 0 ; i < nRandom ; i++) {
    bool isHand = (rand() < (0.5*RAND_MAX));
    Component *newThing;
    if (isHand) {
      hands_.push_back(Hand());
      newThing = &(hands_[hands_.size()-1]);
    } else {
      gears_.push_back(Gear());
      newThing = &(gears_[gears_.size()-1]);
    }
    Component *addTo = randomFreeComponent();
    interfaceType fromInterface = addTo->randomFreeConnectionType();
    interfaceType toInterface = newThing->randomFreeConnectionType();
    if (!interfaceAllowed(fromInterface,toInterface)) {
      cout << " Vetoing from=" << fromInterface << " to=" << toInterface << endl;                                                             
      i--;
      break;
    }
    addTo->link(newThing,fromInterface,toInterface);
    cout << " Added item " << i << " and isOK=" << isOK() << " from=" << fromInterface << " to=" << toInterface << endl;
  }
  resetIdentifiers();
}

Clock::Clock(clockDesign design) {
  if (design==basicPendulum||design==brokenPendulum||design == doublePendulum) {
    hands_.push_back(Hand());
    gears_.push_back(Gear());
    hands_[0].link(&backplate_,handEnd,clockBase);
    hands_[0].link(&(gears_[0]),handEnd,gearTop);
  }
  if (design==brokenPendulum) gears_[0].link(&backplate_,gearBottom,clockBase);
  if (design == doublePendulum) {
    hands_.push_back(Hand());
    gears_.push_back(Gear());
    hands_[1].link(&(gears_[0]),handEnd,gearEdge);
    hands_[1].link(&(gears_[1]),handEnd,gearTop);
  }
  if (design == ratchetPendulum || design == ratchetPendulumThreeGears || design == ratchetPendulumBroken || design == ratchetPendulumWithHand) {
    gears_.push_back(Gear());
    hands_.push_back(Hand());
    gears_.push_back(Gear());
    gears_[0].link(&backplate_,gearBottom,clockBase);
    hands_[0].link(&gears_[0],handEnd,gearEdge);
    hands_[0].link(&gears_[1],handEnd,gearTop);
  }
  if (design == ratchetPendulumThreeGears || design == ratchetPendulumBroken) {
    gears_.push_back(Gear());
    gears_.push_back(Gear());
    gears_[2].link(&backplate_,gearBottom,clockBase);
    gears_[2].link(&gears_[0],gearEdge,gearEdge);
    gears_[3].link(&backplate_,gearBottom,clockBase);
    gears_[3].link(&gears_[2],gearEdge,gearEdge);
  }
  if (design == ratchetPendulumBroken) {
    hands_.push_back(Hand());
    gears_.push_back(Gear());
    hands_[1].link(&gears_[3],handEnd,gearEdge);
    hands_[1].link(&gears_[4],handEnd,gearTop);
  }
  if (design == ratchetPendulumWithHand) {
    hands_.push_back(Hand());
    hands_[1].link(&(gears_[0]),handEnd,gearTop);
  }
  resetIdentifiers();
}

void Clock::resetIdentifiers() {
  backplate_.setIdentifier(0);
  nextId_ = 1;
  for (deque<Hand>::iterator it =hands_.begin() ; it != hands_.end() ; it++) {
    it->setIdentifier(nextId_++);
  }
  for (deque<Gear>::iterator it =gears_.begin() ; it != gears_.end() ; it++) {
    it->setIdentifier(nextId_++);
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

bool Component::hasFreeConnections(){
  for (int i = empty ; i < INTERFACE_MAX ; i++) {
    if (freeConnectionsOfType(interfaceType(i)) > 0) return true;
  }
  return false;
}

deque<interfaceType> Component::freeConnectionTypes() {
  deque<interfaceType> answer;
  for (int i = empty ; i < INTERFACE_MAX ; i++) {
    if (freeConnectionsOfType(interfaceType(i)) > 0) answer.push_back(interfaceType(i));
  }
  return answer;
}

interfaceType Component::randomFreeConnectionType() {
  deque<interfaceType> theList = freeConnectionTypes();
  return theList[rand()%theList.size()];
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
  s << "gear id=" << id_ << " t=" << teeth_;
  return s.str();
}

string Hand::description() {
  stringstream s;
  s << "hand id=" << id_ << " l=" << length_;
  return s.str();
}

deque<float> Clock::periods() {
  deque<float> result;
  for (deque<Hand>::iterator it = hands_.begin() ; it != hands_.end() ; it++) {
    cout << it->description() << " " << it->nTargetsOfType(clockBase) << " " << it->nTargetsOfType(gearEdge) << " " << it->nTargetsOfType(gearTop) << " " << it->period() << endl;
    if ((it->nTargetsOfType(clockBase)==1 || it->nTargetsOfType(gearEdge)) && it->nTargetsOfType(gearTop) == 1) {
      // The gear connected via its gearTop is a candidate for a swinging weight.  
      // But if we can get back to the base without going through the current hand, it's not swinging freely.
      deque<Connection>::iterator cit;
      for (cit = it->connections_.begin() ; cit != it->connections_.end() ; cit++ ) {
	if (cit->otherInterface() == gearTop) break;
      }
      if (cit != it->connections_.end() && !cit->otherComponent()->hasLinkToBaseExcluding(it->identifier())) result.push_back(it->period());
    }
  }
  map<idType,float> gearNumbers;
  for (deque<Gear>::iterator it = gears_.begin() ; it != gears_.end(); it++) {
    gearNumbers[it->identifier()] = -99.;
  }
  bool changed = true;
  while(changed) {
    changed = false;
    for (deque<Gear>::iterator it = gears_.begin() ; it != gears_.end(); it++) {
      cout << it->identifier() << "," << it->nTeeth() << "," << gearNumbers[it->identifier()] << "  ";
    }
    cout<< endl;
    for (deque<Gear>::iterator it = gears_.begin() ; it != gears_.end(); it++) {
      deque<Connection>::iterator cit;
      for (cit = it->connections_.begin() ; cit != it->connections_.end() ; cit++ ) {
	if (cit->myInterface() == gearEdge && cit->otherInterface() == handEnd) {
	  Hand *theHand = dynamic_cast<Hand*>(cit->otherComponent());
	  if (!theHand) cout << "This dynamic cast should not fail" << endl;
	  float newVal = theHand->period()*it->nTeeth();
	  if (gearNumbers[it->identifier()] < 0.) {
	    gearNumbers[it->identifier()] = newVal;
	    changed = true;
	  }
	  if (gearNumbers[it->identifier()] > 0. && newVal != gearNumbers[it->identifier()]) {
	    gearNumbers[it->identifier()] = 0.;
	    changed = true;
	  }
	}
	if ( (cit->myInterface() == gearEdge && cit->otherInterface() == gearEdge) 
	     || (cit->myInterface() == gearTop && cit->otherInterface() == gearBottom)
             || (cit->myInterface() == gearBottom && cit->otherInterface() == gearTop) ) {
	  Gear *otherGear = dynamic_cast<Gear*>(cit->otherComponent());
	  if (!otherGear) cout << "This dynamic cast should not fail" << endl;
	  float newVal = gearNumbers[otherGear->identifier()];
	  if (cit->myInterface() == gearEdge) newVal *= (float(it->nTeeth())/float(otherGear->nTeeth()));
	  //	  cout << "Possibly changing " << gearNumbers[it->identifier()]  << " to " << newVal;
	  if (gearNumbers[it->identifier()] < 0. && newVal > 0.) {
            gearNumbers[it->identifier()] = newVal;
            changed = true;
	    //	    cout << " ... done.";
	  }
	  if (gearNumbers[it->identifier()] > 0. && newVal > 0. && fabs((newVal-gearNumbers[it->identifier()])/newVal) > TOLERANCE) {
	    gearNumbers[it->identifier()] = 0.;
	    changed = true;
	    //	    cout << " ... conflict, set to 0.";
	  }
	  if (newVal == 0. && gearNumbers[it->identifier()] != 0.) {
            gearNumbers[it->identifier()] = 0.;
	    changed = true;
	  }
	  //	  cout << endl;
	}
      }
    }
  }
  for (deque<Gear>::iterator it = gears_.begin() ; it != gears_.end(); it++) {
    cout << it->identifier() << "," << it->nTeeth() << "," << gearNumbers[it->identifier()] << "  ";
    if (gearNumbers[it->identifier()] > 0.) {
      result.push_back(gearNumbers[it->identifier()]);
    }
  }
  cout << endl;
  return result;
}
