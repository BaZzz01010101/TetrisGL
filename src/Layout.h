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

  Layout();
  ~Layout();

public:
  static ReadOnly<float, Layout> backgroundLeft;
  static ReadOnly<float, Layout> backgroundTop;
  static ReadOnly<float, Layout> backgroundWidth;
  static ReadOnly<float, Layout> backgroundHeight;
  static ReadOnly<float, Layout> gameBkTileWidth;
  static ReadOnly<float, Layout> gameBkTileHeight;
  static LayoutObject gameLayout;
  static void load(const char * name);
};
