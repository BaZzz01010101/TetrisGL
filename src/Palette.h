#pragma once

#include "Figure.h"

class Palette
{
public:
  static glm::vec3 gameBackgroundOuter;
  static glm::vec3 gameBackgroundInner;
  static glm::vec3 fieldBackgroundMin;
  static glm::vec3 fieldBackgroundMax;
  static glm::vec3 scoreBarBackground;
  static float scoreBarBackgroundAlpha;
  static glm::vec3 scoreBarMenuButtonBackground;
  static glm::vec3 scoreBarMenuButtonHighlightedBackground;
  static glm::vec3 scoreBarText;
  static glm::vec3 scoreBarMenuButtonText;
  static glm::vec3 scoreBarMenuButtonHighlightedText;
  static glm::vec3 holdCaptionText;
  static glm::vec3 holdEmptyPanel;
  static glm::vec3 nextCaptionText;
  static glm::vec3 nextEmptyPanel;
  static glm::vec3 levelCaptionText;
  static glm::vec3 levelPanelBackground;
  static glm::vec3 levelPanelText;
  static glm::vec3 goalCaptionText;
  static glm::vec3 goalPanelBackground;
  static glm::vec3 goalPanelText;
  static glm::vec3 figureShadow;

  static glm::vec3 cellColorArray[Cell::COLOR_COUNT];

  static glm::vec3 menuNormalRowBackgroundTop;
  static glm::vec3 menuNormalRowBackgroundBottom;
  static glm::vec3 menuSelectedRowBackgroundTop;
  static glm::vec3 menuSelectedRowBackgroundBottom;
  static glm::vec3 menuNormalRowText;
  static glm::vec3 menuSelectedRowText;
  static glm::vec3 menuNormalRowGlow;
  static glm::vec3 menuSelectedRowGlow;

  static glm::vec3 settingsBackgroundTop;
  static glm::vec3 settingsBackgroundBottom;
  static glm::vec3 settingsGlow;
  static glm::vec3 settingsTitleText;
  static float settingsTitleShadowAlpha;
  static float settingsTitleShadowBlur;
  static glm::vec3 settingsPanelBackgroundTop;
  static glm::vec3 settingsPanelBackgroundBottom;
  static glm::vec3 settingsPanelBorder;
  static glm::vec3 settingsPanelTitleText;
  static glm::vec3 settingsActiveRowBackground;
  static glm::vec3 settingsInactiveRowBackground;
  static glm::vec3 settingsMouseoverRowBackground;
  static glm::vec3 settingsActiveRowText;
  static glm::vec3 settingsInactiveRowText;
  static glm::vec3 settingsMouseoverRowText;
  static glm::vec3 settingsProgressBarForeground;
  static glm::vec3 settingsProgressBarBackground;
  static glm::vec3 settingsBackButton;
  static glm::vec3 settingsBackButtonHighlighted;
  static glm::vec3 settingsBindingMsgBackground;
  static glm::vec3 settingsBindingMsgBorder;
  static glm::vec3 settingsBindingMsgText;

  static glm::vec3 leaderboardBackgroundTop;
  static glm::vec3 leaderboardBackgroundBottom;
  static glm::vec3 leaderboardGlow;
  static glm::vec3 leaderboardTitleText;
  static float leaderboardTitleShadowAlpha;
  static float leaderboardTitleShadowBlur;
  static glm::vec3 leaderboardPanelBackgroundTop;
  static glm::vec3 leaderboardPanelBackgroundBottom;
  static glm::vec3 leaderboardPanelBorder;
  static glm::vec3 leaderboardPanelHeaderText;
  static glm::vec3 leaderboardRowText;
  static glm::vec3 leaderboardEditRowText;
  static glm::vec3 leaderboardBackButton;
  static glm::vec3 leaderboardBackButtonHighlighted;
    
  static float backgroundShadeAlpha;
  static float figureGlowOuterBright;
  static float figureGlowInnerBright;
  static float deletedRowFlashBright;
  static float deletedRowRaysBright;
  static float deletedRowShineBright;
  static float fieldBackgroundInnerBright;

  static void load(const char * name);

private:

  Palette();
  ~Palette();

  static void loadValue(rapidjson::Value & source, const char * name, float * result);
  static void loadValue(rapidjson::Value & source, const char * name, glm::vec3 * result);
};

