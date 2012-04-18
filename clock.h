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
  };

  class Component {
  private:
    deque<Connection> connections_;
  public:
    Component(){};
    virtual ~Component(){}; //otherwise dynamic casts don't work
    bool isOK();
  };

  class Hand : public Component {
  private:
    float length_;
    // 3 connections
  };

  class Gear : public Component {
  private:
    float radius_;
    int teeth_;
    // center link
    // multiple surface links
  public:
    bool isOK();
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

