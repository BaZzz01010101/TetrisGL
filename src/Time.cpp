#include "static_headers.h"

#include "Time.h"
#include "Crosy.h"

double Time::freq = (double)Crosy::getPerformanceFrequency();
ReadOnly<uint64_t, Time> Time::counter = Crosy::getPerformanceCounter();
ReadOnly<uint64_t, Time> Time::counterDelta = 0;
ReadOnly<double, Time> Time::timer = freq > 0.0 ? double(counter) / freq : 0.0;
ReadOnly<float, Time> Time::timerDelta = 0.0f;

void Time::update()
{
  assert(freq > 0);
  uint64_t newCounter = Crosy::getPerformanceCounter();
  counterDelta = newCounter - counter;
  counter = newCounter;
  double newTime = freq > VERY_SMALL_NUMBER ? double(counter) / freq : timer;
  timerDelta = float(newTime - timer);
  timer = newTime;
}

//namespace Time
//{
//  double freqF = (double)Crosy::getPerformanceFrequency();
//  uint64_t counterF = Crosy::getPerformanceCounter();
//  uint64_t counterDeltaF = 0;
//  double timerF = freqF > 0.0 ? double(counterF) / freqF : 0.0;
//  float timerDeltaF = 0.0f;
//
//  const uint64_t & counter = counterF;
//  const uint64_t & counterDelta = counterDeltaF;
//  const double & timer = timerF;
//  const float & timerDelta = timerDeltaF;
//
//  void update()
//  {
//    assert(freqF > 0);
//    uint64_t newCounter = Crosy::getPerformanceCounter();
//    counterDeltaF = newCounter - counterF;
//    counterF = newCounter;
//    double newTime = freqF > 0.0 ? double(counterF) / freqF : timerF;
//    timerDeltaF = float(newTime - timerF);
//    timerF = newTime;
//  }
//}
