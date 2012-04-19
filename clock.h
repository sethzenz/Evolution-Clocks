#include <string>
#include <deque>
#include <math.h>
#include <stdlib.h>

using namespace std;

typedef int idType;

namespace EvolvingClocks {

  enum interfaceType { empty = 0, clockBase, handEnd,  gearEdge, gearTop, gearBottom, INTERFACE_MAX };
  enum clockDesign { plate = 0, basicPendulum, brokenPendulum, doublePendulum, ratchetPendulum, ratchetPendulumWithHand };

  class Component;

  class Connection {
  private:
    interfaceType myInterface_;
    interfaceType otherInterface_;
    Component* other_;
  public:
    Connection();
    Connection(Component*,interfaceType, interfaceType);
    bool isOK(bool verbose=false);
    interfaceType myInterface() {return myInterface_;}
    interfaceType otherInterface() {return otherInterface_;}
    Component* otherComponent() {return other_;}
  };

  class Component {
    friend class Clock;
  protected:
    idType id_;
    deque<Connection> connections_;
  public:
    Component(){id_ = rand();}
    Component(idType id){id_ = id;}
    virtual ~Component(){} //otherwise dynamic casts don't work
    bool isOK(bool verbose=false);
    void link(Component*,interfaceType,interfaceType);
    void delink(Component*);
    bool hasLinkToBase();
    bool hasLinkToBase(deque<idType>);
    bool hasLinkToBaseExcluding(idType);
    virtual unsigned int maxConnectionsOfType(interfaceType iType){return 0;}
    unsigned int nConnectionsOfType(interfaceType);
    unsigned int nTargetsOfType(interfaceType);
    unsigned int freeConnectionsOfType(interfaceType);
    deque<interfaceType> freeConnectionTypes();
    string description(){ return ""; }
    idType identifier(){return id_;}
    void setIdentifier(idType id){id_=id;}
  };

  class Hand : public Component {
  private:
    float length_;
  public:
    unsigned int maxConnectionsOfType(interfaceType iType);
    ~Hand(){};
    Hand(float l, idType id) {  length_ = l; id_ = id; }
    Hand();
    float period() { return 2*3.14159*sqrt(length_/9.8); } 
    string description();
  };

  class Gear : public Component {
  private:
    //    float radius_;
    int teeth_;
  public:
    ~Gear(){};
    bool isOK(bool verbose=false);
    bool hasLoop();
    unsigned int maxConnectionsOfType(interfaceType iType);
    //    Gear(float r, int t) { radius_ = r; teeth_ = t; }
    Gear(int t,idType id) { teeth_ = t; id_ = id; }
    Gear();
    int nTeeth() { return teeth_; }
    string description();
  };

  class Backplate : public Component {
  public:
    unsigned int maxConnectionsOfType(interfaceType iType);
    ~Backplate(){};
    Backplate(){};
    string description(){ return "backplate"; }
  };

  class Clock {
  private:
    deque<Hand> hands_;
    Backplate backplate_;
    deque<Gear> gears_;
    idType nextId_;
  public:
    Clock(){};
    Clock(clockDesign);
    bool isOK(bool verbose=false);
    deque<float> periods();
    void resetIdentifiers();
  };

}

