#include "Background.h"
#include "Crosy.h"

Background::Background() :
  vert(GL_VERTEX_SHADER),
  frag(GL_FRAGMENT_SHADER),
  vertexBufferId(0),
  uvBufferId(0),
  textureId(0),
  screenScale(1.0f),
  aspect(0.66f)
{
}

Background::~Background()
{
  glDeleteBuffers(1, &vertexBufferId);
  glDeleteBuffers(1, &uvBufferId);
  glDeleteTextures(1, &textureId);
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

  const float xReps = 50;
  const float yReps = xReps / aspect * 0.85;
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

  if (!uvBufferId) {
    glGenBuffers(1, &uvBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferData), uvBufferData, GL_STATIC_DRAW);
  assert(!glGetError());

  if (!vertexBufferId) {
    glGenBuffers(1, &vertexBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);
  assert(!glGetError());

  vert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in vec2 vertexUV;"
    "uniform vec2 screenScale;"
    "out vec2 pos;"
    "out vec2 uv;"

    "void main()"
    "{"
    "  gl_Position = vec4(screenScale * vertexPos, 0, 1);"
    "  uv = vertexUV;"
    "  pos = vertexPos / screenScale;"
    "}");
  assert(!vert.isError());

  frag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2D tex;"
    "in vec2 uv;"
    "in vec2 pos;"
    "out vec4 color;"

    "void main()"
    "{"
    "  float mul = 0.9 - 0.3 * length(pos) + 0.3 * pos.y;"
    "  color = vec4(mul * texture(tex, uv).rgb, 1);"
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

  std::string texPath = Crosy::getExePath() + "\\textures\\background.png";
  textureId = SOIL_load_OGL_texture(texPath.c_str(), 0, 0, SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);
  assert(textureId);
  assert(!glGetError());

  GLuint texId = glGetUniformLocation(prog.getId(), "tex");
  assert(!glGetError());

  glUniform1i(texId, 0);
  assert(!glGetError());
}

void Background::setScreenScale(const glm::vec2 & scale)
{
  screenScale = scale;

  prog.use();
  assert(!prog.isError());

  GLuint projId = glGetUniformLocation(prog.getId(), "screenScale");
  assert(!glGetError());

  glUniform2fv(projId, 1, &screenScale.x);
  assert(!glGetError());
}

void Background::draw() const
{
  glBindTexture(GL_TEXTURE_2D, textureId);
  assert(!glGetError());

  prog.use();
  assert(!prog.isError());

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  assert(!glGetError());

  glEnableVertexAttribArray(0);
  assert(!glGetError());

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
  assert(!glGetError());

  glEnableVertexAttribArray(1);
  assert(!glGetError());

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  assert(!glGetError());

  glDisableVertexAttribArray(0);
  assert(!glGetError());

  glDisableVertexAttribArray(1);
  assert(!glGetError());
}
