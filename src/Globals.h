#pragma once

class Globals
{
private:
  Globals();
  ~Globals();

public:
//  enum Color { clNone = -1, clRed, clOrange, clYellow, clGreen, clCyan, clBlue, clPurple };
//  static glm::vec3 Globals::ColorValues[7];
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
  static int sidePanelGlowTexIndex;
  static int lineTexIndex;
  static int levelGoalBkTexIndex;
  static int levelBackShevronTexIndex;
  static int fontFirstTexIndex;

  //static glm::vec2 bkTileSize;
  //static glm::vec2 sidePanelBkTileSize;
  static int nextFiguresCount;
  static float rowsDeletionEffectTime;
  //static glm::vec2 gameBkPos;
  //static glm::vec2 gameBkSize;
  //static glm::vec2 glassPos;
  //static glm::vec2 glassSize;
  //static float defaultCaptionHeight;
  //static float holdNextBkHorzGap;
  //static float holdNextBkSize;
  //static float scoreBarGaps;
  //static float scoreBarHeight;
  //static float scoreBarCaptionWidth;
  //static float scoreBarValueWidth;
  //static float scoreBarMenuWidth;
  //static glm::vec3 levelPanelColor;
  //static glm::vec3 goalPanelColor;
  //static float menuTop;
  //static float menuRowWidth;
  //static float menuRowHeight;
  //static float menuRowInterval;
  //static float menuRowGlowWidth;
  //static float menuRowCornerSize;
  static float menuShowingTime;
  static float menuHidingTime;
  //static glm::vec3 menuNormalPanelColor;
  //static glm::vec3 menuSelectedPanelColor;
  //static glm::vec3 menuNormalTextColor;
  //static glm::vec3 menuSelectedTextColor;
  static float settingsTop;
  static float settingsWidth;
  static float settingsHeight;
  static float settingsGlowWidth;
  static float settingsCornerSize;
  static float settingsShowingTime;
  static float settingsHidingTime;
  static glm::vec3 settingsTopBkColor;
  static glm::vec3 settingsBottomBkColor;
  static glm::vec3 settingsPanelTopBkColor;
  static glm::vec3 settingsPanelBottomBkColor;
  static glm::vec3 settingsPanelActiveRowBkColor;
  static glm::vec3 settingsPanelInactiveRowBkColor;
  static glm::vec3 settingsPanelActiveRowFgColor;
  static glm::vec3 settingsPanelInactiveRowFgColor;
  static glm::vec3 settingsCaptionColor;
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

