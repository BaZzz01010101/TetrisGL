#pragma once

class Globals
{
private:
  Globals();
  ~Globals();

public:
  static std::string glErrorMessage;
};

static const int FAST_RAND_MAX = 0x7FFF;

inline int fastrand()
{
  static int g_seed = rand();
  g_seed = (214013 * g_seed + 2531011);
  return (g_seed >> 16) & FAST_RAND_MAX;
}

extern bool checkGlErrors();
