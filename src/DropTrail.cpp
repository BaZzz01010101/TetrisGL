#include "DropTrail.h"
#include "Globals.h"
#include "Crosy.h"

GLuint DropTrail::trailVertexBufferId = 0;
GLuint DropTrail::trailUVWBufferId = 0;
GLuint DropTrail::sparklesPointBufferId = 0;
GLuint DropTrail::sparklesAlphaBufferId = 0;
GLuint DropTrail::sparklesSpeedBufferId = 0;
uint64_t DropTrail::freq = Crosy::getPerformanceFrequency();
const float DropTrail::trailEffectTime = 0.5f;
const float DropTrail::sparklesEffectTime = 1.0f;

int DropTrail::trailVertCount = 0;
int DropTrail::sparklesVertCount = 0;

Program DropTrail::trailProg;
Shader DropTrail::trailVert(GL_VERTEX_SHADER);
Shader DropTrail::trailFrag(GL_FRAGMENT_SHADER);
Program DropTrail::sparklesProg;
Shader DropTrail::sparklesVert(GL_VERTEX_SHADER);
Shader DropTrail::sparklesGeom(GL_GEOMETRY_SHADER);
Shader DropTrail::sparklesFrag(GL_FRAGMENT_SHADER);

DropTrail::DropTrail() :
  createCounter(Crosy::getPerformanceCounter()),
  height(0.0f),
  pos(glm::vec2(0.0f, 0.0f)),
  color(Globals::Color::clRed)
{
}

DropTrail::~DropTrail()
{
}

