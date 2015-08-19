#include "Figure.h"
#include "Crosy.h"

GLuint Figure::textureId = 0;
Program Figure::figureProg;
Shader Figure::figureVert(GL_VERTEX_SHADER);
Shader Figure::figureFrag(GL_FRAGMENT_SHADER);
//Program Figure::glowProg;
//Shader Figure::glowVert(GL_VERTEX_SHADER);
//Shader Figure::glowFrag(GL_FRAGMENT_SHADER);
glm::vec2 Figure::screenScale(1.0f);
const glm::vec3 Figure::clRed(0.8, 0.05, 0.05);
const glm::vec3 Figure::clOrange(0.9, 0.4, 0.1);
const glm::vec3 Figure::clYellow(0.9, 0.7, 0.1);
const glm::vec3 Figure::clGreen(0.2, 0.6, 0.2);
const glm::vec3 Figure::clCyan(0.2, 0.5, 0.7);
const glm::vec3 Figure::clBlue(0.1, 0.2, 0.6);
const glm::vec3 Figure::clPurple(0.4, 0.2, 0.6);

// data - is a string of '0' and '1'
Figure::Figure(int dim, glm::vec3 color, const char * data) :
  dim(dim),
  color(color),
  figureVertexBufferId(0),
  figureUVBufferId(0),
  borderMaskBufferId(0),
  vertCount(0)
{
  assert(strlen(data) == dim * dim);

  for (const char * ptr = data, * end = data + dim * dim; ptr < end; ptr++)
    this->data.push_back(*ptr - '0');

  buildMeshes();
}

Figure::~Figure()
{
  glDeleteBuffers(1, &figureVertexBufferId);
  glDeleteBuffers(1, &figureUVBufferId);
  glDeleteBuffers(1, &borderMaskBufferId);
}

void Figure::init()
{
  figureVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;\n"
    "layout(location = 1) in vec2 vertexUV;\n"
    "layout(location = 2) in int borderMask;\n"
    "uniform vec2 screenScale;\n"
    "uniform float sqSize;\n"
    "uniform vec2 pos;\n"
    "out vec2 uv;\n"
    "flat out int bord;\n"

    "void main()\n"
    "{\n"
    "  gl_Position = vec4(screenScale * (vertexPos * sqSize + pos), 1, 1);\n"
    "  uv = vertexUV;\n"
    "  bord = borderMask;\n"
    "}\n");
  assert(!figureVert.isError());

  figureFrag.compileFromString(
    "#version 330 core\n"
    "uniform vec3 color;\n"
    "uniform sampler2D tex;\n"
    "in vec2 uv;\n"
    "flat in int bord;\n"
    "out vec3 out_color;\n"

    "void main()\n"
    "{\n"
    " if((bool(bord & 1<<0) && uv.x < 0.02) || \n"
    "    (bool(bord & 1<<1) && uv.x < 0.02  && uv.y < 0.02) || \n"
    "    (bool(bord & 1<<2) && uv.y < 0.02) || \n"
    "    (bool(bord & 1<<3) && uv.x > 0.98  && uv.y < 0.02) || \n"
    "    (bool(bord & 1<<4) && uv.x > 0.98) || \n"
    "    (bool(bord & 1<<5) && uv.x > 0.98  && uv.y > 0.98) || \n"
    "    (bool(bord & 1<<6) && uv.y > 0.98) || \n"
    "    (bool(bord & 1<<7) && uv.x < 0.02  && uv.y > 0.98))\n"
    "  out_color = vec3(0, 0, 0);\n"
    " else if((bool(bord & 1<<0) && uv.x < 0.1) || \n"
    "    (bool(bord & 1<<1) && uv.x < 0.1  && uv.y < 0.1) || \n"
    "    (bool(bord & 1<<2) && uv.y < 0.1) || \n"
    "    (bool(bord & 1<<3) && uv.x > 0.9  && uv.y < 0.1) || \n"
    "    (bool(bord & 1<<4) && uv.x > 0.9) || \n"
    "    (bool(bord & 1<<5) && uv.x > 0.9  && uv.y > 0.9) || \n"
    "    (bool(bord & 1<<6) && uv.y > 0.9) || \n"
    "    (bool(bord & 1<<7) && uv.x < 0.1  && uv.y > 0.9))\n"
    "  out_color = vec3(1, 1, 1);\n"
    "else\n"
    "  out_color = color + 0.4 * texture(tex, uv).rgb;\n"
    "}\n");
  assert(!figureFrag.isError());

  figureProg.attachShader(figureVert);
  assert(!figureProg.isError());

  figureProg.attachShader(figureFrag);
  assert(!figureProg.isError());

  figureProg.link();
  assert(!figureProg.isError());

  figureProg.use();
  assert(!figureProg.isError());

  std::string texPath = Crosy::getExePath() + "\\textures\\block.jpg";
  textureId = SOIL_load_OGL_texture(texPath.c_str(), 0, 0, SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS);
  assert(textureId);
  assert(!glGetError());

  GLuint texId = glGetUniformLocation(figureProg.getId(), "tex");
  assert(!glGetError());

  glUniform1i(texId, 0);
  assert(!glGetError());

  //glowVert.compileFromString(
  //  "#version 330 core\n"
  //  "layout(location = 0) in vec2 vertexPos;\n"
  //  "uniform mat2 screenScale;\n"
  //  "uniform float sqSize;\n"
  //  "uniform vec2 pos;\n"

  //  "void main()\n"
  //  "{\n"
  //  "  gl_Position = vec4(screenScale * vertexPos, 1, 1);\n"
  //  "}\n");
  //assert(!glowVert.isError());

  //glowFrag.compileFromString(
  //  "#version 330 core\n"
  //  "out vec3 color;\n"

  //  "void main()\n"
  //  "{\n"
  //  "  color = vec3(1, 1, 1);\n"
  //  "}\n");
  //assert(!glowFrag.isError());

  //glowProg.attachShader(glowVert);
  //assert(!glowProg.isError());

  //glowProg.attachShader(glowFrag);
  //assert(!glowProg.isError());

  //glowProg.link();
  //assert(!glowProg.isError());
}

