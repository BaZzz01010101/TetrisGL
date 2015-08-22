#pragma once
#include "glall.h"
#include "Program.h"
#include "Shader.h"

class Figure
{
public:
  enum Color { clRed = 0, clOrange, clYellow, clGreen, clCyan, clBlue, clPurple };

private:
  Color color;
  std::vector<float> figureVertexBufferData;
  std::vector<float> figureUVBufferData;
  std::vector<float> shadowVertexBufferData;
  std::vector<float> shadowUVBufferData;
  std::vector<float> glowVertexBufferData;
  std::vector<float> glowUVBufferData;
  std::vector<float> glowAlphaBufferData;
  GLuint figureVertexBufferId;
  GLuint figureUVBufferId;
  GLuint shadowVertexBufferId;
  GLuint shadowUVBufferId;
  GLuint glowVertexBufferId;
  GLuint glowUVBufferId;
  GLuint glowAlphaBufferId;
  int figureVertCount;
  int shadowVertCount;
  int glowVertCount;
  static GLuint textureId;
  static Program figureProg;
  static Shader figureVert;
  static Shader figureFrag;
  static Program glowProg;
  static Shader glowVert;
  static Shader glowFrag;
  static glm::vec2 screenScale;

  void buildMeshes();
  void clearVertexBuffersData();
  void addFigureVertex(float x, float y, float dx, float dy, float dv);
  void addShadowVertex(float x, float y, float dx, float dy, float u, float v);
  void addGlowVertex(float x, float y, float dx, float dy, float alpha);

public:
  int dim;
  std::vector<int> data;

  Figure(int dim, Color color, const char * data);
  ~Figure();

  static void init();
  static void setScreenScale(const glm::vec2 & scale);
  void rotate(int halfPiAngles);
  void drawFigure(float x, float y, float sqSize);
  void drawShadow(float x, float y, float sqSize);
  void drawGlow(float x, float y, float sqSize);
};

