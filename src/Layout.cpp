#include "static_headers.h"

#include "Layout.h"
#include "InterfaceLogic.h"
#include "Binding.h"
#include "Crosy.h"

const float Layout::screenLeft = 0.0f;
const float Layout::screenTop = 0.0f;
const float Layout::screenWidth = 1.0f;
const float Layout::screenHeight = 1.0f;

float Layout::backgroundWidth = 1.5f;
float Layout::backgroundHeight = 2.0f;
float Layout::backgroundLeft = Layout::screenLeft + 0.5f * (screenWidth - backgroundWidth);
float Layout::backgroundTop = Layout::screenTop - 0.5f * (screenHeight - backgroundHeight);
float Layout::gameBkTileWidth = 0.03f;
float Layout::gameBkTileHeight = 0.02f;
float Layout::sidePanelBkTileWidth = 0.024f;
float Layout::sidePanelBkTileHeight = 0.016f;
float Layout::holdNextFigureScale = 0.0375f;
float Layout::menuRowGlowWidth = 0.03f;
float Layout::menuRowCornerSize = 0.03f;
float Layout::menuRowTextOffset = 0.15f;
float Layout::menuFontHeight = 0.08f;

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
float Layout::levelGoalTextHeight = 0.12f;

float Layout::menuTop = -0.5f;
float Layout::menuRowWidth = 0.75f;
float Layout::menuRowHeight = 0.12f;
float Layout::menuRowInterval = 0.08f;

float Layout::settingsTop = -0.5f;
float Layout::settingsWidth = 1.25f;
float Layout::settingsHeight = 1.0f;
float Layout::settingsGlowWidth = 0.05f;
float Layout::settingsCornerSize = 0.08f;

float Layout::settingsTitleLeft = 0.05f;
float Layout::settingsTitleTop = -0.08f;
float Layout::settingsTitleHeight = 0.08f;
float Layout::settingsTitleShadowDX = 0.01f;
float Layout::settingsTitleShadowDY = 0.01f;

float Layout::settingsPanelTitleLeft = 0.04f;
float Layout::settingsPanelLeft = 0.05f;
float Layout::settingsPanelTop = 0.12f;
float Layout::settingsPanelRightGap = 0.05f;
float Layout::settingsPanelBottomGap = 0.08f;
float Layout::settingsPanelBorderWidth = 0.005f;

float Layout::settingsBackShevronSize = 0.05f;
float Layout::settingsBackShevronStep = 0.04f;
float Layout::settingsBackShevronRightGap = 0.05f;

float Layout::settingsPanelTitleTopGap = 0.02f;
float Layout::settingsPanelTitleBottomGap = 0.01f;
float Layout::settingsPanelTitleHeight = 0.08f;
float Layout::settingsPanelRowHorzGap = 0.01f;
float Layout::settingsPanelRowVertGap = 0.01f;
float Layout::settingsPanelRowHeight = 0.06f;
float Layout::settingsPanelRowCaptionIndent = 0.01f;
float Layout::settingsPanelRowCaptionHeight = 0.05f;
float Layout::settingsKeyBindingCaptionWidth = 0.6f;

float Layout::settingsProgressBarWidth = 0.8f;
float Layout::settingsProgressBarOuterGap = 0.01f;
float Layout::settingsProgressBarBorder = 0.005f;
float Layout::settingsProgressBarInnerGap = 0.01f;

