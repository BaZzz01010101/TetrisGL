#include "static_headers.h"

#include "FpsCounter.h"
#include "Crosy.h"


FpsCounter::FpsCounter()
{
}


FpsCounter::~FpsCounter()
{
}

void FpsCounter::init()
{
  cnt = 0;
  freq = Crosy::getPerformanceFrequency();
  ticks = Crosy::getPerformanceCounter();
}

char * FpsCounter::count(float interval)
{
  cnt++;

  if (freq)
  {
    float timePass = float(double(Crosy::getPerformanceCounter() - ticks) / double(freq));

    if (timePass > interval)
    {
      float fps = cnt / timePass;
      Crosy::snprintf(buf, bufSize, "Fps: %.3f", fps);
      ticks = Crosy::getPerformanceCounter();
      cnt = 0;
    }
  }

  return buf;
}
