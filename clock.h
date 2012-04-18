#include <string>

using namespace std;

namespace EvolvingClocks {

  class Component {
  private:
    string name_;
  public:
    Component() {name_ = "";};
    void setName(string);
    string getName();
  };

}

