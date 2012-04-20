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
    // Scores based on how close any period in the clock is to the desired period.
    // Averages scores of all hands
  private:
    float desired_; // desired period
    float precision_; // should be positive and small
                      // the closer it is to 0, the greater the reward for getting 1 period exactly right 
                      // (even with many extras around, which are penalized in principle)
  public:
    Frequentist(float d, float p=TOLERANCE){desired_ = d; precision_ = p;}
    float eval(Clock&);
  };

  class Traditionalist : public Observer {
    // Wants exactly one pendulum with a 1s period, and three gears with hands with periods of 1 minute, 1 hour, and 12 hours
    // Penalties for the wrong number of either pendulums or gears with hands.  Periods of handless gears ignored
  private:
    float precision_; // should be positive and small. 
                      // the closer it is to 0, the greater the reward for getting 1 period exactly right 
                      // (compared with getting others in ballpark)
  public:
    Traditionalist(float p=TOLERANCE){ precision_ = p;}
    float eval(Clock&);
  };

  class Cat : public Observer {
    // Doesn't matter what things are moving; the more things moving, the better
  public:
    Cat(){}
    float eval(Clock& c){return float(c.periods().size()); }
  };


}

#endif
