#include "static_headers.h"

#include "Time.h"
#include "Crosy.h"

uint64_t Time::freq = Crosy::getPerformanceFrequency();
uint64_t Time::counter = Crosy::getPerformanceCounter();
uint64_t Time::counterDelta = 0;
double Time::timer = freq > 0.0 ? double(counter) / freq : 0.0;
float Time::timerDelta = 0.0f;

void Time::update()
{
  assert(freq > 0);
  uint64_t newCounter = Crosy::getPerformanceCounter();
  counterDelta = newCounter - counter;
  counter = newCounter;
  double newTime = freq > 0 ? double(counter) / double(freq) : timer;
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
