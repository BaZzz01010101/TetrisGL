#include "MainMesh.h"
#include "Globals.h"

MainMesh::MainMesh() :
  shaderVert(GL_VERTEX_SHADER),
  shaderFrag(GL_FRAGMENT_SHADER)
{
}


MainMesh::~MainMesh()
{
}

void MainMesh::init()
{
  glGenBuffers(1, &vertexBufferId);
  assert(!checkGlErrors());
  const int initBufferSize = 16384;
  vertexBuffer.reserve(initBufferSize);

  shaderVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in vec3 vertexUVW;"
    "uniform float scale;"
    "uniform vec2 pos;"
    "out vec3 uvw;"

    "void main()"
    "{"
    "  gl_Position = vec4(vertexPos * scale + pos, 0, 1);"
    "  uvw = vertexUVW;"
    "}");

  shaderFrag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2DArray tex;"
    "in vec3 uvw;"
    "out vec4 out_color;"

    "void main()"
    "{"
    "  out_color = texture(tex, uvw).rgba;"
    "}");

  shaderProg.attachShader(shaderVert);
  shaderProg.attachShader(shaderFrag);
  shaderProg.link();
  shaderProg.use();
  shaderProg.setUniform("tex", 0);
}

void MainMesh::clear()
{
  vertexBuffer.clear();
  vertexCount = 0;
}

void MainMesh::addVertex(float x, float y, float u, float v)
{
  vertexBuffer.push_back(x);
  vertexBuffer.push_back(y);
  vertexBuffer.push_back(u);
  vertexBuffer.push_back(v);
  vertexBuffer.push_back(float(Globals::shadowTexIndex));
  vertexCount++;
}

void MainMesh::send()
{
  if (vertexCount)
  {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    assert(!checkGlErrors());

    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), vertexBuffer.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());
  }
}

void MainMesh::draw(float x, float y, float scale)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assert(!checkGlErrors());

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  assert(!checkGlErrors());

  shaderProg.use();
  shaderProg.setUniform("pos", glm::vec2(x, y));
  shaderProg.setUniform("scale", scale);
  glEnableVertexAttribArray(0);
  assert(!checkGlErrors());
  glEnableVertexAttribArray(1);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  assert(!checkGlErrors());
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
  assert(!checkGlErrors());

  glDrawArrays(GL_TRIANGLES, 0, vertexCount);
  assert(!checkGlErrors());

  glDisableVertexAttribArray(0);
  assert(!checkGlErrors());
  glDisableVertexAttribArray(1);
  assert(!checkGlErrors());
}

