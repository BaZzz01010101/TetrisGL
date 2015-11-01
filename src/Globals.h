#pragma once

class Globals
{
private:
  Globals();
  ~Globals();

public:
  enum Color { clNone = -1, clRed, clOrange, clYellow, clGreen, clCyan, clBlue, clPurple };
  static glm::vec3 Globals::ColorValues[7];
  static std::string glErrorMessage;
  static GLuint mainArrayTextureId;
  static int mainArrayTextureSize;
  static float mainArrayTexturePixelSize;

  static int emptyTexIndex;
  static int backgroundTexIndex;
  static int blockTemplateTexIndex;
  static int boldBlockTemplateTexIndex;
  static int shadowTexIndex;
  static int holdFigureBkTexIndex;
  static int nextFigureBkTexIndex;
  static int dropTrailTexIndex;
  static int dropSparkleTexIndex;
  static int rowFlashTexIndex;
  static int rowShineRayTexIndex;
  static int rowShineLightTexIndex;
  static int sidePanelGlowingBorderTexIndex;
  static int sidePanelInnerGlowTexIndex;
  static int levelGoalBkTexIndex;
  static int fontFirstTexIndex;
  static const int nextFiguresCount = 3;
  static glm::vec2 gameBkPos;
  static glm::vec2 gameBkSize;
  static glm::vec2 glassPos;
  static glm::vec2 glassSize;
  static float dafaultCaptionHeight;
  static float holdNextBkHorzGap;
  static float holdNextBkSize;
  static float scoreBarGaps;
  static float scoreBarHeight;
  static float scoreBarCaptionWidth;
  static float scoreBarValueWidth;
  static float scoreBarMenuWidth;
  static int smallFontSize;
  static int midFontSize;
  static int bigFontSize;
};

extern bool checkGlErrors();
extern double getTimer();

static int g_seed = rand();
static const int FAST_RAND_MAX = 0x7FFF;
inline int fastrand()
{
  g_seed = (214013 * g_seed + 2531011);
  return (g_seed >> 16) & FAST_RAND_MAX;
}

