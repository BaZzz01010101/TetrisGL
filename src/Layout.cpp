#include "static_headers.h"

#include "Layout.h"
#include "InterfaceLogic.h"
#include "Crosy.h"

const float Layout::screenLeft = -1.0f;
const float Layout::screenTop = 1.0f;
const float Layout::screenWidth = 2.0f;
const float Layout::screenHeight = 2.0f;

ReadOnly<float, Layout> Layout::backgroundWidth = 1.5f;
ReadOnly<float, Layout> Layout::backgroundHeight = 2.0f;
ReadOnly<float, Layout> Layout::backgroundLeft = Layout::screenLeft + 0.5f * (screenWidth - backgroundWidth);
ReadOnly<float, Layout> Layout::backgroundTop = Layout::screenTop - 0.5f * (screenHeight - backgroundHeight);
ReadOnly<float, Layout> Layout::gameBkTileWidth = 0.03f;
ReadOnly<float, Layout> Layout::gameBkTileHeight = 0.02f;
ReadOnly<float, Layout> Layout::holdNextFigureScale = 0.0375f;
ReadOnly<float, Layout> Layout::menuRowGlowWidth = 0.03f;
ReadOnly<float, Layout> Layout::menuRowCornerSize = 0.03f;
ReadOnly<float, Layout> Layout::menuRowTextOffset = 0.15f;
ReadOnly<float, Layout> Layout::menuFontHeight = 0.08f;

float Layout::scoreBarTopGap = 0.01f;
float Layout::scoreBarLeftGap = 0.01f;
float Layout::scoreBarRightGap = 0.01f;
float Layout::scoreBarSeparatorGap = 0.01f;
float Layout::scoreBarHeight = 0.08f;
float Layout::scoreBarCaptionWidth = 0.45f;
float Layout::scoreBarMenuButtonWidth = 0.25f;

float Layout::glassWidth = 0.9f;
float Layout::glassHeight = 1.8f;
float Layout::glassTop= 0.85f;

float Layout::infoPanelsHorzGapFromGlass = 0.04f;
float Layout::infoPanelsWidth = 0.2f;
float Layout::holdNextCaptionsTop = -0.15f;
float Layout::holdNextPanelsTop = -0.19f;
float Layout::levelCaptionTop = -0.75f;
float Layout::levelPanelTop = -0.8f;
float Layout::goalCaptionTop = -0.95f;
float Layout::goalPanelTop = -1.0f;
float Layout::infoPanelsCaptionHeight = 0.05f;
float Layout::holdNextPanelsHeight = 0.2f;
float Layout::levelGoalPanelsHeight = 0.12f;

float Layout::menuTop = -0.5f;
float Layout::menuRowWidth = 0.75f;
float Layout::menuRowHeight = 0.12f;
float Layout::menuRowInterval = 0.08f;

