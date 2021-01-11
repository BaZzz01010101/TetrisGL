#pragma once

class PerfTime
{
private:
  PerfTime();
  ~PerfTime();

public:
  static uint64_t freq;
  static uint64_t counter;
  static uint64_t counterDelta;
  static double timer;
  static float timerDelta;
  static void update();
  static float getCurrentTimerDelta();
};
