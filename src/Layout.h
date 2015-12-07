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
  static float backgroundLeft;
  static float backgroundTop;
  static float backgroundWidth;
  static float backgroundHeight;
  static float gameBkTileWidth;
  static float gameBkTileHeight;
  static float sidePanelBkTileWidth;
  static float sidePanelBkTileHeight;
  static float holdNextFigureScale;
  static float menuRowGlowWidth;
  static float menuRowCornerSize;
  static float menuRowTextOffset;
  static float menuFontHeight;

  static LayoutObject gameLayout;
  static LayoutObject mainMenuLayout;
  static LayoutObject inGameMenuLayout;
  static LayoutObject quitConfirmationMenuLayout;
  static LayoutObject restartConfirmationMenuLayout;
  static LayoutObject exitToMainConfirmationMenuLayout;
  static LayoutObject saveSettingsMenuLayout;
  static void load(const char * name);
};
