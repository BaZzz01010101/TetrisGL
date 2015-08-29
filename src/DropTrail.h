#pragma once
#include "glall.h"
#include "Program.h"
#include "Shader.h"
#include "Figure.h"

class DropTrail
{
private:
  static Program trailProg;
  static Shader trailVert;
  static Shader trailFrag;
  static Program sparklesProg;
  static Shader sparklesVert;
  static Shader sparklesGeom;
  static Shader sparklesFrag;

  static GLuint trailVertexBufferId;
  static GLuint trailUVWBufferId;
  static GLuint sparklesPointBufferId;
  static GLuint sparklesAlphaBufferId;
  static GLuint sparklesSpeedBufferId;
  static int trailVertCount;
  static int sparklesVertCount;

  static uint64_t freq;
  static const float trailEffectTime;
  static const float sparklesEffectTime;

  uint64_t createCounter;

public:
  glm::vec2 pos;
  float height;
  Figure::Color color;

  DropTrail();
  ~DropTrail();

  static void init();
  static void setScale(float scale);

  bool isExpired();
  void draw();
};

