#include "static_headers.h"

#include "FpsCounter.h"
#include "Crosy.h"
#include "Time.h"

FpsCounter::FpsCounter()
{
}


FpsCounter::~FpsCounter()
{
}


void FpsCounter::init()
{
  cnt = 0;
  fps = 0.0f;
  freq = Crosy::getPerformanceFrequency();
  lastIntervalCounter = Crosy::getPerformanceCounter();
  maxFrameTime = 0.0f;
}


char * FpsCounter::count(float interval)
{
  cnt++;

  if (freq)
  {
    bool redraw = false;
    uint64_t counter = Crosy::getPerformanceCounter();
    float intervalTime = float(double(counter - lastIntervalCounter) / double(freq));
    float frameTime = PerfTime::getCurrentTimerDelta();// float(double(counter - lastFrameCounter) / double(freq));
    lastFrameCounter = counter;

    if (intervalTime > interval)
    {
      fps = cnt / intervalTime;
      redraw = true;
      lastIntervalCounter = Crosy::getPerformanceCounter();
      cnt = 0;
    }

    if (frameTime  > maxFrameTime)
    {
      maxFrameTime = frameTime;
      frameTimeCounter = counter;
      redraw = true;
    }
    else if (counter - frameTimeCounter > uint64_t(interval * freq))
    {
      maxFrameTime = 0.0f;
      frameTimeCounter = counter;
    }

    if (redraw)
      Crosy::snprintf(buf, bufSize, "Fps: %.3f MaxFrameTime: %.3fms", fps, maxFrameTime * 1000);
  }

  return buf;
}
