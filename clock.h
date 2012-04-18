#include <string>
#include <deque>

using namespace std;

namespace EvolvingClocks {

  enum interfaceType { empty, clockBase, handEnd,  gearEdge, gearTop, gearBottom };

  class Component;

  class Connection {
  private:
    interfaceType myInterface_;
    interfaceType otherInterface_;
    Component* other_;
  public:
    Connection();
    Connection(Component*,interfaceType, interfaceType);
    bool isOK();
    interfaceType myInterface() {return myInterface_;}
    interfaceType otherInterface() {return otherInterface_;}
    Component* otherComponent() {return other_;}
  };

  class Component {
  protected:
    deque<Connection> connections_;
  public:
    Component(){};
    virtual ~Component(){}; //otherwise dynamic casts don't work
    bool isOK();
    void link(Component*,interfaceType,interfaceType);
    bool hasLinkToBase(deque<Component*>* sofar=NULL);
  };

  class Hand : public Component {
  private:
    float length_;
  };

  class Gear : public Component {
  private:
    float radius_;
    int teeth_;
  public:
    bool isOK();
    bool hasLoop();
  };

  class Backplate : public Component {
    // ?
  };

  class Clock {
  private:
    deque<Component> components_;
  public:
    bool isOK();
  };

}

