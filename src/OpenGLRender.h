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
  enum OriginType { otLeft, otRight, otCenter};

  struct Glyph
  {
    int texIndex;
    FT_UInt glyphIndex;
    FT_Glyph_Metrics metrics;
  };

  GameLogic & gameLogic;
  InterfaceLogic & interfaceLogic;
  std::vector<float> vertexBuffer;
  FT_Library ftLibrary;
  FT_Face ftFace;
  typedef std::map<int, Glyph> GlyphSizeMap;
  typedef std::map<char, GlyphSizeMap> GlyphCharMap;
  std::map<char, std::map<int, Glyph> > glyphs;
  GLuint vaoId;
  GLuint vertexBufferId;
  Program figureProg;
  Shader figureVert;
  Shader figureFrag;
  int vertexCount;

  void loadGlyph(char ch, int size);
  void addVertex(const glm::vec2 & xy, const glm::vec2 & uv, int texIndex, const glm::vec3 & color, float alpha);
  void clearVertices();
  void sendToDevice();
  void rebuildMesh();
  void drawMesh();
  void buildRect(float left, float top, float right, float bottom, glm::vec3 color, float alpha);
  void buildBackground();
  void buildFigureBackgroundsMesh();
  void buidGlassShadow();
  void buidGlassBlocks();
  void biuldGlassGlow();
  void buildFigureBlocks();
  void buildFigureGlow();
  void buildDropTrails();
  void buildRowFlashes();
  void buildSidePanel(float x, float y, float width, float height, glm::vec3 color);
  void buildMenu();
  void buildTextMesh(const char * str, int fontSize, float scale, glm::vec3 color, float alpha, float originX, float originY, OriginType originType = otLeft);
};
