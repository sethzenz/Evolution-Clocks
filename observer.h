#include <string>
#include <deque>
#include <math.h>
#include <stdlib.h>
#include "clock.h"

#ifndef _OBSERVER_H_
#define _OBSERVER_H_

using namespace std;

namespace EvolvingClocks {

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
