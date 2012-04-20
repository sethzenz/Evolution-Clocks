#include "clock.h"
#include "observer.h"

using namespace EvolvingClocks;

float Frequentist::eval(Clock& c) {
  float score = 0.;
  deque<PeriodInfo> p = c.periods();
  for (deque<PeriodInfo>::iterator it = p.begin() ; it != p.end() ; it++) {
    score += 1/(precision_+fracDiff(it->period(),desired_));
  }
  score /= p.size();
  return score;
}

float Traditionalist::eval(Clock& c) {
  deque<PeriodInfo> p = c.periods();
  float pendScore = 0.;
  float hourHandScore = 0.;
  float secondHandScore = 0.;
  float minuteHandScore = 0.;
  int nPend = 0;
  int nProperHands = 0;
  for (deque<PeriodInfo>::iterator it = p.begin() ; it != p.end() ; it++) {
    if (it->type() == pendulum) {
      nPend++;
      float newPendScore = 1/(precision_+fracDiff(it->period(),1.));
      if (newPendScore > pendScore) pendScore = newPendScore;
    }
    if (it->type() == gearWithHand) {
      nProperHands++;
      float newHourHandScore = 1/(precision_+fracDiff(it->period(),12*3600.));
      if (newHourHandScore > hourHandScore) hourHandScore = newHourHandScore;
      float newMinuteHandScore = 1/(precision_+fracDiff(it->period(),3600.));
      if (newMinuteHandScore > minuteHandScore) minuteHandScore = newMinuteHandScore;
      float newSecondHandScore = 1/(precision_+fracDiff(it->period(),60.));
      if (newSecondHandScore > secondHandScore) secondHandScore = newSecondHandScore;
    }
  }
  float score = pendScore + minuteHandScore + secondHandScore + hourHandScore;
  score /= (1+abs(nPend-1));
  score /= (1+abs(nProperHands-3));
  return score;
}