void DropTrail::init()
{
  trailVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in vec3 vertexUVW;"
    "uniform float scale;"
    "uniform float height;"
    "uniform vec2 pos;"
    "uniform float color;"
    "uniform float time;"
    "flat out float alpha;"
    "out vec3 uvw;"

    "void main()"
    "{"
    "  gl_Position = vec4((vertexPos  - vec2(0.22, 0.11)) * scale * vec2(2.0, height * (1.0 - time)) + pos - vec2(0.0, height * time * scale), 0, 1);"
    "  uvw = vertexUVW + vec3(0.0, 0.0, color);"
    "  alpha = 1.0 - time;"
    "}");

  trailFrag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2DArray tex;"
    "in vec3 uvw;"
    "flat in float alpha;"
    "out vec4 out_color;"

    "void main()"
    "{"
    "  out_color = texture(tex, uvw).rgba * vec4(1.0, 1.0, 1.0, alpha);"
    "}");

  trailProg.attachShader(trailVert);
  trailProg.attachShader(trailFrag);
  trailProg.link();
  trailProg.use();
  trailProg.setUniform("tex", 0);

  GLfloat trailVertexBufferData[4 * 2] =
  {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, -1.0f,
    1.0f, -1.0f,
  };

  GLfloat trailUVWBufferData[4 * 3] =
  {
    0.0f, 0.0f, float(Globals::dropTrailsTexIndex),
    1.0f, 0.0f, float(Globals::dropTrailsTexIndex),
    0.0f, 1.0f, float(Globals::dropTrailsTexIndex),
    1.0f, 1.0f, float(Globals::dropTrailsTexIndex),
  };

  if (!trailVertexBufferId) {
    glGenBuffers(1, &trailVertexBufferId);
    assert(!checkGlErrors());
  }

  glBindBuffer(GL_ARRAY_BUFFER, trailVertexBufferId);
  assert(!checkGlErrors());

  glBufferData(GL_ARRAY_BUFFER, sizeof(trailVertexBufferData), trailVertexBufferData, GL_STATIC_DRAW);
  assert(!checkGlErrors());

  if (!trailUVWBufferId) {
    glGenBuffers(1, &trailUVWBufferId);
    assert(!checkGlErrors());
  }

  glBindBuffer(GL_ARRAY_BUFFER, trailUVWBufferId);
  assert(!checkGlErrors());

  glBufferData(GL_ARRAY_BUFFER, sizeof(trailUVWBufferData), trailUVWBufferData, GL_STATIC_DRAW);
  assert(!checkGlErrors());

  sparklesVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 posArray;"
    "layout(location = 1) in float alphaArray;"
    "layout(location = 2) in float speedArray;"
    "uniform float scale;"
    "uniform vec2 pos;"
    "uniform float height;"
    "uniform float time;"
    "out vec2 pointPos;"
    "out float pointAlpha;"

    "void main()"
    "{"
    "  vec2 pt = fract(posArray * (10.0 + pos * 10.0));"
    "  pointPos = vec2(pt.x, -pt.y * height * 0.5 + speedArray * time - height * 0.5) * scale + pos;"
    "  pointAlpha = (1.0 - time) * alphaArray * pt.y;"
    "}");

  sparklesGeom.compileFromString(
    "#version 330 core\n"
    "layout(points) in;"
    "layout(triangle_strip, max_vertices = 4) out;"
    "in vec2 pointPos[];"
    "in float pointAlpha[];"
    "const float sparkleRadius = 0.003;"
    "out vec2 vertexUV;"
    "flat out float vertexAlpha;"

    "void main()"
    "{"
    "  vec2 pt = pointPos[0];"

    "  gl_Position = vec4(pt.x - sparkleRadius, pt.y - sparkleRadius, 0.0, 1.0);"
    "  vertexUV = vec2(0.0, 0.0);"
    "  vertexAlpha = pointAlpha[0];"
    "  EmitVertex();"

    "  gl_Position = vec4(pt.x + sparkleRadius, pt.y - sparkleRadius, 0.0, 1.0);"
    "  vertexUV = vec2(1.0, 0.0);"
    "  vertexAlpha = pointAlpha[0];"
    "  EmitVertex();"

    "  gl_Position = vec4(pt.x - sparkleRadius, pt.y + sparkleRadius, 0.0, 1.0);"
    "  vertexUV = vec2(0.0, 1.0);"
    "  vertexAlpha = pointAlpha[0];"
    "  EmitVertex();"

    "  gl_Position = vec4(pt.x + sparkleRadius, pt.y + sparkleRadius, 0.0, 1.0);"
    "  vertexUV = vec2(1.0, 1.0);"
    "  vertexAlpha = pointAlpha[0];"
    "  EmitVertex();"

    "  EndPrimitive();"
    "}");

  sparklesFrag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2DArray tex;"
    "uniform float texlayer;"
    "in vec2 vertexUV;"
    "flat in float vertexAlpha;"
    "out vec4 out_color;"

    "void main()"
    "{"
    "  out_color = texture(tex, vec3(vertexUV, texlayer)).rgba + vertexAlpha * 0.5;"
    "  out_color.a *= vertexAlpha;"
    "}");

  sparklesProg.attachShader(sparklesVert);
  sparklesProg.attachShader(sparklesGeom);
  sparklesProg.attachShader(sparklesFrag);
  sparklesProg.link();
  sparklesProg.use();
  sparklesProg.setUniform("tex", 0);

  const int sparklesCount = 128;
  std::vector<float> sparklesPointBufferData;
  std::vector<float> sparklesAlphaBufferData;
  std::vector<float> sparklesSpeedBufferData;

  for (int i = 0; i < sparklesCount; i++)
  {
    sparklesPointBufferData.push_back(float(rand()) / RAND_MAX);
    sparklesPointBufferData.push_back(float(rand()) / RAND_MAX);
    sparklesAlphaBufferData.push_back(0.5f + float(rand()) / RAND_MAX);
    sparklesSpeedBufferData.push_back(3.0f + 2.0f * float(rand()) / RAND_MAX);
  }

  sparklesVertCount = sparklesCount;

  glGenBuffers(1, &sparklesPointBufferId);
  assert(!checkGlErrors());
  glGenBuffers(1, &sparklesAlphaBufferId);
  assert(!checkGlErrors());
  glGenBuffers(1, &sparklesSpeedBufferId);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, sparklesPointBufferId);
  assert(!checkGlErrors());
  glBufferData(GL_ARRAY_BUFFER, sparklesPointBufferData.size() * sizeof(float), &sparklesPointBufferData.front(), GL_STATIC_DRAW);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, sparklesAlphaBufferId);
  assert(!checkGlErrors());
  glBufferData(GL_ARRAY_BUFFER, sparklesAlphaBufferData.size() * sizeof(float), &sparklesAlphaBufferData.front(), GL_STATIC_DRAW);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, sparklesSpeedBufferId);
  assert(!checkGlErrors());
  glBufferData(GL_ARRAY_BUFFER, sparklesSpeedBufferData.size() * sizeof(float), &sparklesSpeedBufferData.front(), GL_STATIC_DRAW);
  assert(!checkGlErrors());
}