LayoutObject Layout::screen(loScreen, NULL, screenLeft, screenTop, screenWidth, screenHeight);

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

  loadValue(doc, "BackgroundWidth", &backgroundWidth);
  loadValue(doc, "BackgroundHeight", &backgroundHeight);
  backgroundLeft = 0.5f * (1.0f - backgroundWidth);
  backgroundTop = 0.5f * (1.0f - backgroundHeight);

  assert(doc.HasMember("Game"));

  if (doc.HasMember("Game"))
  {
    rapidjson::Value & gameSection = doc["Game"];

    loadValue(gameSection, "GameBkTileWidth", &gameBkTileWidth);
    loadValue(gameSection, "GameBkTileHeight", &gameBkTileHeight);

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
    loadValue(gameSection, "LevelGoalTextHeight", &levelGoalTextHeight);

    holdNextFigureScale = 0.25f * 0.75f * glm::min(holdNextPanelsHeight, infoPanelsWidth);
  }

  assert(doc.HasMember("Interface"));

  if (doc.HasMember("Interface"))
  {
    rapidjson::Value & intrfaceSection = doc["Interface"];

    loadValue(intrfaceSection, "SidePanelBkTileWidth", &sidePanelBkTileWidth);
    loadValue(intrfaceSection, "SidePanelBkTileHeight", &sidePanelBkTileHeight);

    loadValue(intrfaceSection, "MenuTop", &menuTop);
    loadValue(intrfaceSection, "MenuRowWidth", &menuRowWidth);
    loadValue(intrfaceSection, "MenuRowHeight", &menuRowHeight);
    loadValue(intrfaceSection, "MenuRowInterval", &menuRowInterval);
    loadValue(intrfaceSection, "MenuRowGlowWidth", &menuRowGlowWidth);
    loadValue(intrfaceSection, "MenuRowCornerSize", &menuRowCornerSize);
    loadValue(intrfaceSection, "MenuRowTextOffset", &menuRowTextOffset);
    loadValue(intrfaceSection, "MenuFontHeight", &menuFontHeight);

    loadValue(intrfaceSection, "SettingsTop", &settingsTop);
    loadValue(intrfaceSection, "SettingsWidth", &settingsWidth);
    loadValue(intrfaceSection, "SettingsHeight", &settingsHeight);
    loadValue(intrfaceSection, "SettingsGlowWidth", &settingsGlowWidth);
    loadValue(intrfaceSection, "SettingsCornerSize", &settingsCornerSize);

    loadValue(intrfaceSection, "SettingsTitleLeft", &settingsTitleLeft);
    loadValue(intrfaceSection, "SettingsTitleTop", &settingsTitleTop);
    loadValue(intrfaceSection, "SettingsTitleHeight", &settingsTitleHeight);
    loadValue(intrfaceSection, "SettingsTitleShadowDX", &settingsTitleShadowDX);
    loadValue(intrfaceSection, "SettingsTitleShadowDY", &settingsTitleShadowDY);

    loadValue(intrfaceSection, "SettingsPanelLeft", &settingsPanelLeft);
    loadValue(intrfaceSection, "SettingsPanelTop", &settingsPanelTop);
    loadValue(intrfaceSection, "SettingsPanelRightGap", &settingsPanelRightGap);
    loadValue(intrfaceSection, "SettingsPanelBottomGap", &settingsPanelBottomGap);
    loadValue(intrfaceSection, "SettingsPanelBorderWidth", &settingsPanelBorderWidth);

    loadValue(intrfaceSection, "SettingsBackShevronSize", &settingsBackShevronSize);
    loadValue(intrfaceSection, "SettingsBackShevronStep", &settingsBackShevronStep);
    loadValue(intrfaceSection, "SettingsBackShevronRightGap", &settingsBackShevronRightGap);

    
    loadValue(intrfaceSection, "SettingsPanelTitleLeft", &settingsPanelTitleLeft);
    loadValue(intrfaceSection, "SettingsPanelTitleTopGap", &settingsPanelTitleTopGap);
    loadValue(intrfaceSection, "SettingsPanelTitleBottomGap", &settingsPanelTitleBottomGap);
    loadValue(intrfaceSection, "SettingsPanelTitleHeight", &settingsPanelTitleHeight);
    loadValue(intrfaceSection, "SettingsPanelRowHorzGap", &settingsPanelRowHorzGap);
    loadValue(intrfaceSection, "SettingsPanelRowVertGap", &settingsPanelRowVertGap);
    loadValue(intrfaceSection, "SettingsPanelRowHeight", &settingsPanelRowHeight);
    loadValue(intrfaceSection, "SettingsPanelRowCaptionIndent", &settingsPanelRowCaptionIndent);
    loadValue(intrfaceSection, "SettingsPanelRowCaptionHeight", &settingsPanelRowCaptionHeight);
    loadValue(intrfaceSection, "SettingsKeyBindingCaptionWidth", &settingsKeyBindingCaptionWidth);

    loadValue(intrfaceSection, "SettingsProgressBarWidth", &settingsProgressBarWidth);
    loadValue(intrfaceSection, "SettingsProgressBarOuterGap", &settingsProgressBarOuterGap);
    loadValue(intrfaceSection, "SettingsProgressBarBorder", &settingsProgressBarBorder);
    loadValue(intrfaceSection, "SettingsProgressBarInnerGap", &settingsProgressBarInnerGap);
  }

  screen.clear();

  LayoutObject * gameLayout = screen.addChild(loGame, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
  LayoutObject * mainMenuLayout = screen.addChild(loMainMenu, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
  LayoutObject * inGameMenuLayout = screen.addChild(loInGameMenu, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
  LayoutObject * quitConfirmationMenuLayout = screen.addChild(loQuitConfirmationMenu, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
  LayoutObject * restartConfirmationMenuLayout = screen.addChild(loRestartConfirmationMenu, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
  LayoutObject * exitToMainConfirmationMenuLayout = screen.addChild(loExitToMainConfirmationMenu, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
  LayoutObject * saveSettingsMenuLayout = screen.addChild(loSaveSettingsMenu, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);
  LayoutObject * settingsLayout = screen.addChild(loSettings, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);

  const float scoreBarValueLeft = scoreBarLeftGap + scoreBarCaptionWidth + scoreBarSeparatorGap;
  const float scoreBarValueWidth = backgroundWidth - scoreBarLeftGap - scoreBarCaptionWidth - scoreBarSeparatorGap - scoreBarSeparatorGap - scoreBarMenuButtonWidth - scoreBarRightGap;
  const float scoreBarMenuButtonLeft = scoreBarValueLeft + scoreBarValueWidth + scoreBarSeparatorGap;
  gameLayout->addChild(loScoreBarCaption, scoreBarLeftGap, scoreBarTopGap, scoreBarCaptionWidth, scoreBarHeight);
  gameLayout->addChild(loScoreBarValue, scoreBarValueLeft, scoreBarTopGap, scoreBarValueWidth, scoreBarHeight);
  gameLayout->addChild(loScoreBarMenuButton, scoreBarMenuButtonLeft, scoreBarTopGap, scoreBarMenuButtonWidth, scoreBarHeight);

  const float glassLeft = 0.5f * (backgroundWidth - glassWidth);
  gameLayout->addChild(loGlass, glassLeft, glassTop, glassWidth, glassHeight);

  const float leftSidePanelsLeft = glassLeft - infoPanelsHorzGapFromGlass - infoPanelsWidth;
  const float rightSidePanelsLeft = glassLeft + glassWidth + infoPanelsHorzGapFromGlass;
  gameLayout->addChild(loHoldPanelCaption, leftSidePanelsLeft, holdNextCaptionsTop, infoPanelsWidth, infoPanelsCaptionHeight);
  gameLayout->addChild(loNextPanelCaption, rightSidePanelsLeft, holdNextCaptionsTop, infoPanelsWidth, infoPanelsCaptionHeight);
  gameLayout->addChild(loHoldPanel, leftSidePanelsLeft, holdNextPanelsTop, infoPanelsWidth, holdNextPanelsHeight);
  gameLayout->addChild(loNextPanel, rightSidePanelsLeft, holdNextPanelsTop, infoPanelsWidth, holdNextPanelsHeight);
  gameLayout->addChild(loLevelPanelCaption, leftSidePanelsLeft, levelCaptionTop, infoPanelsWidth, infoPanelsCaptionHeight);
  gameLayout->addChild(loLevelPanel, leftSidePanelsLeft, levelPanelTop, infoPanelsWidth, levelGoalPanelsHeight);
  gameLayout->addChild(loGoalPanelCaption, leftSidePanelsLeft, goalCaptionTop, infoPanelsWidth, infoPanelsCaptionHeight);
  gameLayout->addChild(loGoalPanel, leftSidePanelsLeft, goalPanelTop, infoPanelsWidth, levelGoalPanelsHeight);

  mainMenuLayout->addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::mainMenu.rowCount; i++)
    mainMenuLayout->addRow(i ? menuRowInterval : menuTop, menuRowHeight);

  inGameMenuLayout->addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::inGameMenu.rowCount; i++)
    inGameMenuLayout->addRow(i ? menuRowInterval : menuTop, menuRowHeight);

  quitConfirmationMenuLayout->addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::quitConfirmationMenu.rowCount; i++)
    quitConfirmationMenuLayout->addRow(i ? menuRowInterval : menuTop, menuRowHeight);

  restartConfirmationMenuLayout->addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::restartConfirmationMenu.rowCount; i++)
    restartConfirmationMenuLayout->addRow(i ? menuRowInterval : menuTop, menuRowHeight);

  exitToMainConfirmationMenuLayout->addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::exitToMainConfirmationMenu.rowCount; i++)
    exitToMainConfirmationMenuLayout->addRow(i ? menuRowInterval : menuTop, menuRowHeight);

  saveSettingsMenuLayout->addColumn(0.0f, menuRowWidth);

  for (int i = 0; i < InterfaceLogic::settingsLogic.saveConfirmationMenu.rowCount; i++)
    saveSettingsMenuLayout->addRow(i ? menuRowInterval : menuTop, menuRowHeight);

  
  LayoutObject * settingsWindowLayout = settingsLayout->addChild(loSettingsWindow, 0.0f, settingsTop, settingsWidth, settingsHeight);
  settingsWindowLayout->addChild(loSettingsTitle, settingsTitleLeft, settingsTitleTop, 0.0, settingsTitleHeight);
  const float settingsTitleShadowLeft = settingsTitleLeft + settingsTitleShadowDX;
  const float settingsTitleShadowTop = settingsTitleTop + settingsTitleShadowDY;
  settingsWindowLayout->addChild(loSettingsTitleShadow, settingsTitleShadowLeft, settingsTitleShadowTop, 0.0, settingsTitleHeight);

  const float settingsPanelWidth = settingsWidth - settingsPanelLeft - settingsPanelRightGap;
  const float settingsPanelHeight = settingsHeight - settingsPanelTop - settingsPanelBottomGap;
  LayoutObject * settingsPanelLayout = settingsWindowLayout->addChild(loSettingsPanel, settingsPanelLeft, settingsPanelTop, settingsPanelWidth, settingsPanelHeight);

  float settingsPanelChildTop = settingsPanelTitleTopGap;
  const float settingsPanelRowLeft = settingsPanelRowHorzGap + settingsPanelBorderWidth;
  const float settingsPanelRowWidth = settingsPanelWidth - 2.0f * (settingsPanelRowHorzGap + settingsPanelBorderWidth);
  settingsPanelLayout->addChild(loVolumeTitle, settingsPanelTitleLeft, settingsPanelChildTop, settingsPanelRowWidth, settingsPanelTitleHeight);

  settingsPanelChildTop += settingsPanelTitleHeight + settingsPanelTitleBottomGap;
  LayoutObject * soundProgressBarLayout = settingsPanelLayout->addChild(loSoundVolume, settingsPanelRowLeft, settingsPanelChildTop, settingsPanelRowWidth, settingsPanelRowHeight);
  settingsPanelChildTop += settingsPanelRowHeight + settingsPanelRowVertGap;
  LayoutObject * musicProgressBarLayout = settingsPanelLayout->addChild(loMusicVolume, settingsPanelRowLeft, settingsPanelChildTop, settingsPanelRowWidth, settingsPanelRowHeight);

  const float settingsProgressBarLeft = settingsPanelRowWidth - settingsProgressBarOuterGap - settingsProgressBarWidth;
  const float settingsProgressBarHeight = settingsPanelRowHeight - 2.0f * settingsProgressBarOuterGap;
  soundProgressBarLayout->addChild(loSoundProgressBar, settingsProgressBarLeft, settingsProgressBarOuterGap, settingsProgressBarWidth, settingsProgressBarHeight);
  musicProgressBarLayout->addChild(loMusicProgressBar, settingsProgressBarLeft, settingsProgressBarOuterGap, settingsProgressBarWidth, settingsProgressBarHeight);

  settingsPanelChildTop += settingsPanelRowHeight + settingsPanelTitleTopGap;
  settingsPanelLayout->addChild(loKeyBindingTitle, settingsPanelTitleLeft, settingsPanelChildTop, settingsPanelRowWidth, settingsPanelTitleHeight);
  
  settingsPanelChildTop += settingsPanelTitleHeight + settingsPanelTitleBottomGap;
  const float keyBindingGridHeight = Binding::ACTION_COUNT * settingsPanelRowHeight + (Binding::ACTION_COUNT - 1) * settingsPanelRowVertGap;
  LayoutObject * keyBindingGridLayout = settingsPanelLayout->addChild(loKeyBindingGrid, settingsPanelRowLeft, settingsPanelChildTop, settingsPanelRowWidth, keyBindingGridHeight);
  keyBindingGridLayout->addColumn(0.0f, settingsKeyBindingCaptionWidth);
  keyBindingGridLayout->addColumn(settingsPanelRowHorzGap, settingsPanelRowWidth - settingsKeyBindingCaptionWidth - settingsPanelRowHorzGap);

  for (Binding::Action action = Binding::doNothing + 1; action < Binding::ACTION_COUNT; action++)
  {
    const float topGap = (action == Binding::doNothing + 1) ? 0.0f : settingsPanelRowVertGap;
    keyBindingGridLayout->addRow(topGap, settingsPanelRowHeight);
  }

  const float settingsBackButtonWidth = settingsBackShevronSize + 2.0f * settingsBackShevronStep;
  const float settingsBackButtonLeft = settingsWidth - settingsBackShevronRightGap - settingsBackButtonWidth;
  const float settingsBackButtonTop = 0.5f * (settingsWindowLayout->height + settingsPanelLayout->top + settingsPanelLayout->height - settingsBackShevronSize);
  LayoutObject * settingsBackButtonLayout = settingsWindowLayout->addChild(loSettingsBackButton, settingsBackButtonLeft, settingsBackButtonTop, settingsBackButtonWidth, settingsBackShevronSize);
  settingsBackButtonLayout->addRow(0.0f, settingsBackShevronSize);
  const float columnShift = settingsBackShevronStep - settingsBackShevronSize;

  for (int i = 0; i < 3; i++)
    settingsBackButtonLayout->addColumn(i ? columnShift : 0.0f, settingsBackShevronSize);

}

void Layout::loadValue(rapidjson::Value & source, const char * name, float * result)
{
  assert(source.HasMember(name));

  if (source.HasMember(name))
    *result = float(source[name].GetDouble());
}

