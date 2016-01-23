#pragma once
#include "Program.h"
#include "Shader.h"
#include "Cell.h"
#include "Figure.h"
#include "GameLogic.h"
#include "InterfaceLogic.h"
#include "LayoutObject.h"
#include "SDFF.h"

class OpenGLRender
{
public:
  bool showWireframe;

  OpenGLRender();
  ~OpenGLRender();

  void init(int width, int height);
  void resize(int width, int height);
  void update();

private:

  enum TextureIndex 
  {
    FIRST_TEX_INDEX = 0,
    tiEmpty = 0, 
    tiFlame = 1,
    tiFigureCellNormal = 2, 
    tiFigureCellBold = 3, 
    tiFigureShadow = 4, 
    tiHoldBackground = 5, 
    tiNextBackground = 6, 
    tiDropTrail = 7, 
    tiDropSparkle = 8, 
    tiRowFlash = 9, 
    tiRowShineRay = 10, 
    tiRowShineLight = 11, 
    tiGuiPanelGlow = 12, 
    tiLine = 13, 
    tiLevelGoalBackground = 14, 
    tiBackShevron = 15, 
    TEX_INDEX_COUNT 
  };

  enum HorzAllign 
  { 
    haLeft, 
    haRight, 
    haCenter 
  };

  enum VertAllign 
  { 
    vaTop, 
    vaBottom, 
    vaCenter 
  };

  struct Vertex
  {
    glm::vec2 xy;
    glm::vec2 uv;
    glm::vec4 rgba;
  };

  struct TextVertex
  {
    glm::vec2 xy;
    glm::vec2 uv;
    glm::vec4 rgba;
    float falloff;
  };

  const float edgeBlurWidth;
  const int atlasSpriteSize = 64;
  int width;
  int height;
  float pxSize;
  SDFF_Font font;
  GLuint vaoId = 0;
  GLuint bkVertexBufferId = 0;
  GLuint atlasVertexBufferId = 0;
  GLuint textVertexBufferId = 0;
  GLuint bkTextureId = 0;
  GLuint atlasTextureId = 0;
  GLuint fontTextureId = 0;
  Program commonProg;
  Shader commonVert;
  Shader commonFrag;
  Program fontProg;
  Shader fontVert;
  Shader fontFrag;
  glm::vec2 texPos[TEX_INDEX_COUNT];
  std::vector<Vertex> bkVertexBuffer;
  std::vector<Vertex> atlasVertexBuffer;
  std::vector<TextVertex> textVertexBuffer;

  void clearVertices();
  void drawMesh();
  void addBkVertex(const glm::vec2 & xy, const glm::vec2 & uv, const glm::vec3 & color,
                   float alpha);
  void addAtlasVertex(const glm::vec2 & xy, const glm::vec2 & uv, int texIndex, 
                      const glm::vec3 & color, float alpha);
  void addTextVertex(const glm::vec2 & xy, const glm::vec2 & uv, float falloffSize, 
                     const glm::vec3 & color, float alpha);
  // TODO : overload build functions to give Rect argument
  void buildRect(float left, float top, float width, float height, const glm::vec3 & color, 
                 float alpha);
  void buildSmoothRect(float left, float top, float width, float height, float blur, 
                       const glm::vec3 & color, float alpha);
  void buildTexturedRect(float left, float top, float width, float height, int texIndex, 
                         const glm::vec3 & color, float alpha);
  void buildVertGradientRect(float left, float top, float width, float height, 
                             const glm::vec3 & topColor, float topAlpha, 
                             const glm::vec3 & bottomColor, float bottomAlpha);
  void buildLine(float x0, float y0, float x1, float y1, float width, const glm::vec3 & color, 
                 float alpha);
  void buildFrameRect(float left, float top, float width, float height, float borderWidth, 
                      const glm::vec3 & borderColor, float borderAlpha);
  void buildProgressBar(float left, float top, float width, float height, 
                        const glm::vec3 & bkColor, const glm::vec3 & borderColor, 
                        const glm::vec3 & barColor, float alpha, float progress);
  void buildSideBar(float left, float top, float width, float height, float cornerSize, 
                    float glowWidth, const glm::vec3 & topColor, const glm::vec3 & bottomColor, 
                    const glm::vec3 & glowColor);
  void buildWindow(float left, float top, float width, float height, float cornerSize, 
                   float glowWidth, const glm::vec3 & topColor, const glm::vec3 & bottomColor, 
                   const glm::vec3 & glowColor);
  float buildTextMesh(float left, float top, float width, float height, const char * str, 
                      float size, const glm::vec3 & color, float alpha, float blur, 
                      HorzAllign horzAllign = haLeft, VertAllign vertAllign = vaTop);
  void buildMenu(MenuLogic * menuLogic, LayoutObject * menuLayout);
  void buildSettingsWindow();
  void buildLeaderboardWindow();
  void buildBackground();
  void buidFieldShadows();
  void buidFieldBlocks();
  void biuldFieldGlow();
  void buildFigureBlocks();
  void buildFigureGlow();
  void buildDropTrails();
  void buildRowFlashes();
  void buildDropPredictor();
  void buildCountdown();
  void buildLevelUp();

  void updateGameLayer();
  void updateSettingsLayer();
  void updateLeaderboardLayer();
  void updateMenuLayer();
};
