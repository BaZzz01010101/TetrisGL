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
float Layout::scoreBarTopGap = 0.01f;
float Layout::scoreBarLeftGap = 0.01f;
float Layout::scoreBarRightGap = 0.01f;
float Layout::scoreBarSeparatorGap = 0.01f;
float Layout::scoreBarHeight = 0.08f;
float Layout::scoreBarCaptionWidth = 0.45f;
float Layout::scoreBarMenuButtonWidth = 0.25f;

LayoutObject Layout::gameLayout("GameBackground", NULL, backgroundLeft, backgroundTop, backgroundWidth, backgroundHeight);

void Layout::load(const char * name)
{
  rapidjson::Document doc;
  std::string fileName = Crosy::getExePath() + "/layouts/" + name + ".lt";
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

    assert(game.HasMember("BackgroundTileWidth"));
    if (game.HasMember("BackgroundTileWidth"))
      gameBkTileWidth = float(game["BackgroundTileWidth"].GetDouble());

    assert(game.HasMember("BackgroundTileHeight"));
    if (game.HasMember("BackgroundTileHeight"))
      gameBkTileHeight = float(game["BackgroundTileHeight"].GetDouble());

    assert(game.HasMember("ScoreBarTopGap"));
    if (game.HasMember("ScoreBarTopGap"))
      scoreBarTopGap = float(game["ScoreBarTopGap"].GetDouble());

    assert(game.HasMember("ScoreBarLeftGap"));
    if (game.HasMember("ScoreBarLeftGap"))
      scoreBarLeftGap = float(game["ScoreBarLeftGap"].GetDouble());

    assert(game.HasMember("ScoreBarRightGap"));
    if (game.HasMember("ScoreBarRightGap"))
      scoreBarRightGap = float(game["ScoreBarRightGap"].GetDouble());

    assert(game.HasMember("ScoreBarSeparatorGap"));
    if (game.HasMember("ScoreBarSeparatorGap"))
      scoreBarSeparatorGap = float(game["ScoreBarSeparatorGap"].GetDouble());

    assert(game.HasMember("ScoreBarHeight"));
    if (game.HasMember("ScoreBarHeight"))
      scoreBarHeight = float(game["ScoreBarHeight"].GetDouble());

    assert(game.HasMember("ScoreBarCaptionWidth"));
    if (game.HasMember("ScoreBarCaptionWidth"))
      scoreBarCaptionWidth = float(game["ScoreBarCaptionWidth"].GetDouble());

    assert(game.HasMember("ScoreBarMenuButtonWidth"));
    if (game.HasMember("ScoreBarMenuButtonWidth"))
      scoreBarMenuButtonWidth = float(game["ScoreBarMenuButtonWidth"].GetDouble());


  }

  gameLayout.clear();
  const float scoreBarValueLeft = scoreBarLeftGap + scoreBarCaptionWidth + scoreBarSeparatorGap;
  const float scoreBarValueWidth = backgroundWidth - scoreBarLeftGap - scoreBarCaptionWidth - scoreBarSeparatorGap - scoreBarSeparatorGap - scoreBarMenuButtonWidth - scoreBarRightGap;
  const float scoreBarMenuButtonLeft = scoreBarValueLeft + scoreBarValueWidth + scoreBarSeparatorGap;

  gameLayout.addChild("ScoreBarCaption", scoreBarLeftGap, scoreBarTopGap, scoreBarCaptionWidth, scoreBarHeight);
  gameLayout.addChild("ScoreBarValue", scoreBarValueLeft, scoreBarTopGap, scoreBarValueWidth, scoreBarHeight);
  gameLayout.addChild("ScoreBarMenuButton", scoreBarMenuButtonLeft, scoreBarTopGap, scoreBarMenuButtonWidth, scoreBarHeight);
}
