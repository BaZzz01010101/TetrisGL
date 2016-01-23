#include "static_headers.h"

#include "Time.h"
#include "Crosy.h"

uint64_t Time::freq = Crosy::getPerformanceFrequency();
uint64_t Time::counter = Crosy::getPerformanceCounter();
uint64_t Time::counterDelta = 0;
double Time::timer = freq > 0 ? double(counter) / freq : 0.0;
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


float Time::getCurrentTimerDelta()
{
  assert(freq > 0);
  uint64_t newCounter = Crosy::getPerformanceCounter();
  double newTime = freq > 0 ? double(newCounter) / double(freq) : timer;
  return float(newTime - timer);
}
