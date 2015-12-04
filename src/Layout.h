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
  static LayoutObject gameLayout;
  static void load(const char * name);
};
