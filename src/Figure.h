#pragma once
#include "glall.h"
#include "Program.h"
#include "Shader.h"

class Figure
{
public:
  enum Color { clRed = 0, clOrange, clYellow, clGreen, clCyan, clBlue, clPurple };
  enum Type { typeI = 0, typeJ, typeL, typeO, typeS, typeT, typeZ, TYPE_COUNT };
  enum Rotation { rotRight, rotLeft, rot180, rot270};

private:
  static Program figureProg;
  static Shader figureVert;
  static Shader figureFrag;
  static Program glowProg;
  static Shader glowVert;
  static Shader glowFrag;
  static std::vector<float> figureVertexBufferData;
  static std::vector<float> figureUVBufferData;
  static std::vector<float> shadowVertexBufferData;
  static std::vector<float> shadowUVBufferData;
  static std::vector<float> glowVertexBufferData;
  static std::vector<float> glowAlphaBufferData;

  GLuint figureVertexBufferId;
  GLuint figureUVWBufferId;
  GLuint shadowVertexBufferId;
  GLuint shadowUVBufferId;
  GLuint glowVertexBufferId;
  GLuint glowAlphaBufferId;
  int figureVertCount;
  int shadowVertCount;
  int glowVertCount;

  void clearBuffersData();
  void addFigureVertex(float sqx, float sqy, float u, float v, int blockArrayIndex);
  void addShadowVertex(float x, float y, float u, float v);
  void addGlowVertex(float x, float y, float alpha);

public:
  int col;
  int row;
  int dim;
  Color color;
  std::vector<int> data;

  Figure();
  Figure(int dim, Color color, const char * cdata);
  ~Figure();

  static void init();
  static void setScale(float scale);

  void rotate(Rotation rot);
  void buildMeshes();

  void drawFigure(float x, float y);
  void drawShadow(float x, float y);
  void drawGlow(float x, float y);
};

