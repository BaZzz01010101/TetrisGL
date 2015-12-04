#pragma once
#include "Program.h"
#include "Shader.h"
#include "Cell.h"
#include "Figure.h"
#include "GameLogic.h"
#include "InterfaceLogic.h"

class OpenGLRender
{
public:
  bool showWireframe;

  OpenGLRender(GameLogic & gameLogic, InterfaceLogic & interfaceLogic);
  ~OpenGLRender();

  void init(int width, int height);
  void resize(int width, int height);
  void update();

private:
  enum HorzAllign { haLeft, haRight, haCenter };
  enum VertAllign { vaTop, vaBottom, vaCenter };
  int width;
  int height;

  struct Glyph
  {
    int texIndex;
    FT_UInt glyphIndex;
    FT_Glyph_Metrics metrics;
  };

  GameLogic & gameLogic;
  InterfaceLogic & interfaceLogic;
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
  void buildRect(float x, float y, float width, float height, glm::vec3 color, float alpha);
  void buildTexturedRect(float x, float y, float width, float height, int texIndex, glm::vec3 color, float alpha);
  void buildVertGradientRect(float x, float y, float width, float height, glm::vec3 topColor, float topAlpha, glm::vec3 bottomColor, float bottomAlpha);
  void buildLine(float x0, float y0, float x1, float y1, float width, glm::vec3 color, float alpha);
  void buildFrameRect(float x, float y, float width, float height, float borderWidth, glm::vec3 borderColor, float borderAlpha);
  void buildProgressBar(float x, float y, float width, float height, glm::vec3 bkColor, glm::vec3 fgColor, float alpha, float progress);
  void buildBackground();
  void buildFigureBackgroundsMesh();
  void buidGlassShadow();
  void buidGlassBlocks();
  void biuldGlassGlow();
  void buildFigureBlocks();
  void buildFigureGlow();
  void buildDropTrails();
  void buildRowFlashes();
  void buildSidePanel(float x, float y, float width, float height, float cornerSize, glm::vec3 topColor, glm::vec3 bottomColor, glm::vec3 glowColor, float glowWidth);
  void buildMenu();
  void buildTextMesh(float left, float top, float width, float height, const char * str, int fontSize, float scale, glm::vec3 color, float alpha, HorzAllign horzAllign = haLeft, VertAllign vertAllign = vaTop);
  void buildSettings();
};