void DropTrail::setScale(float scale)
{
  trailProg.use();
  trailProg.setUniform("scale", scale);
  sparklesProg.use();
  sparklesProg.setUniform("scale", scale);
}

bool DropTrail::isExpired()
{
  return float(double(Crosy::getPerformanceCounter() - createCounter) / double(freq)) > glm::max<float>(trailEffectTime , sparklesEffectTime);
}

void DropTrail::draw()
{
  float timePassed = float(double(Crosy::getPerformanceCounter() - createCounter) / double(freq));

  if (timePassed < trailEffectTime)
  {
    trailProg.use();
    trailProg.setUniform("pos", pos);
    trailProg.setUniform("color", float(color));
    trailProg.setUniform("height", height);
    trailProg.setUniform("time", glm::clamp<float>(timePassed / trailEffectTime, 0.0f, 1.0f));
    glEnableVertexAttribArray(0);
    assert(!checkGlErrors());
    glEnableVertexAttribArray(1);
    assert(!checkGlErrors());


    glBindBuffer(GL_ARRAY_BUFFER, trailVertexBufferId);
    assert(!checkGlErrors());
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    assert(!checkGlErrors());

    glBindBuffer(GL_ARRAY_BUFFER, trailUVWBufferId);
    assert(!checkGlErrors());
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    assert(!checkGlErrors());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    assert(!checkGlErrors());


    glDisableVertexAttribArray(0);
    assert(!checkGlErrors());
    glDisableVertexAttribArray(1);
    assert(!checkGlErrors());
  }

  if (timePassed < sparklesEffectTime)
  {
    sparklesProg.use();
    sparklesProg.setUniform("pos", pos);
    sparklesProg.setUniform("texlayer", float(Globals::dropSparklesTexIndex + color));
    sparklesProg.setUniform("height", height);
    sparklesProg.setUniform("time", glm::clamp<float>(timePassed / sparklesEffectTime, 0.0f, 1.0f));

    glEnableVertexAttribArray(0);
    assert(!checkGlErrors());
    glEnableVertexAttribArray(1);
    assert(!checkGlErrors());
    glEnableVertexAttribArray(2);
    assert(!checkGlErrors());

    glBindBuffer(GL_ARRAY_BUFFER, sparklesPointBufferId);
    assert(!checkGlErrors());
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    assert(!checkGlErrors());

    glBindBuffer(GL_ARRAY_BUFFER, sparklesAlphaBufferId);
    assert(!checkGlErrors());
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    assert(!checkGlErrors());

    glBindBuffer(GL_ARRAY_BUFFER, sparklesSpeedBufferId);
    assert(!checkGlErrors());
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    assert(!checkGlErrors());


    glDrawArrays(GL_POINTS, 0, sparklesVertCount);
    assert(!checkGlErrors());


    glDisableVertexAttribArray(0);
    assert(!checkGlErrors());
    glDisableVertexAttribArray(1);
    assert(!checkGlErrors());
    glDisableVertexAttribArray(2);
    assert(!checkGlErrors());
  }

  //if (timePassed > sparklesEffectTime)
  //  createCounter = Crosy::getPerformanceCounter();
}
