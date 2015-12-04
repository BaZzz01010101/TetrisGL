#include "static_headers.h"

#include "Layout.h"
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

LayoutObject Layout::gameLayout("GameBackground", NULL, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);

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
    rapidjson::Value & game = doc["Game"];

    loadValue(game, "BackgroundTileWidth", &gameBkTileWidth.val);
    loadValue(game, "BackgroundTileHeight", &gameBkTileHeight.val);

    loadValue(game, "ScoreBarTopGap", &scoreBarTopGap);
    loadValue(game, "ScoreBarLeftGap", &scoreBarLeftGap);
    loadValue(game, "ScoreBarRightGap", &scoreBarRightGap);
    loadValue(game, "ScoreBarSeparatorGap", &scoreBarSeparatorGap);
    loadValue(game, "ScoreBarHeight", &scoreBarHeight);
    loadValue(game, "ScoreBarCaptionWidth", &scoreBarCaptionWidth);
    loadValue(game, "ScoreBarMenuButtonWidth", &scoreBarMenuButtonWidth);

    loadValue(game, "GlassWidth", &glassWidth);
    loadValue(game, "GlassHeight", &glassHeight);
    loadValue(game, "GlassTop", &glassTop);

    loadValue(game, "InfoPanelsHorzGapFromGlass", &infoPanelsHorzGapFromGlass);
    loadValue(game, "InfoPanelsWidth", &infoPanelsWidth);
    loadValue(game, "HoldNextCaptionsTop", &holdNextCaptionsTop);
    loadValue(game, "HoldNextPanelsTop", &holdNextPanelsTop);
    loadValue(game, "LevelCaptionTop", &levelCaptionTop);
    loadValue(game, "LevelPanelTop", &levelPanelTop);
    loadValue(game, "GoalCaptionTop", &goalCaptionTop);
    loadValue(game, "GoalPanelTop", &goalPanelTop);
    loadValue(game, "InfoPanelsCaptionHeight", &infoPanelsCaptionHeight);
    loadValue(game, "HoldNextPanelsHeight", &holdNextPanelsHeight);
    loadValue(game, "LevelGoalPanelsHeight", &levelGoalPanelsHeight);

    holdNextFigureScale = 0.25f * 0.75f * glm::min(holdNextPanelsHeight, infoPanelsWidth);

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

}

void Layout::loadValue(rapidjson::Value & source, const char * name, float * result)
{
  assert(source.HasMember(name));
  if (source.HasMember(name))
    *result = float(source[name].GetDouble());
}

