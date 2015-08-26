#include "Globals.h"
#include "Background.h"
#include "Crosy.h"

Background::Background() :
  vert(GL_VERTEX_SHADER),
  frag(GL_FRAGMENT_SHADER),
  vertexBufferId(0),
  uvBufferId(0),
  aspect(0.66f)
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
    -aspect, -1.0f,
    -aspect, 1.0f,
    aspect, -1.0f,
    aspect, 1.0f,
  };

  const float xReps = 50.0f;
  const float yReps = xReps / aspect * 0.85f;
  const float texScaleCorrection = 0.56f;
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

  vert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in vec2 vertexUV;"
    "uniform vec2 screen;"
    "out vec2 pos;"
    "out vec2 uv;"

    "void main()"
    "{"
    "  gl_Position = vec4(screen * vertexPos, 0, 1);"
    "  uv = vertexUV;"
    "  pos = vertexPos / screen;"
    "}");
  assert(!vert.isError());

  frag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2DArray tex;"
    "uniform float texIndex;"
    "in vec2 uv;"
    "in vec2 pos;"
    "out vec4 color;"

    "void main()"
    "{"
    "  float mul = 0.9 - 0.3 * length(pos) + 0.3 * pos.y;"
    "  color = vec4(mul * texture(tex, vec3(uv, texIndex)).rgb, 1);"
    "}");
  assert(!frag.isError());

  prog.attachShader(vert);    
  assert(!prog.isError());

  prog.attachShader(frag);    
  assert(!prog.isError());

  prog.link();                
  assert(!prog.isError());

  prog.use();
  assert(!prog.isError());

  prog.setUniform("tex", 0);
  assert(!prog.isError());

  prog.setUniform("texIndex", float(Globals::backgroundTexIndex));
  assert(!prog.isError());

  //prog.setUniform("texChunkPos", Globals::atlasChunkSize * 7.0f + Globals::atlasBorderSize, Globals::atlasChunkSize * 1.0f + Globals::atlasBorderSize);
  //assert(!prog.isError());

  //prog.setUniform("texChunkSize", Globals::atlasChunkSize - 2.0f * Globals::atlasBorderSize);
  //assert(!prog.isError());
}

void Background::setScreen(const glm::vec2 & screen)
{
  prog.use();
  assert(!prog.isError());

  prog.setUniform("screen", screen);
  assert(!prog.isError());
}

void Background::draw() const
{
  prog.use();
  assert(!prog.isError());

  //glBindTexture(GL_TEXTURE_2D, Globals::mainTextureAtlasId);
  //assert(!checkGlErrors());

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
}
