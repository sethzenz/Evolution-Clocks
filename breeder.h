#include <string>
#include <deque>
#include <math.h>
#include <stdlib.h>
#include "clock.h"

#ifndef _BREEDER_H_
#define _BREEDER_H_

using namespace std;

namespace EvolvingClocks {

   class Breeder{
      private:
         
      public:
         Breeder();
         ~Breeder();
         Clock BreedPair(Clock& dad, Clock& mom);
   };



}

#endif
