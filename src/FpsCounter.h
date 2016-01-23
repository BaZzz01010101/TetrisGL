#pragma once

class FpsCounter
{
private:
  int cnt;
  float fps;
  uint64_t freq;
  uint64_t lastIntervalCounter;
  uint64_t lastFrameCounter;
  float maxFrameTime;
  uint64_t frameTimeCounter;
  enum { bufSize = 256 };
  char buf[bufSize];

public:
  FpsCounter();
  ~FpsCounter();

  void init();
  char * count(float interval);
};