void Figure::buildMeshes()
{
  figureVertexBufferData.clear();
  figureUVBufferData.clear();
  borderMaskBufferData.clear();

  vertCount = 0;
  int index = 0;

  for (int iy = 0; iy < dim; iy++)
  for (int ix = 0; ix < dim; ix++)
  {
    float fx = float(ix);
    float fy = float(-iy);

    if (data[index])
    {
      int borderMask = 0;

      borderMask |= (!ix || !data[index - 1]);
      borderMask |= (!ix || !iy || !data[index - dim - 1]) << 1;
      borderMask |= (!iy || !data[index - dim]) << 2;
      borderMask |= (!iy || ix == dim - 1 || !data[index - dim + 1]) << 3;
      borderMask |= (ix == dim - 1 || !data[index + 1]) << 4;
      borderMask |= (ix == dim - 1 || iy == dim - 1 || !data[index + dim + 1]) << 5;
      borderMask |= (iy == dim - 1 || !data[index + dim]) << 6;
      borderMask |= (!ix || iy == dim - 1 || !data[index + dim - 1]) << 7;

      figureVertexBufferData.push_back(fx);
      figureVertexBufferData.push_back(fy);
      figureUVBufferData.push_back(0.0f);
      figureUVBufferData.push_back(0.0f);
      borderMaskBufferData.push_back(borderMask);

      figureVertexBufferData.push_back(fx + 1.0f);
      figureVertexBufferData.push_back(fy);
      figureUVBufferData.push_back(1.0f);
      figureUVBufferData.push_back(0.0f);
      borderMaskBufferData.push_back(borderMask);

      figureVertexBufferData.push_back(fx + 1.0f);
      figureVertexBufferData.push_back(fy - 1.0f);
      figureUVBufferData.push_back(1.0f);
      figureUVBufferData.push_back(1.0f);
      borderMaskBufferData.push_back(borderMask);

      figureVertexBufferData.push_back(fx);
      figureVertexBufferData.push_back(fy);
      figureUVBufferData.push_back(0.0f);
      figureUVBufferData.push_back(0.0f);
      borderMaskBufferData.push_back(borderMask);

      figureVertexBufferData.push_back(fx + 1.0f);
      figureVertexBufferData.push_back(fy - 1.0f);
      figureUVBufferData.push_back(1.0f);
      figureUVBufferData.push_back(1.0f);
      borderMaskBufferData.push_back(borderMask);

      figureVertexBufferData.push_back(fx);
      figureVertexBufferData.push_back(fy - 1.0f);
      figureUVBufferData.push_back(0.0f);
      figureUVBufferData.push_back(1.0f);
      borderMaskBufferData.push_back(borderMask);

      vertCount += 6;
    }

    index++;
  }

  if (!figureVertexBufferId) {
    glGenBuffers(1, &figureVertexBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, figureVertexBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, figureVertexBufferData.size() * sizeof(float), &figureVertexBufferData.front(), GL_STATIC_DRAW);
  assert(!glGetError());


  if (!figureUVBufferId) {
    glGenBuffers(1, &figureUVBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, figureUVBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, figureUVBufferData.size() * sizeof(float), &figureUVBufferData.front(), GL_STATIC_DRAW);
  assert(!glGetError());


  if (!borderMaskBufferId) {
    glGenBuffers(1, &borderMaskBufferId);
    assert(!glGetError());
  }

  glBindBuffer(GL_ARRAY_BUFFER, borderMaskBufferId);
  assert(!glGetError());

  glBufferData(GL_ARRAY_BUFFER, borderMaskBufferData.size() * sizeof(int), &borderMaskBufferData.front(), GL_STATIC_DRAW);
  assert(!glGetError());

}

void Figure::rotate(int halfPiAngles)
{
}

void Figure::setScreenScale(const glm::vec2 & scale)
{
  screenScale = scale;

  figureProg.use();
  assert(!figureProg.isError());

  GLuint figureProjId = glGetUniformLocation(figureProg.getId(), "screenScale");
  assert(!glGetError());

  glUniform2fv(figureProjId, 1, &screenScale.x);
  assert(!glGetError());
}

void Figure::draw(float x, float y, float sqSize)
{
  Figure::figureProg.use();
  assert(!Figure::figureProg.isError());

  glBindTexture(GL_TEXTURE_2D, textureId);
  assert(!glGetError());


  glBindBuffer(GL_ARRAY_BUFFER, figureVertexBufferId);
  assert(!glGetError());

  glEnableVertexAttribArray(0);
  assert(!glGetError());

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());


  glBindBuffer(GL_ARRAY_BUFFER, figureUVBufferId);
  assert(!glGetError());

  glEnableVertexAttribArray(1);
  assert(!glGetError());

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  assert(!glGetError());


  glBindBuffer(GL_ARRAY_BUFFER, borderMaskBufferId);
  assert(!glGetError());

  glEnableVertexAttribArray(2);
  assert(!glGetError());

  glVertexAttribIPointer(2, 1, GL_INT, 0, (void*)0);
  assert(!glGetError());


  GLuint posId = glGetUniformLocation(figureProg.getId(), "pos");
  assert(!glGetError());

  glUniform2f(posId, x, y);
  assert(!glGetError());

  GLuint texId = glGetUniformLocation(figureProg.getId(), "color");
  assert(!glGetError());

  glUniform3fv(texId, 1, &color.r);
  assert(!glGetError());

  GLuint sizeId = glGetUniformLocation(figureProg.getId(), "sqSize");
  assert(!glGetError());

  glUniform1f(sizeId, sqSize);
  assert(!glGetError());


  glDrawArrays(GL_TRIANGLES, 0, vertCount);
  assert(!glGetError());


  glDisableVertexAttribArray(0);
  assert(!glGetError());

  glDisableVertexAttribArray(1);
  assert(!glGetError());

  glDisableVertexAttribArray(2);
  assert(!glGetError());
}
