#include "static_headers.h"

#include "Time.h"
#include "Crosy.h"

uint64_t PerfTime::freq = Crosy::getPerformanceFrequency();
uint64_t PerfTime::counter = Crosy::getPerformanceCounter();
uint64_t PerfTime::counterDelta = 0;
double PerfTime::timer = freq > 0 ? double(counter) / freq : 0.0;
float PerfTime::timerDelta = 0.0f;

void PerfTime::update()
{
  assert(freq > 0);
  uint64_t newCounter = Crosy::getPerformanceCounter();
  counterDelta = newCounter - counter;
  counter = newCounter;
  double newTime = freq > 0 ? double(counter) / double(freq) : timer;
  timerDelta = float(newTime - timer);
  timer = newTime;
}


float PerfTime::getCurrentTimerDelta()
{
  assert(freq > 0);
  uint64_t newCounter = Crosy::getPerformanceCounter();
  double newTime = freq > 0 ? double(newCounter) / double(freq) : timer;
  return float(newTime - timer);
}
