#include "Globals.h"
#include "Background.h"
#include "Crosy.h"

Background::Background() :
  gameBkVert(GL_VERTEX_SHADER),
  gameBkFrag(GL_FRAGMENT_SHADER),
  glassBkVert(GL_VERTEX_SHADER),
  glassBkFrag(GL_FRAGMENT_SHADER),
  vertexBufferId(0),
  uvBufferId(0)
{
}

Background::~Background()
{
  glDeleteBuffers(1, &vertexBufferId);
  glDeleteBuffers(1, &uvBufferId);
}

void Background::init()
{
  GLfloat vertexBufferData[6 * 2] =
  {
    0.0f, 0.0f,
    0.0f, -1.0f,
    1.0f, 0.0f,
    1.0f, -1.0f,
  };

  const float xReps = 50.0f;
  const float yReps = xReps / Globals::gameBkSize.x * Globals::gameBkSize.y * 0.85f;
  const float texScaleCorrection = 0.56f; // tile deformation to get power of two texture size
  const float xVal = xReps * texScaleCorrection;
  const float yVal = yReps;

  GLfloat uvBufferData[6 * 2] =
  {
    0.0f, 0.0f,
    0.0f, yVal,
    xVal, 0.0f,
    xVal, yVal,
  };

  if (!vertexBufferId) {
    glGenBuffers(1, &vertexBufferId);
    assert(!checkGlErrors());
  }

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  assert(!checkGlErrors());

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);
  assert(!checkGlErrors());

  if (!uvBufferId) {
    glGenBuffers(1, &uvBufferId);
    assert(!checkGlErrors());
  }

  glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
  assert(!checkGlErrors());

  glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferData), uvBufferData, GL_STATIC_DRAW);
  assert(!checkGlErrors());

  gameBkVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in vec2 vertexUV;"
    "uniform vec2 pos;"
    "uniform vec2 size;"
    "out vec2 pixPos;"
    "out vec2 uv;"

    "void main()"
    "{"
    "  gl_Position = vec4(vertexPos * size + pos, 0, 1);"
    "  uv = vertexUV;"
    "  pixPos = (vertexPos + vec2(-0.5, 0.5)) * 2.0;"
    "}");

  gameBkFrag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2DArray tex;"
    "uniform float texIndex;"
    "in vec2 uv;"
    "in vec2 pixPos;"
    "out vec4 color;"

    "void main()"
    "{"
    "  float mul = 0.9 - 0.3 * length(pixPos) + 0.3 * pixPos.y;"
    "  color = vec4(mul * texture(tex, vec3(uv, texIndex)).rgb, 1);"
    "}");

  gameBkProg.attachShader(gameBkVert);    
  gameBkProg.attachShader(gameBkFrag);    
  gameBkProg.link();                
  gameBkProg.use();
  gameBkProg.setUniform("tex", 0);
  gameBkProg.setUniform("texIndex", float(Globals::backgroundTexIndex));
  gameBkProg.setUniform("pos", Globals::gameBkPos);
  gameBkProg.setUniform("size", Globals::gameBkSize);

  glassBkVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "uniform vec2 pos;"
    "uniform vec2 size;"
    "out vec2 pixPos;"

    "void main()"
    "{"
    "  gl_Position = vec4(vertexPos * size + pos, 0, 1);"
    "  pixPos = (vertexPos + vec2(-0.5, 0.5)) * 2.0;"
    "}");

  glassBkFrag.compileFromString(
    "#version 330 core\n"
    "in vec2 pixPos;"
    "out vec4 color;"

    "float rand(vec2 co){"
    "  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);"
    "}"

    "void main()"
    "{"
    "  float mul = pow(0.1 * length(pixPos) + 0.1 * abs(pixPos.x), 2) + rand(pixPos) / 200;"
    "  color = vec4(vec3(0.1, 0.2, 0.3) + vec3(1.0, 2.0, 1.6) * mul, 1.0);"
    "}");

  glassBkProg.attachShader(glassBkVert);
  glassBkProg.attachShader(glassBkFrag);
  glassBkProg.link();
  glassBkProg.use();
  glassBkProg.setUniform("pos", Globals::glassPos);
  glassBkProg.setUniform("size", Globals::glassSize);
}

void Background::draw() const
{
  gameBkProg.use();

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  assert(!checkGlErrors());

  glEnableVertexAttribArray(0);
  assert(!checkGlErrors());

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
  assert(!checkGlErrors());

  glEnableVertexAttribArray(1);
  assert(!checkGlErrors());

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());


  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  assert(!checkGlErrors());

  glDisableVertexAttribArray(0);
  assert(!checkGlErrors());

  glDisableVertexAttribArray(1);
  assert(!checkGlErrors());


  glassBkProg.use();

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  assert(!checkGlErrors());

  glEnableVertexAttribArray(0);
  assert(!checkGlErrors());

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!checkGlErrors());

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  assert(!checkGlErrors());

  glDisableVertexAttribArray(0);
  assert(!checkGlErrors());

}
