#pragma once

#include "LayoutObject.h"

class Layout
{
private:
  static void loadValue(rapidjson::Value & source, const char * name, float * result);

  Layout();
  ~Layout();

public:
  static const float screenLeft;
  static const float screenTop;
  static const float screenWidth;
  static const float screenHeight;

  static float backgroundLeft;
  static float backgroundTop;
  static float backgroundWidth;
  static float backgroundHeight;

  static float gameBkTileWidth;
  static float gameBkTileHeight;

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
  static float levelGoalTextHeight;
  static float holdNextFigureScale;

  static float sidePanelBkTileWidth;
  static float sidePanelBkTileHeight;

  static float menuTop;
  static float menuRowWidth;
  static float menuRowHeight;
  static float menuRowInterval;
  static float menuRowGlowWidth;
  static float menuRowCornerSize;
  static float menuRowTextOffset;
  static float menuFontHeight;

  static float settingsTop;
  static float settingsWidth;
  static float settingsHeight;
  static float settingsGlowWidth;
  static float settingsCornerSize;

  static float settingsTitleLeft;
  static float settingsTitleTop;
  static float settingsTitleHeight;
  static float settingsTitleShadowDX;
  static float settingsTitleShadowDY;

  static float settingsPanelLeft;
  static float settingsPanelTop;
  static float settingsPanelRightGap;
  static float settingsPanelBottomGap;
  static float settingsPanelBorderWidth;

  static float settingsBackShevronSize;
  static float settingsBackShevronStep;
  static float settingsBackShevronRightGap;

  static float settingsPanelTitleLeft;
  static float settingsPanelTitleTopGap;
  static float settingsPanelTitleBottomGap;
  static float settingsPanelTitleHeight;
  static float settingsPanelRowHorzGap;
  static float settingsPanelRowVertGap;
  static float settingsPanelRowHeight;
  static float settingsPanelRowCaptionIndent;
  static float settingsPanelRowCaptionHeight;
  static float settingsKeyBindingCaptionWidth;

  static float settingsProgressBarWidth;
  static float settingsProgressBarOuterGap;
  static float settingsProgressBarBorder;
  static float settingsProgressBarInnerGap;

  static float settingsBindingMsgWidth;
  static float settingsBindingMsgHeight;
  static float settingsBindingMsgBorder;
  static float settingsBindingMsgTextHeight;

  static LayoutObject screen;

  static void load(const char * name);
};
