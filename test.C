#include <iostream>
#include "clock.h"

using namespace std;
using namespace EvolvingClocks;

int main() {
  cout << "Hello" << endl;
  Component x;
  Gear y;
  y.link(&y,gearBottom,gearTop);
  cout << "One gear linked to self: isOK=" << y.isOK() << " hasLoop=" << y.hasLoop() << " hasLinkToBase()=" << y.hasLinkToBase() << endl;
  Gear a,b;
  a.link(&b,gearBottom,gearTop);
  cout << "Two gears linked sanely: isOK=" << a.isOK() << " hasLoop=" << a.hasLoop() << " hasLinkToBase()=" << a.hasLinkToBase() << endl;
  Gear j,k,l;
  j.link(&k,gearBottom,gearTop);
  k.link(&l,gearBottom,gearTop);
  l.link(&j,gearBottom,gearTop);
  cout << "Three gears in a vertical loop: isOK=" << j.isOK() << " hasLoop=" << j.hasLoop() << " hasLinkToBase()=" << j.hasLinkToBase() << endl;
  Backplate bp;
  Gear g1,g2;
  g2.link(&g1,gearBottom,gearTop);
  g1.link(&bp,gearBottom,clockBase);
  cout << "Two gears on backplate: isOK=" << g2.isOK() << " hasLoop=" << g2.hasLoop() << " hasLinkToBase()=" << g2.hasLinkToBase() << endl;
  return 0;
}
