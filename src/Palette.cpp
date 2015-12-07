#include "static_headers.h"

#include "Palette.h"
#include "Crosy.h"

glm::vec3 Palette::gameBackgroundOuter = glm::vec3(0.05f, 0.1f, 0.2f);
glm::vec3 Palette::gameBackgroundInner = glm::vec3(0.3f, 0.6f, 1.0f);
glm::vec3 Palette::glassBackgroundMin = glm::vec3(0.1f, 0.1f, 0.2f);
glm::vec3 Palette::glassBackgroundMax = glm::vec3(0.3f, 0.3f, 0.5f);
glm::vec3 Palette::scoreBarBackground = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 Palette::scoreBarMenuButtonBackground = glm::vec3(0.3f, 0.6f, 0.9f);
glm::vec3 Palette::scoreBarText = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::scoreBarMenuButtonText = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::holdCaptionText = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::holdEmptyPanel = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3 Palette::nextCaptionText = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::nextEmptyPanel = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3 Palette::levelCaptionText = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::levelPanelBackground = glm::vec3(0.2f, 0.45f, 0.8f);
glm::vec3 Palette::levelPanelText = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::goalCaptionText = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::goalPanelBackground = glm::vec3(0.2f, 0.45f, 0.8f);
glm::vec3 Palette::goalPanelText = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Palette::figureRed = glm::vec3(1.0f, 0.05f, 0.05f);
glm::vec3 Palette::figureOrange = glm::vec3(1.0f, 0.35f, 0.0f);
glm::vec3 Palette::figureYellow = glm::vec3(0.85f, 0.6f, 0.0f);
glm::vec3 Palette::figureGreen = glm::vec3(0.05f, 0.55f, 0.1f);
glm::vec3 Palette::figureCyan = glm::vec3(0.05f, 0.6f, 1.0f);
glm::vec3 Palette::figureBlue = glm::vec3(0.1f, 0.2f, 0.9f);
glm::vec3 Palette::figurePurple = glm::vec3(0.4f, 0.1f, 0.8f);

glm::vec3 Palette::figureColorArray[Figure::Color::COLOR_COUNT] =
{
  figureRed,
  figureOrange,
  figureYellow,
  figureGreen,
  figureCyan,
  figureBlue,
  figurePurple,
};

glm::vec3 Palette::figureShadow = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 Palette::menuNormalRowBackgroundTop = glm::vec3(0.4f, 0.7f, 1.2f);
glm::vec3 Palette::menuNormalRowBackgroundBottom = glm::vec3(0.04f, 0.07f, 0.12f);
glm::vec3 Palette::menuSelectedRowBackgroundTop = glm::vec3(2.0f, 1.9f, 1.2f);
glm::vec3 Palette::menuSelectedRowBackgroundBottom = glm::vec3(0.2f, 0.19f, 0.12f);
glm::vec3 Palette::menuNormalRowText = glm::vec3(0.4f, 0.7f, 1.0f);
glm::vec3 Palette::menuSelectedRowText = glm::vec3(1.0f, 0.9f, 0.4f);
glm::vec3 Palette::menuNormalRowGlow = glm::vec3(0.4f, 0.7f, 1.2f);
glm::vec3 Palette::menuSelectedRowGlow = glm::vec3(2.0f, 1.9f, 1.2);

float Palette::figureGlowOuterBright = 0.01f;
float Palette::figureGlowInnerBright = 0.25f;
float Palette::deletedRowFlashBright = 1.0f;
float Palette::deletedRowRaysBright = 1.0f;
float Palette::deletedRowShineBright = 1.0f;
float Palette::glassBackgroundInnerBright = 0.25f;

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
  loadValue(doc, "GlassBackgroundMin", &glassBackgroundMin);
  loadValue(doc, "GlassBackgroundMax", &glassBackgroundMax);
  loadValue(doc, "ScoreBarBackground", &scoreBarBackground);
  loadValue(doc, "ScoreBarMenuButtonBackground", &scoreBarMenuButtonBackground);
  loadValue(doc, "ScoreBarText", &scoreBarText);
  loadValue(doc, "ScoreBarMenuButtonText", &scoreBarMenuButtonText);
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
  loadValue(doc, "FigureRed", &figureColorArray[Figure::Color::clRed]);
  loadValue(doc, "FigureOrange", &figureColorArray[Figure::Color::clOrange]);
  loadValue(doc, "FigureYellow", &figureColorArray[Figure::Color::clYellow]);
  loadValue(doc, "FigureGreen", &figureColorArray[Figure::Color::clGreen]);
  loadValue(doc, "FigureCyan", &figureColorArray[Figure::Color::clCyan]);
  loadValue(doc, "FigureBlue", &figureColorArray[Figure::Color::clBlue]);
  loadValue(doc, "FigurePurple", &figureColorArray[Figure::Color::clPurple]);
  loadValue(doc, "FigureShadow", &figureShadow);
  loadValue(doc, "MenuNormalRowBackgroundTop", &menuNormalRowBackgroundTop);
  loadValue(doc, "MenuNormalRowBackgroundBottom", &menuNormalRowBackgroundBottom);
  loadValue(doc, "MenuSelectedRowBackgroundTop", &menuSelectedRowBackgroundTop);
  loadValue(doc, "MenuSelectedRowBackgroundBottom", &menuSelectedRowBackgroundBottom);
  loadValue(doc, "MenuNormalRowText", &menuNormalRowText);
  loadValue(doc, "MenuSelectedRowText", &menuSelectedRowText);
  loadValue(doc, "MenuNormalRowGlow", &menuNormalRowGlow);
  loadValue(doc, "MenuSelectedRowGlow", &menuSelectedRowGlow);
  loadValue(doc, "FigureGlowOuterBright", &figureGlowOuterBright);
  loadValue(doc, "FigureGlowInnerBright", &figureGlowInnerBright);
  loadValue(doc, "DeletedRowFlashBright", &deletedRowFlashBright);
  loadValue(doc, "DeletedRowRaysBright", &deletedRowRaysBright);
  loadValue(doc, "DeletedRowShineBright", &deletedRowShineBright);
  loadValue(doc, "GlassBackgroundInnerBright", &glassBackgroundInnerBright);

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

