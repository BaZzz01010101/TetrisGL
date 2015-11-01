#pragma once

class FpsCounter
{
private:
  GLFWwindow * win;
  int cnt;
  uint64_t freq;
  uint64_t ticks;
public:
  FpsCounter();
  ~FpsCounter();

  void init(GLFWwindow * window);
  void pulse();
};

