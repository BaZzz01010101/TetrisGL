#pragma once

#include "LayoutObject.h"

class Layout
{
private:
  static const float screenLeft;
  static const float screenTop;
  static const float screenWidth;
  static const float screenHeight;

  static float scoreBarTopGap;
  static float scoreBarLeftGap;
  static float scoreBarRightGap;
  static float scoreBarSeparatorGap;
  static float scoreBarHeight;
  static float scoreBarCaptionWidth;
  static float scoreBarMenuButtonWidth;

  static float glassWidth;
  static float glassHeight;
  static float glassTop;

  static float infoPanelsHorzGapFromGlass;
  static float infoPanelsWidth;
  static float holdNextCaptionsTop;
  static float holdNextPanelsTop;
  static float levelCaptionTop;
  static float levelPanelTop;
  static float goalCaptionTop;
  static float goalPanelTop;
  static float infoPanelsCaptionHeight;
  static float holdNextPanelsHeight;
  static float levelGoalPanelsHeight;

  static float menuTop;
  static float menuRowWidth;
  static float menuRowHeight;
  static float menuRowInterval;

  static void loadValue(rapidjson::Value & source, const char * name, float * result);

  Layout();
  ~Layout();

public:
  static ReadOnly<float, Layout> backgroundLeft;
  static ReadOnly<float, Layout> backgroundTop;
  static ReadOnly<float, Layout> backgroundWidth;
  static ReadOnly<float, Layout> backgroundHeight;
  static ReadOnly<float, Layout> gameBkTileWidth;
  static ReadOnly<float, Layout> gameBkTileHeight;
  static ReadOnly<float, Layout> holdNextFigureScale;
  static ReadOnly<float, Layout> menuRowGlowWidth;
  static ReadOnly<float, Layout> menuRowCornerSize;
  static ReadOnly<float, Layout> menuRowTextOffset;
  static ReadOnly<float, Layout> menuFontHeight;

  static LayoutObject gameLayout;
  static LayoutObject mainMenuLayout;
  static LayoutObject inGameMenuLayout;
  static LayoutObject quitConfirmationMenuLayout;
  static LayoutObject restartConfirmationMenuLayout;
  static LayoutObject exitToMainConfirmationMenuLayout;
  static LayoutObject saveSettingsMenuLayout;
  static void load(const char * name);
};
