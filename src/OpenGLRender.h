#pragma once
#include "Program.h"
#include "Shader.h"
#include "Cell.h"
#include "Figure.h"
#include "GameLogic.h"
#include "InterfaceLogic.h"
#include "LayoutObject.h"

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
  enum HorzAllign { haLeft, haRight, haCenter };
  enum VertAllign { vaTop, vaBottom, vaCenter };
  const float edgeBlurWidth;
  int width;
  int height;

  struct Glyph
  {
    int texIndex;
    FT_UInt glyphIndex;
    FT_Glyph_Metrics metrics;
  };

  FT_Library ftLibrary;
  FT_Face ftFace;
  typedef std::map<int, Glyph> GlyphSizeMap;
  typedef std::map<char, GlyphSizeMap> GlyphCharMap;
  GlyphCharMap glyphs;
  GLuint vaoId;
  GLuint vertexBufferId;
  Program figureProg;
  Shader figureVert;
  Shader figureFrag;

  struct Vertex
  {
    glm::vec2 xy;
    glm::vec3 uvw;
    glm::vec4 rgba;
  };

  std::vector<Vertex> vertexBuffer;

  void loadGlyph(char ch, int size);
  void addVertex(const glm::vec2 & xy, const glm::vec2 & uv, int texIndex, const glm::vec3 & color, float alpha);
  void clearVertices();
  void sendToDevice();
  void rebuildMesh();
  void drawMesh();
  void buildRect(float left, float top, float width, float height, const glm::vec3 & color, float alpha);
  void buildSmoothRect(float left, float top, float width, float height, float blur, const glm::vec3 & color, float alpha);
  void buildTexturedRect(float left, float top, float width, float height, int texIndex, const glm::vec3 & color, float alpha);
  void buildVertGradientRect(float left, float top, float width, float height, const glm::vec3 & topColor, float topAlpha, const glm::vec3 & bottomColor, float bottomAlpha);
  void buildLine(float x0, float y0, float x1, float y1, float width, const glm::vec3 & color, float alpha);
  void buildFrameRect(float left, float top, float width, float height, float borderWidth, const glm::vec3 & borderColor, float borderAlpha);
  void buildProgressBar(float left, float top, float width, float height, const glm::vec3 & bkColor, const glm::vec3 & borderColor, const glm::vec3 & barColor, float alpha, float progress);
  void buildBackground();
  void buildFigureBackgroundsMesh();
  void buidGlassShadow();
  void buidGlassBlocks();
  void biuldGlassGlow();
  void buildFigureBlocks();
  void buildFigureGlow();
  void buildDropTrails();
  void buildRowFlashes();
  void buildSideBar(float left, float top, float width, float height, float cornerSize, float glowWidth, const glm::vec3 & topColor, const glm::vec3 & bottomColor, const glm::vec3 & glowColor);
  void buildWindow(float left, float top, float width, float height, float cornerSize, float glowWidth, const glm::vec3 & topColor, const glm::vec3 & bottomColor, const glm::vec3 & glowColor);
  void buildMenu();
  void buildMenu(MenuLogic * menuLogic, LayoutObject * menuLayout);
  float buildTextMesh(float left, float top, float width, float height, const char * str, int fontSize, float scale, const glm::vec3 & color, float alpha, HorzAllign horzAllign = haLeft, VertAllign vertAllign = vaTop);
  void buildSettings();
  void buildLeaderboard();
  void buildCountdown();
  void buildLevelUp();
};
