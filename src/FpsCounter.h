#pragma once

class FpsCounter
{
private:
  int cnt;
  uint64_t freq;
  uint64_t ticks;
  enum { bufSize = 256 };
  char buf[bufSize];
public:
  FpsCounter();
  ~FpsCounter();

  void init();
  char * count(float interval);
};

