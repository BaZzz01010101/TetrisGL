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

  static float fieldWidth;
  static float fieldHeight;
  static float fieldTop;

  static float infoPanelsHorzGapFromField;
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

  static float leaderboardTop;
  static float leaderboardWidth;
  static float leaderboardHeight;
  static float leaderboardGlowWidth;
  static float leaderboardCornerSize;

  static float leaderboardTitleLeft;
  static float leaderboardTitleTop;
  static float leaderboardTitleHeight;
  static float leaderboardTitleShadowDX;
  static float leaderboardTitleShadowDY;

  static float leaderboardPanelLeft;
  static float leaderboardPanelTop;
  static float leaderboardPanelRightGap;
  static float leaderboardPanelBottomGap;
  static float leaderboardPanelBorderWidth;

  static float leaderboardPanelHeaderTop;
  static float leaderboardPanelHeaderHeight;
  static float leaderboardPanelHeaderTextHeight;
  static float leaderboardPanelRowHeight;
  static float leaderboardPanelRowTextHeight;
  static float leaderboardPanelColPlaceWidth;
  static float leaderboardPanelColNameWidth;
  static float leaderboardPanelColLevelWidth;
  static float leaderboardPanelColScoreWidth;
  static float leaderboardPanelNameLeftIndent;
  static float leaderboardPanelScoreRightIndent;
  static float leaderboardPanelLastRowBottomGap;

  static float leaderboardBackShevronSize;
  static float leaderboardBackShevronStep;
  static float leaderboardBackShevronRightGap;

  static LayoutObject screen;

  static void load(const char * name);
};