LayoutObject Layout::gameLayout("Game", NULL, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
LayoutObject Layout::mainMenuLayout("MainMenu", NULL, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
LayoutObject Layout::inGameMenuLayout("InGameMenu", NULL, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
LayoutObject Layout::quitConfirmationMenuLayout("QuitConfirmationMenu", NULL, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
LayoutObject Layout::restartConfirmationMenuLayout("RestartConfirmationMenu", NULL, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
LayoutObject Layout::exitToMainConfirmationMenuLayout("ExitToMainConfirmationMenu", NULL, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
LayoutObject Layout::saveSettingsMenuLayout("SaveSettingsMenu", NULL, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);

void Layout::load(const char * name)
{
  rapidjson::Document doc;
  std::string fileName = Crosy::getExePath() + "/layouts/" + name + ".json";
  FILE * file = fopen(fileName.c_str(), "rb");
  assert(file);
  char buf[65536];
  rapidjson::FileReadStream frstream(file, buf, 65536);
  doc.ParseStream<rapidjson::FileReadStream>(frstream);
  fclose(file);

  assert(doc.HasMember("BackgroundWidth"));
  if (doc.HasMember("BackgroundWidth"))
    backgroundWidth = float(doc["BackgroundWidth"].GetDouble());

  assert(doc.HasMember("BackgroundHeight"));
  if (doc.HasMember("BackgroundHeight"))
    backgroundHeight = float(doc["BackgroundHeight"].GetDouble());

  backgroundLeft = screenLeft + 0.5f * (screenWidth - backgroundWidth);
  backgroundTop = screenTop - 0.5f * (screenHeight - backgroundHeight);

  assert(doc.HasMember("Game"));
  if (doc.HasMember("Game"))
  {
    rapidjson::Value & gameSection = doc["Game"];

    loadValue(gameSection, "BackgroundTileWidth", &gameBkTileWidth.value);
    loadValue(gameSection, "BackgroundTileHeight", &gameBkTileHeight.value);

    loadValue(gameSection, "ScoreBarTopGap", &scoreBarTopGap);
    loadValue(gameSection, "ScoreBarLeftGap", &scoreBarLeftGap);
    loadValue(gameSection, "ScoreBarRightGap", &scoreBarRightGap);
    loadValue(gameSection, "ScoreBarSeparatorGap", &scoreBarSeparatorGap);
    loadValue(gameSection, "ScoreBarHeight", &scoreBarHeight);
    loadValue(gameSection, "ScoreBarCaptionWidth", &scoreBarCaptionWidth);
    loadValue(gameSection, "ScoreBarMenuButtonWidth", &scoreBarMenuButtonWidth);

    loadValue(gameSection, "GlassWidth", &glassWidth);
    loadValue(gameSection, "GlassHeight", &glassHeight);
    loadValue(gameSection, "GlassTop", &glassTop);

    loadValue(gameSection, "InfoPanelsHorzGapFromGlass", &infoPanelsHorzGapFromGlass);
    loadValue(gameSection, "InfoPanelsWidth", &infoPanelsWidth);
    loadValue(gameSection, "HoldNextCaptionsTop", &holdNextCaptionsTop);
    loadValue(gameSection, "HoldNextPanelsTop", &holdNextPanelsTop);
    loadValue(gameSection, "LevelCaptionTop", &levelCaptionTop);
    loadValue(gameSection, "LevelPanelTop", &levelPanelTop);
    loadValue(gameSection, "GoalCaptionTop", &goalCaptionTop);
    loadValue(gameSection, "GoalPanelTop", &goalPanelTop);
    loadValue(gameSection, "InfoPanelsCaptionHeight", &infoPanelsCaptionHeight);
    loadValue(gameSection, "HoldNextPanelsHeight", &holdNextPanelsHeight);
    loadValue(gameSection, "LevelGoalPanelsHeight", &levelGoalPanelsHeight);

    holdNextFigureScale = 0.25f * 0.75f * glm::min(holdNextPanelsHeight, infoPanelsWidth);

  }

  assert(doc.HasMember("Interface"));
  if (doc.HasMember("Interface"))
  {
    rapidjson::Value & intrfaceSection = doc["Interface"];

    loadValue(intrfaceSection, "MenuTop", &menuTop);
    loadValue(intrfaceSection, "MenuRowWidth", &menuRowWidth);
    loadValue(intrfaceSection, "MenuRowHeight", &menuRowHeight);
    loadValue(intrfaceSection, "MenuRowInterval", &menuRowInterval);
    loadValue(intrfaceSection, "MenuRowGlowWidth", &menuRowGlowWidth.value);
    loadValue(intrfaceSection, "MenuRowCornerSize", &menuRowCornerSize.value);
    loadValue(intrfaceSection, "MenuRowTextOffset", &menuRowTextOffset.value);
    loadValue(intrfaceSection, "MenuFontHeight", &menuFontHeight.value);
  }

  gameLayout.clear();

  const float scoreBarValueLeft = scoreBarLeftGap + scoreBarCaptionWidth + scoreBarSeparatorGap;
  const float scoreBarValueWidth = backgroundWidth - scoreBarLeftGap - scoreBarCaptionWidth - scoreBarSeparatorGap - scoreBarSeparatorGap - scoreBarMenuButtonWidth - scoreBarRightGap;
  const float scoreBarMenuButtonLeft = scoreBarValueLeft + scoreBarValueWidth + scoreBarSeparatorGap;
  gameLayout.addChild("ScoreBarCaption", scoreBarLeftGap, -scoreBarTopGap, scoreBarCaptionWidth, scoreBarHeight);
  gameLayout.addChild("ScoreBarValue", scoreBarValueLeft, -scoreBarTopGap, scoreBarValueWidth, scoreBarHeight);
  gameLayout.addChild("ScoreBarMenuButton", scoreBarMenuButtonLeft, -scoreBarTopGap, scoreBarMenuButtonWidth, scoreBarHeight);

  const float glassLeft = 0.5f * (backgroundWidth - glassWidth);
  gameLayout.addChild("Glass", glassLeft, glassTop, glassWidth, glassHeight);

  const float leftSidePanelsLeft = glassLeft - infoPanelsHorzGapFromGlass - infoPanelsWidth;
  const float rightSidePanelsLeft = glassLeft + glassWidth + infoPanelsHorzGapFromGlass;
  gameLayout.addChild("HoldPanelCaption", leftSidePanelsLeft, holdNextCaptionsTop, infoPanelsWidth, infoPanelsCaptionHeight);
  gameLayout.addChild("NextPanelCaption", rightSidePanelsLeft, holdNextCaptionsTop, infoPanelsWidth, infoPanelsCaptionHeight);
  gameLayout.addChild("HoldPanel", leftSidePanelsLeft, holdNextPanelsTop, infoPanelsWidth, holdNextPanelsHeight);
  gameLayout.addChild("NextPanel", rightSidePanelsLeft, holdNextPanelsTop, infoPanelsWidth, holdNextPanelsHeight);
  gameLayout.addChild("LevelPanelCaption", leftSidePanelsLeft, levelCaptionTop, infoPanelsWidth, infoPanelsCaptionHeight);
  gameLayout.addChild("LevelPanel", leftSidePanelsLeft, levelPanelTop, infoPanelsWidth, levelGoalPanelsHeight);
  gameLayout.addChild("GoalPanelCaption", leftSidePanelsLeft, goalCaptionTop, infoPanelsWidth, infoPanelsCaptionHeight);
  gameLayout.addChild("GoalPanel", leftSidePanelsLeft, goalPanelTop, infoPanelsWidth, levelGoalPanelsHeight);

  mainMenuLayout.addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::mainMenu.rowCount; i++)
    mainMenuLayout.addRow(i ? menuRowInterval : -menuTop, menuRowHeight);

  inGameMenuLayout.addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::inGameMenu.rowCount; i++)
    inGameMenuLayout.addRow(i ? menuRowInterval : -menuTop, menuRowHeight);

  quitConfirmationMenuLayout.addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::quitConfirmationMenu.rowCount; i++)
    quitConfirmationMenuLayout.addRow(i ? menuRowInterval : -menuTop, menuRowHeight);

  restartConfirmationMenuLayout.addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::restartConfirmationMenu.rowCount; i++)
    restartConfirmationMenuLayout.addRow(i ? menuRowInterval : -menuTop, menuRowHeight);

  exitToMainConfirmationMenuLayout.addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::exitToMainConfirmationMenu.rowCount; i++)
    exitToMainConfirmationMenuLayout.addRow(i ? menuRowInterval : -menuTop, menuRowHeight);

  saveSettingsMenuLayout.addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::saveSettingsMenu.rowCount; i++)
    saveSettingsMenuLayout.addRow(i ? menuRowInterval : -menuTop, menuRowHeight);
}

void Layout::loadValue(rapidjson::Value & source, const char * name, float * result)
{
  assert(source.HasMember(name));
  if (source.HasMember(name))
    *result = float(source[name].GetDouble());
}

