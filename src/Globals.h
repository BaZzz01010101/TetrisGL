#pragma once

class Globals
{
private:
  Globals();
  ~Globals();

public:
  static std::string glErrorMessage;

  static int nextFiguresCount;
  static float rowsDeletionEffectTime;
  static float menuShowingTime;
  static float menuHidingTime;
  static float settingsShowingTime;
  static float settingsHidingTime;
  static float leaderboardShowingTime;
  static float leaderboardHidingTime;
  static const int beginMs = 51;
  static const int endMs = 112992;
  static int smallFontSize;
  static int midFontSize;
  static int bigFontSize;
};

extern bool checkGlErrors();

static int g_seed = rand();
static const int FAST_RAND_MAX = 0x7FFF;
inline int fastrand()
{
  g_seed = (214013 * g_seed + 2531011);
  return (g_seed >> 16) & FAST_RAND_MAX;
}

