#pragma once
#include "Model.h"
#include "Program.h"
#include "Shader.h"

class MainMesh
{
private:
  enum OriginType { otLeft, otRight, otCenter};

  struct Glyph
  {
    int texIndex;
    FT_UInt glyphIndex;
    FT_Glyph_Metrics metrics;
  };

  Model & model;
  std::vector<float> vertexBuffer;
  FT_Library ftLibrary;
  FT_Face ftFace;
  typedef std::map<int, Glyph> GlyphSizeMap;
  typedef std::map<char, GlyphSizeMap> GlyphCharMap;
  std::map<char, std::map<int, Glyph> > glyphs;
  GLuint vertexBufferId;
  Program figureProg;
  Shader figureVert;
  Shader figureFrag;
  int vertexCount;

  void loadGlyph(char ch, int size);
  Cell * getGlassCell(int x, int y);
  Cell * getFigureCell(Figure & figure, int x, int y);
  void addVertex(const glm::vec2 & xy, const glm::vec2 & uv, int texIndex, const glm::vec3 & color, float alpha);
  void clear();
  void sendToDevice();
  void buildBackground();
  void buildFigureBackgroundsMesh();
  void buidGlassShadow();
  void buidGlassBlocks();
  void biuldGlassGlow();
  void buildFigureBlocks();
  void buildFigureGlow();
  void buildDropTrails();
  void buildRowFlashes();
  void buildSidePanel(float x, float y, const char * text, glm::vec3 textColor, bool highlighted);
  void buildTextMesh(const char * str, int fontSize, float scale, glm::vec3 color, float originX, float originY, OriginType originType = otLeft);

public:
  MainMesh(Model & model);
  ~MainMesh();

  void init();
  void fillDepthBuffer();
  void rebuild();
  void draw();
};
