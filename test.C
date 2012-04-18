#include <iostream>
#include "clock.h"

using namespace std;
using namespace EvolvingClocks;

int main() {
  cout << "Hello" << endl;
  Component x;
  cout << x.getName() << endl;
  x.setName("Bob");
  cout << x.getName() <<endl;
  return 0;
}
