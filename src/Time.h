#pragma once

class Time
{
private:
  static double freq;

  Time();
  ~Time();

public:
  static ReadOnly<uint64_t, Time> counter;
  static ReadOnly<uint64_t, Time> counterDelta;
  static ReadOnly<double, Time> timer;
  static ReadOnly<float, Time> timerDelta;
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
