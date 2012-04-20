#include <string>
#include <deque>
#include <math.h>
#include <stdlib.h>

#ifndef _CLOCK_H_
#define _CLOCK_H_

#define TOLERANCE 0.000001
#define fracDiff(a,b) (fabs(a-b)/b)


using namespace std;

typedef int idType;


namespace EvolvingClocks {

  enum interfaceType { empty = 0, clockBase, handEnd,  gearEdge, gearTop, gearBottom, INTERFACE_MAX };
  enum clockDesign { plate = 0, basicPendulum, brokenPendulum, doublePendulum, ratchetPendulum, ratchetPendulumThreeGears, ratchetPendulumBroken, ratchetPendulumWithHand };
  enum periodType { pendulum, plainGear, gearWithHand };

  class PeriodInfo {
  private:
    float period_;
    periodType type_;
    idType id_; // Of component with this period
  public:
    PeriodInfo(float p, periodType t, idType id){ period_ = p; type_ = t; id_ = id; }
    float period(){ return period_; }
    periodType type(){return type_; }
    idType componentId() { return id_; }
  };

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
    string description();
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
    interfaceType randomFreeConnectionType();
    bool hasFreeConnections();
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
    Hand(float l) { length_ = l; id_ = rand(); }
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
    Gear(int t) { teeth_ = t; id_ = rand(); }
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
    Clock(){ resetIdentifiers(); };
    Clock(clockDesign);
    Clock(int);
    bool isOK(bool verbose=false);
    deque<PeriodInfo> periods(bool verbose=false);
    void resetIdentifiers();
    int nPieces(){return (1+gears_.size()+hands_.size());}
    deque<Component*> freeComponents();
    Component* randomFreeComponent();
    void display ();
    void AddRandom();
  };

  class Observer {
  public:
    Observer(){};
    float eval(Clock&){return 1;}
  };

  class Frequentist : public Observer {
  private:
    float desired_; // desired period
    float precision_; // should be positive and small. the closer it is to 0, the greater the reward for getting 1 period exactly right
  public:
    Frequentist(float d, float p=TOLERANCE){desired_ = d; precision_ = p;}
    float eval(Clock&);
  };

  class Traditionalist : public Observer {
  private:
    float precision_;
  public:
    Traditionalist(float p=TOLERANCE){ precision_ = p;}
    float eval(Clock&);
  };




}

#endif
