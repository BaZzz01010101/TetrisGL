#pragma once

class Time
{
private:

  Time();
  ~Time();

public:
  static uint64_t freq;
  static uint64_t counter;
  static uint64_t counterDelta;
  static double timer;
  static float timerDelta;
  static void update();
};

//namespace Time
//{
//  extern const uint64_t & counter;
//  extern const uint64_t & counterDelta;
//  extern const double & timer;
//  extern const float & timerDelta;
//  extern void update();
//};
//
