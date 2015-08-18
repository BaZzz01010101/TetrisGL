#include "Background.h"
#include "Crosy.h"

Background::Background() :
  vert(GL_VERTEX_SHADER),
  frag(GL_FRAGMENT_SHADER),
  vertexBufferId(0),
  uvBufferId(0),
  textureId(0),
  proj(1),
  aspect(0.66f)
{
}

Background::~Background()
{
}

void Background::init()
{
  GLfloat vertexBufferData[6 * 3] =
  {
    -aspect, -1.0f, 0.0f,
    -aspect, 1.0f, 0.0f,
    aspect, -1.0f, 0.0f,
    aspect, 1.0f, 0.0f,
  };

  const float xReps = 40;
  const float yReps = xReps / aspect;
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
    "layout(location = 0) in vec3 vertexPos;\n"
    "layout(location = 1) in vec2 vertexUV;\n"
    "uniform mat3 proj;\n"
    "out vec3 pos;\n"
    "out vec2 uv;\n"

    "void main()\n"
    "{\n"
    "  gl_Position = vec4(proj * vertexPos, 1);\n"
    "  uv = vertexUV;\n"
    "  pos = vertexPos;\n"
    "}\n");
  assert(!vert.isError());

  frag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2D tex;\n"
    "in vec2 uv;\n"
    "in vec3 pos;\n"
    "out vec3 color;\n"

    "float rand(vec2 co) {\n"
    "  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"
    "}\n"

    "void main()\n"
    "{\n"
    "  color = (1 - (0.4 * length(pos) - 0.3 * pos.y)) * texture(tex, uv).rgb;\n"
    "}\n");
  assert(!frag.isError());

  prog.attachShader(vert);    
  assert(!prog.isError());

  prog.attachShader(frag);    
  assert(!prog.isError());

  prog.link();                
  assert(!prog.isError());

  prog.use();
  assert(!prog.isError());

  std::string texPath = Crosy::getExePath() + "\\textures\\background.jpg";
  textureId = SOIL_load_OGL_texture(texPath.c_str(), 0, 0, SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);
  assert(textureId);
  assert(!glGetError());

  GLuint texId = glGetUniformLocation(prog.getId(), "tex");
  assert(!glGetError());

  glUniform1i(texId, 0);
  assert(!glGetError());
}

void Background::setProjMatrix(const glm::mat3x3 & m)
{
  proj = m;

  prog.use();
  assert(!prog.isError());

  GLuint mvId = glGetUniformLocation(prog.getId(), "proj");
  assert(!glGetError());

  glUniformMatrix3fv(mvId, 1, GL_FALSE, &(proj)[0][0]);
  assert(!glGetError());
}

void Background::draw() const
{
  prog.use();
  assert(!prog.isError());

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  assert(!glGetError());

  glEnableVertexAttribArray(0);
  assert(!glGetError());

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
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
