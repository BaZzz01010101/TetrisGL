#include "static_headers.h"

#include "Palette.h"
#include "Crosy.h"

glm::vec3 Palette::gameBackgroundOuter(0.05f, 0.1f, 0.2f);
glm::vec3 Palette::gameBackgroundInner(0.3f, 0.6f, 1.0f);
glm::vec3 Palette::fieldBackgroundMin(0.1f, 0.1f, 0.2f);
glm::vec3 Palette::fieldBackgroundMax(0.3f, 0.3f, 0.5f);
glm::vec3 Palette::scoreBarBackground(0.0f, 0.0f, 0.0f);
float Palette::scoreBarBackgroundAlpha = 0.6f;
glm::vec3 Palette::scoreBarMenuButtonBackground(0.3f, 0.6f, 0.9f);
glm::vec3 Palette::scoreBarMenuButtonHighlightedBackground(0.5f, 0.75f, 1.3f);
glm::vec3 Palette::scoreBarText(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::scoreBarMenuButtonText(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::scoreBarMenuButtonHighlightedText(1.0f, 0.9f, 0.4f);
glm::vec3 Palette::holdCaptionText(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::holdEmptyPanel(0.5f, 0.5f, 0.5f);
glm::vec3 Palette::nextCaptionText(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::nextEmptyPanel(0.5f, 0.5f, 0.5f);
glm::vec3 Palette::levelCaptionText(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::levelPanelBackground(0.2f, 0.45f, 0.8f);
glm::vec3 Palette::levelPanelText(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::goalCaptionText(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::goalPanelBackground(0.2f, 0.45f, 0.8f);
glm::vec3 Palette::goalPanelText(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::figureShadow(0.0f, 0.0f, 0.0f);

glm::vec3 Palette::cellColorArray[Cell::Color::COLOR_COUNT] =
{
  { 1.0f, 0.05f, 0.05f },
  { 1.0f, 0.35f, 0.0f },
  { 0.85f, 0.6f, 0.0f },
  { 0.05f, 0.55f, 0.1f },
  { 0.05f, 0.6f, 1.0f },
  { 0.1f, 0.2f, 0.9f },
  { 0.4f, 0.1f, 0.8f },
};

glm::vec3 Palette::menuNormalRowBackgroundTop(0.4f, 0.7f, 1.2f);
glm::vec3 Palette::menuNormalRowBackgroundBottom(0.04f, 0.07f, 0.12f);
glm::vec3 Palette::menuSelectedRowBackgroundTop(2.0f, 1.9f, 1.2f);
glm::vec3 Palette::menuSelectedRowBackgroundBottom(0.2f, 0.19f, 0.12f);
glm::vec3 Palette::menuNormalRowText(0.4f, 0.7f, 1.0f);
glm::vec3 Palette::menuSelectedRowText(1.0f, 0.9f, 0.4f);
glm::vec3 Palette::menuNormalRowGlow(0.4f, 0.7f, 1.2f);
glm::vec3 Palette::menuSelectedRowGlow(2.0f, 1.9f, 1.2);

glm::vec3 Palette::settingsBackgroundTop(0.4f, 0.7f, 1.2f);
glm::vec3 Palette::settingsBackgroundBottom(0.04f, 0.07f, 0.12f);
glm::vec3 Palette::settingsGlow(0.4f, 0.7f, 1.2f);
glm::vec3 Palette::settingsTitleText(1.0f, 0.9f, 0.4f);
float Palette::settingsTitleShadowAlpha = 0.5f;
float Palette::settingsTitleShadowBlur = 0.75f;
glm::vec3 Palette::settingsPanelBackgroundTop(0.04f, 0.14f, 0.24f);
glm::vec3 Palette::settingsPanelBackgroundBottom(0.01f, 0.035f, 0.0625f);
glm::vec3 Palette::settingsPanelBorder(0.5f, 0.45f, 0.2f);
glm::vec3 Palette::settingsPanelTitleText(0.65f, 0.65f, 0.65f);
glm::vec3 Palette::settingsActiveRowBackground(0.75f, 0.75f, 0.75f);
glm::vec3 Palette::settingsInactiveRowBackground(0.25f, 0.25f, 0.25f);
glm::vec3 Palette::settingsMouseoverRowBackground(0.35f, 0.35f, 0.35f);
glm::vec3 Palette::settingsActiveRowText(0.25f, 0.25f, 0.25f);
glm::vec3 Palette::settingsInactiveRowText(0.75f, 0.75f, 0.75f);
glm::vec3 Palette::settingsMouseoverRowText(0.75f, 0.75f, 0.75f);
glm::vec3 Palette::settingsProgressBarForeground(0.25f, 0.25f, 0.25f);
glm::vec3 Palette::settingsProgressBarBackground(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::settingsBackButton(0.5f, 0.5f, 0.5f);
glm::vec3 Palette::settingsBackButtonHighlighted(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::settingsBindingMsgBackground(0.1f, 0.2f, 0.3f);
glm::vec3 Palette::settingsBindingMsgBorder(0.9f, 0.9f, 1.0f);
glm::vec3 Palette::settingsBindingMsgText(0.9f, 0.9f, 1.0f);

glm::vec3 Palette::leaderboardBackgroundTop(0.4f, 0.7f, 1.2f);
glm::vec3 Palette::leaderboardBackgroundBottom(0.04f, 0.07f, 0.12f);
glm::vec3 Palette::leaderboardGlow(0.4f, 0.7f, 1.2f);
glm::vec3 Palette::leaderboardTitleText(1.0f, 0.9f, 0.4f);
float Palette::leaderboardTitleShadowAlpha = 0.5f;
float Palette::leaderboardTitleShadowBlur = 0.5f;
glm::vec3 Palette::leaderboardPanelBackgroundTop(0.04f, 0.14f, 0.24f);
glm::vec3 Palette::leaderboardPanelBackgroundBottom(0.01f, 0.035f, 0.0625f);
glm::vec3 Palette::leaderboardPanelBorder(0.5f, 0.45f, 0.2f);
glm::vec3 Palette::leaderboardPanelHeaderText(0.85f, 0.85f, 0.65f);
glm::vec3 Palette::leaderboardRowText(0.9f, 0.9f, 1.0f);
glm::vec3 Palette::leaderboardEditRowText(1.0f, 0.9f, 0.4f);
glm::vec3 Palette::leaderboardBackButton(0.5f, 0.5f, 0.6f);
glm::vec3 Palette::leaderboardBackButtonHighlighted(0.9f, 0.9f, 1.0f);

float Palette::backgroundShadeAlpha = 0.75f;
float Palette::figureGlowOuterBright = 0.01f;
float Palette::figureGlowInnerBright = 0.25f;
float Palette::deletedRowFlashBright = 1.0f;
float Palette::deletedRowRaysBright = 1.0f;
float Palette::deletedRowShineBright = 1.0f;
float Palette::fieldBackgroundInnerBright = 0.25f;

void Palette::load(const char * name)
{
  rapidjson::Document doc;
  std::string fileName = Crosy::getExePath() + "/palettes/" + name + ".json";
  FILE * file = fopen(fileName.c_str(), "rb");
  assert(file);
  char buf[65536];
  rapidjson::FileReadStream frstream(file, buf, 65536);
  doc.ParseStream<rapidjson::FileReadStream>(frstream);
  fclose(file);

  loadValue(doc, "GameBackgroundOuter", &gameBackgroundOuter);
  loadValue(doc, "GameBackgroundInner", &gameBackgroundInner);
  loadValue(doc, "FieldBackgroundMin", &fieldBackgroundMin);
  loadValue(doc, "FieldBackgroundMax", &fieldBackgroundMax);
  loadValue(doc, "ScoreBarBackground", &scoreBarBackground);
  loadValue(doc, "ScoreBarBackgroundAlpha", &scoreBarBackgroundAlpha);
  loadValue(doc, "ScoreBarMenuButtonBackground", &scoreBarMenuButtonBackground);
  loadValue(doc, "ScoreBarMenuButtonHighlightedBackground", &scoreBarMenuButtonHighlightedBackground);
  loadValue(doc, "ScoreBarText", &scoreBarText);
  loadValue(doc, "ScoreBarMenuButtonText", &scoreBarMenuButtonText);
  loadValue(doc, "ScoreBarMenuButtonHighlightedText", &scoreBarMenuButtonHighlightedText);
  loadValue(doc, "HoldCaptionText", &holdCaptionText);
  loadValue(doc, "HoldEmptyPanel", &holdEmptyPanel);
  loadValue(doc, "NextCaptionText", &nextCaptionText);
  loadValue(doc, "NextEmptyPanel", &nextEmptyPanel);
  loadValue(doc, "LevelCaptionText", &levelCaptionText);
  loadValue(doc, "LevelPanelBackground", &levelPanelBackground);
  loadValue(doc, "LevelPanelText", &levelPanelText);
  loadValue(doc, "GoalCaptionText", &goalCaptionText);
  loadValue(doc, "GoalPanelBackground", &goalPanelBackground);
  loadValue(doc, "GoalPanelText", &goalPanelText);
  loadValue(doc, "FigureShadow", &figureShadow);

  loadValue(doc, "CellRed", &cellColorArray[Cell::Color::clRed]);
  loadValue(doc, "CellOrange", &cellColorArray[Cell::Color::clOrange]);
  loadValue(doc, "CellYellow", &cellColorArray[Cell::Color::clYellow]);
  loadValue(doc, "CellGreen", &cellColorArray[Cell::Color::clGreen]);
  loadValue(doc, "CellCyan", &cellColorArray[Cell::Color::clCyan]);
  loadValue(doc, "CellBlue", &cellColorArray[Cell::Color::clBlue]);
  loadValue(doc, "CellPurple", &cellColorArray[Cell::Color::clPurple]);

  loadValue(doc, "MenuNormalRowBackgroundTop", &menuNormalRowBackgroundTop);
  loadValue(doc, "MenuNormalRowBackgroundBottom", &menuNormalRowBackgroundBottom);
  loadValue(doc, "MenuSelectedRowBackgroundTop", &menuSelectedRowBackgroundTop);
  loadValue(doc, "MenuSelectedRowBackgroundBottom", &menuSelectedRowBackgroundBottom);
  loadValue(doc, "MenuNormalRowText", &menuNormalRowText);
  loadValue(doc, "MenuSelectedRowText", &menuSelectedRowText);
  loadValue(doc, "MenuNormalRowGlow", &menuNormalRowGlow);
  loadValue(doc, "MenuSelectedRowGlow", &menuSelectedRowGlow);

  loadValue(doc, "SettingsBackgroundTop", &settingsBackgroundTop);
  loadValue(doc, "SettingsBackgroundBottom", &settingsBackgroundBottom);
  loadValue(doc, "SettingsGlow", &settingsGlow);
  loadValue(doc, "SettingsTitleText", &settingsTitleText);
  loadValue(doc, "SettingsTitleShadowAlpha", &settingsTitleShadowAlpha);
  loadValue(doc, "SettingsTitleShadowBlur", &settingsTitleShadowBlur);
  loadValue(doc, "SettingsPanelBackgroundTop", &settingsPanelBackgroundTop);
  loadValue(doc, "SettingsPanelBackgroundBottom", &settingsPanelBackgroundBottom);
  loadValue(doc, "SettingsPanelBorder", &settingsPanelBorder);
  loadValue(doc, "SettingsPanelTitleText", &settingsPanelTitleText);
  loadValue(doc, "SettingsActiveRowBackground", &settingsActiveRowBackground);
  loadValue(doc, "SettingsInactiveRowBackground", &settingsInactiveRowBackground);
  loadValue(doc, "SettingsMouseoverRowBackground", &settingsMouseoverRowBackground);
  loadValue(doc, "SettingsActiveRowText", &settingsActiveRowText);
  loadValue(doc, "SettingsInactiveRowText", &settingsInactiveRowText);
  loadValue(doc, "SettingsMouseoverRowText", &settingsMouseoverRowText);
  loadValue(doc, "SettingsProgressBarForeground", &settingsProgressBarForeground);
  loadValue(doc, "SettingsProgressBarBackground", &settingsProgressBarBackground);
  loadValue(doc, "SettingsBackButton", &settingsBackButton);
  loadValue(doc, "SettingsBackButtonHighlighted", &settingsBackButtonHighlighted);
  loadValue(doc, "SettingsBindingMsgBackground", &settingsBindingMsgBackground);
  loadValue(doc, "SettingsBindingMsgBorder", &settingsBindingMsgBorder);
  loadValue(doc, "SettingsBindingMsgText", &settingsBindingMsgText);

  loadValue(doc, "LeaderboardBackgroundTop", &leaderboardBackgroundTop);
  loadValue(doc, "LeaderboardBackgroundBottom", &leaderboardBackgroundBottom);
  loadValue(doc, "LeaderboardGlow", &leaderboardGlow);
  loadValue(doc, "LeaderboardTitleText", &leaderboardTitleText);
  loadValue(doc, "LeaderboardTitleShadowAlpha", &leaderboardTitleShadowAlpha);
  loadValue(doc, "LeaderboardTitleShadowBlur", &leaderboardTitleShadowBlur);
  loadValue(doc, "LeaderboardPanelBackgroundTop", &leaderboardPanelBackgroundTop);
  loadValue(doc, "LeaderboardPanelBackgroundBottom", &leaderboardPanelBackgroundBottom);
  loadValue(doc, "LeaderboardPanelBorder", &leaderboardPanelBorder);
  loadValue(doc, "LeaderboardPanelHeaderText", &leaderboardPanelHeaderText);
  loadValue(doc, "LeaderboardRowText", &leaderboardRowText);
  loadValue(doc, "LeaderboardEditRowText", &leaderboardEditRowText);
  loadValue(doc, "LeaderboardBackButton", &leaderboardBackButton);
  loadValue(doc, "LeaderboardBackButtonHighlighted", &leaderboardBackButtonHighlighted);

  loadValue(doc, "BackgroundShadeAlpha", &backgroundShadeAlpha);
  loadValue(doc, "FigureGlowOuterBright", &figureGlowOuterBright);
  loadValue(doc, "FigureGlowInnerBright", &figureGlowInnerBright);
  loadValue(doc, "DeletedRowFlashBright", &deletedRowFlashBright);
  loadValue(doc, "DeletedRowRaysBright", &deletedRowRaysBright);
  loadValue(doc, "DeletedRowShineBright", &deletedRowShineBright);
  loadValue(doc, "FieldBackgroundInnerBright", &fieldBackgroundInnerBright);
}

void Palette::loadValue(rapidjson::Value & source, const char * name, float * result)
{
  assert(source.HasMember(name));
  if (source.HasMember(name))
    *result = float(source[name].GetDouble());
}

void Palette::loadValue(rapidjson::Value & source, const char * name, glm::vec3 * result)
{
  assert(source.HasMember(name));
  if (source.HasMember(name) && source[name].Capacity() == 3)
  {
    result->r = float(source[name][0].GetDouble());
    result->g = float(source[name][1].GetDouble());
    result->b = float(source[name][2].GetDouble());
  }
}

