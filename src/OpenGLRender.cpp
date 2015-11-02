#include "static_headers.h"

#include "OpenGLRender.h"
#include "Globals.h"
#include "DropSparkle.h"
#include "DropTrail.h"
#include "Crosy.h"

OpenGLRender::OpenGLRender(GameLogic & gameLogic) :
  figureVert(GL_VERTEX_SHADER),
  figureFrag(GL_FRAGMENT_SHADER),
  showWireframe(false),
  gameLogic(gameLogic)
{
}

OpenGLRender::~OpenGLRender()
{
}

void OpenGLRender::init(int width, int height)
{
  glGenVertexArrays(1, &vaoId);
  assert(!checkGlErrors());

  glBindVertexArray(vaoId);
  assert(!checkGlErrors());

  glGenBuffers(1, &vertexBufferId);
  assert(!checkGlErrors());

  const int initBufferSize = 20480;
  vertexBuffer.reserve(initBufferSize);

  figureVert.compileFromString(
    "#version 330 core\n"
    "layout(location = 0) in vec2 vertexPos;"
    "layout(location = 1) in vec3 vertexUVW;"
    "layout(location = 2) in vec4 vertexRGBA;"
    "uniform float scale;"
    "uniform vec2 pos;"
    "out vec3 uvw;"
    "out vec4 color;"
    "out vec2 pixPos;"

    "void main()"
    "{"
    "  gl_Position = vec4(vertexPos, 0, 1);"
    "  uvw = vertexUVW;"
    "  color = vertexRGBA;"
    "  pixPos = (vertexPos + vec2(-0.5, 0.5)) * 2.0;"
    "}");

  figureFrag.compileFromString(
    "#version 330 core\n"
    "uniform sampler2DArray tex;"
    "in vec3 uvw;"
    "in vec4 color;"
    "in vec2 pixPos;"
    "out vec4 out_color;"

    "float rand(vec2 co){"
    "  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);"
    "}"

    "void main()"
    "{"
    "  vec4 texcol = texture(tex, uvw).rgba;"
    "  float alpha = (1.0f - texcol.b) * color.a;"
    "  vec3 rgb = mix(texcol.r * color.rgb, texcol.rrr, texcol.g);"
    "  out_color = vec4(rgb * (1.0f + (rand(pixPos) - 0.5f) / 30.0f * alpha), alpha);"
    "}");

  figureProg.attachShader(figureVert);
  figureProg.attachShader(figureFrag);
  figureProg.link();
  figureProg.use();
  figureProg.setUniform("tex", 0);

  std::string backPath = Crosy::getExePath() + "\\textures\\blocks.png";
  int imageWidth, imageHeight, channels;
  unsigned char * img = SOIL_load_image(backPath.c_str(), &imageWidth, &imageHeight, &channels, SOIL_LOAD_RGBA);
  assert(imageWidth == Globals::mainArrayTextureSize);
  assert(!(imageHeight % Globals::mainArrayTextureSize));

  glGenTextures(1, &Globals::mainArrayTextureId);
  assert(!checkGlErrors());
  glBindTexture(GL_TEXTURE_2D_ARRAY, Globals::mainArrayTextureId);
  assert(!checkGlErrors());
  const int glyphsQty = 37 * 3;
  const int textureQty = imageHeight / Globals::mainArrayTextureSize;
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, Globals::mainArrayTextureSize, Globals::mainArrayTextureSize, textureQty + glyphsQty, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  assert(!checkGlErrors());
  glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, Globals::mainArrayTextureSize, Globals::mainArrayTextureSize, textureQty, GL_RGBA, GL_UNSIGNED_BYTE, img);
  assert(!checkGlErrors());
  SOIL_free_image_data(img);

  FT_Error err;
  err = FT_Init_FreeType(&ftLibrary);
  assert(!err);
  err = FT_New_Face(ftLibrary, (Crosy::getExePath() + "/fonts/Montserrat-Bold.otf").c_str(), 0, &ftFace);
  assert(!err);
  err = FT_Set_Char_Size(ftFace, Globals::smallFontSize * 64, Globals::smallFontSize * 64, 64, 64);
  assert(!err);

  for (char ch = '0'; ch <= '9'; ch++)
    loadGlyph(ch, Globals::smallFontSize);

  for (char ch = 'A'; ch <= 'Z'; ch++)
    loadGlyph(ch, Globals::smallFontSize);

  loadGlyph(' ', Globals::smallFontSize);

  err = FT_Set_Char_Size(ftFace, Globals::midFontSize * 64, Globals::midFontSize * 64, 64, 64);
  assert(!err);

  for (char ch = '0'; ch <= '9'; ch++)
    loadGlyph(ch, Globals::midFontSize);

  for (char ch = 'A'; ch <= 'Z'; ch++)
    loadGlyph(ch, Globals::midFontSize);

  loadGlyph(' ', Globals::midFontSize);

  err = FT_Set_Char_Size(ftFace, Globals::bigFontSize * 64, Globals::bigFontSize * 64, 64, 64);
  assert(!err);

  for (char ch = '0'; ch <= '9'; ch++)
    loadGlyph(ch, Globals::bigFontSize);

  for (char ch = 'A'; ch <= 'Z'; ch++)
    loadGlyph(ch, Globals::bigFontSize);

  loadGlyph(' ', Globals::midFontSize);

  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_LINEAR_MIPMAP_LINEAR);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, -1);
  assert(!checkGlErrors());

  glDisable(GL_CULL_FACE);
  assert(!checkGlErrors());

  glEnable(GL_DEPTH_TEST);
  assert(!checkGlErrors());

  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());

  resize(width, height);
}

void OpenGLRender::resize(int width, int height)
{
  const float gameAspect = Globals::gameBkSize.x / Globals::gameBkSize.y;

  if (!height || float(width) / height > gameAspect)
    glViewport((width - height) / 2, 0, height, height);
  else
    glViewport(int(width * (1.0f - 1.0f / gameAspect)) / 2, (height - int(width / gameAspect)) / 2, int(width / gameAspect), int(width / gameAspect));

  glClearDepth(-1.0f);
  assert(!checkGlErrors());

  glClear(GL_DEPTH_BUFFER_BIT);
  assert(!checkGlErrors());

  glDepthFunc(GL_ALWAYS);
  assert(!checkGlErrors());

  clearVertices();
  buildBackground();
  sendToDevice();
  drawMesh();

  glDepthFunc(GL_LEQUAL);
  assert(!checkGlErrors());
}

void OpenGLRender::update()
{
  if (showWireframe)
  {
    glDisable(GL_BLEND);
    assert(!checkGlErrors());
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    assert(!checkGlErrors());
  }
  else
  {
    glEnable(GL_BLEND);
    assert(!checkGlErrors());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    assert(!checkGlErrors());
  }

  glClear(GL_COLOR_BUFFER_BIT);
  assert(!checkGlErrors());

  rebuildMesh();
  drawMesh();
}

void OpenGLRender::rebuildMesh()
{
  clearVertices();

  buildBackground();
  buidGlassShadow();
  buidGlassBlocks();
  biuldGlassGlow();
  buildFigureBlocks();
  buildFigureGlow();
  buildDropTrails();
  buildRowFlashes();

  //buildMainMenu();

  sendToDevice();
}

void OpenGLRender::drawMesh()
{
  figureProg.use();
  glEnableVertexAttribArray(0);
  assert(!checkGlErrors());
  glEnableVertexAttribArray(1);
  assert(!checkGlErrors());
  glEnableVertexAttribArray(2);
  assert(!checkGlErrors());

  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  assert(!checkGlErrors());
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
  assert(!checkGlErrors());
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(2 * sizeof(float)));
  assert(!checkGlErrors());
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
  assert(!checkGlErrors());

  glDrawArrays(GL_TRIANGLES, 0, vertexCount);
  assert(!checkGlErrors());
  
  glDisableVertexAttribArray(0);
  assert(!checkGlErrors());
  glDisableVertexAttribArray(1);
  assert(!checkGlErrors());
  glDisableVertexAttribArray(2);
  assert(!checkGlErrors());
}

Cell * OpenGLRender::getGlassCell(int x, int y)
{
  if (x < 0 || y < 0 || x >= gameLogic.glassWidth || y >= gameLogic.glassHeight)
    return NULL;

  Cell * cell = gameLogic.glass.data() + x + y * gameLogic.glassWidth;

  if (!cell->figureId && !gameLogic.haveFallingRows)
  {
    bool isInCurFigure =
      x >= gameLogic.curFigureX &&
      x < gameLogic.curFigureX + gameLogic.curFigure.dim &&
      y >= gameLogic.curFigureY &&
      y < gameLogic.curFigureY + gameLogic.curFigure.dim;

    if (isInCurFigure)
      cell = gameLogic.curFigure.cells.data() + x - gameLogic.curFigureX + (y - gameLogic.curFigureY) * gameLogic.curFigure.dim;
  }

  return cell;
}

Cell * OpenGLRender::getFigureCell(Figure & figure, int x, int y)
{
  if (x < 0 || y < 0 || x >= figure.dim || y >= figure.dim)
    return NULL;

  Cell * cell = figure.cells.data() + x + y * figure.dim;
  return cell;
}

void OpenGLRender::addVertex(const glm::vec2 & xy, const glm::vec2 & uv, int texIndex, const glm::vec3 & color, float alpha)
{
  vertexBuffer.push_back(xy.x);
  vertexBuffer.push_back(xy.y);
  vertexBuffer.push_back(uv.x);
  vertexBuffer.push_back(uv.y);
  vertexBuffer.push_back(float(texIndex));
  vertexBuffer.push_back(color.r);
  vertexBuffer.push_back(color.g);
  vertexBuffer.push_back(color.b);
  vertexBuffer.push_back(alpha);
  vertexCount++;
}

void OpenGLRender::clearVertices()
{
  vertexCount = 0;
  vertexBuffer.clear();
}

void OpenGLRender::sendToDevice()
{
  if (vertexCount)
  {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    assert(!checkGlErrors());

    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), vertexBuffer.data(), GL_STATIC_DRAW);
    assert(!checkGlErrors());
  }
}

void OpenGLRender::buildBackground()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;

  // add game base background to the mesh

  glm::vec2 origin = Globals::gameBkPos;
  const int xTess = 6;
  const int yTess = 6;

  for (int y = 0; y < yTess; y++)
  for (int x = 0; x < xTess; x++)
  {
    glm::vec2 verts[4] =
    {
      { Globals::gameBkSize.x * x / xTess,       -Globals::gameBkSize.y * y / yTess },
      { Globals::gameBkSize.x * x / xTess,       -Globals::gameBkSize.y * (y + 1) / yTess },
      { Globals::gameBkSize.x * (x + 1) / xTess, -Globals::gameBkSize.y * y / yTess },
      { Globals::gameBkSize.x * (x + 1) / xTess, -Globals::gameBkSize.y * (y + 1) / yTess },
    };

    const float xReps = 80.0f;
    const float yReps = xReps / Globals::gameBkSize.x * Globals::gameBkSize.y * 0.85f;
    const float texScaleCorrection = 0.56f; // tile deformation to get power of two texture size
    const float xVal = xReps * texScaleCorrection;
    const float yVal = yReps;

    glm::vec2 uv[4] =
    {
      { xVal * x / xTess, yVal * y / yTess },
      { xVal * x / xTess, yVal * (y + 1) / yTess },
      { xVal * (x + 1) / xTess, yVal * y / yTess },
      { xVal * (x + 1) / xTess, yVal * (y + 1) / yTess },
    };

    float fx0 = float(abs(xTess - 2 * x)) / xTess;
    float fy0 = float(y) / yTess;
    float fx1 = float(abs(xTess - 2 * (x + 1))) / xTess;
    float fy1 = float(y + 1) / yTess;

    float light[4] =
    {
      1.0f - sqrtf(fx0 * fx0 + fy0 * fy0) / M_SQRT2,
      1.0f - sqrtf(fx0 * fx0 + fy1 * fy1) / M_SQRT2,
      1.0f - sqrtf(fx1 * fx1 + fy0 * fy0) / M_SQRT2,
      1.0f - sqrtf(fx1 * fx1 + fy1 * fy1) / M_SQRT2,
    };

    glm::vec3 darkColor(0.05f, 0.1f, 0.2f);
    glm::vec3 lightColor(0.3f, 0.6f, 1.0f);

    glm::vec3 col[4] =
    {
      darkColor + (lightColor - darkColor) * light[0],
      darkColor + (lightColor - darkColor) * light[1],
      darkColor + (lightColor - darkColor) * light[2],
      darkColor + (lightColor - darkColor) * light[3],
    };

    addVertex(origin + verts[0], uv[0], Globals::backgroundTexIndex, col[0], 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::backgroundTexIndex, col[1], 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::backgroundTexIndex, col[2], 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::backgroundTexIndex, col[1], 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::backgroundTexIndex, col[2], 1.0f);
    addVertex(origin + verts[3], uv[3], Globals::backgroundTexIndex, col[3], 1.0f);
  }

  // add glass background to the mesh

  origin = Globals::glassPos;

  for (int y = 0, yTess = 6; y < yTess; y++)
  for (int x = 0, xTess = 6; x < xTess; x++)
  {
    glm::vec2 verts[4] =
    {
      { Globals::glassSize.x * x / xTess, -Globals::glassSize.y * y / yTess },
      { Globals::glassSize.x * x / xTess, -Globals::glassSize.y * (y + 1) / yTess },
      { Globals::glassSize.x * (x + 1) / xTess, -Globals::glassSize.y * y / yTess },
      { Globals::glassSize.x * (x + 1) / xTess, -Globals::glassSize.y * (y + 1) / yTess },
    };

    float fx0 = float(abs(xTess - 2 * x)) / xTess;
    float fy0 = float(abs(xTess / 4 - y)) / yTess;
    float fx1 = float(abs(xTess - 2 * (x + 1))) / xTess;
    float fy1 = float(abs(xTess / 4 - (y + 1))) / yTess;

    float light[4] =
    {
      sqrtf(fx0 * fx0 + fy0 * fy0) / M_SQRT2,
      sqrtf(fx0 * fx0 + fy1 * fy1) / M_SQRT2,
      sqrtf(fx1 * fx1 + fy0 * fy0) / M_SQRT2,
      sqrtf(fx1 * fx1 + fy1 * fy1) / M_SQRT2,
    };

    double a = 0.0f;
    static double freq = (double)Crosy::getPerformanceFrequency();

    if (freq)
      a = double(Crosy::getPerformanceCounter()) / freq / 300.0f;

    const float rMin = 0.1f;
    const float gMin = 0.1f;
    const float bMin = 0.2f;
    const float rMax = 0.3f;
    const float gMax = 0.3f;
    const float bMax = 0.5f;
    const float darkMul = 0.25f;
    const float darkDiff = (float)M_PI / 20.0f;
    const float gDiff = (float)M_PI / 3.0f;
    const float bDiff = 2.0f * (float)M_PI / 3.0f;

    glm::vec3 lightColor(rMin + (rMax - rMin) * abs(sin(a)),
                         gMin + (gMax - gMin) * abs(sin(a + gDiff)),
                         bMin + (bMax - bMin) * abs(sin(a + bDiff)));

    glm::vec3 darkColor(darkMul * (rMin + (rMax - rMin) * abs(sin(a + darkDiff))),
                        darkMul * (gMin + (gMax - gMin) * abs(sin(a + gDiff + darkDiff))),
                        darkMul * (bMin + (bMax - bMin) * abs(sin(a + bDiff + darkDiff))));

    glm::vec3 col[4] =
    {
      darkColor + (lightColor - darkColor) * light[0],
      darkColor + (lightColor - darkColor) * light[1],
      darkColor + (lightColor - darkColor) * light[2],
      darkColor + (lightColor - darkColor) * light[3],
    };

    glm::vec2 uv(0.5f, 0.5f);

    addVertex(origin + verts[0], uv, Globals::emptyTexIndex, col[0], 1.0f);
    addVertex(origin + verts[1], uv, Globals::emptyTexIndex, col[1], 1.0f);
    addVertex(origin + verts[2], uv, Globals::emptyTexIndex, col[2], 1.0f);
    addVertex(origin + verts[1], uv, Globals::emptyTexIndex, col[1], 1.0f);
    addVertex(origin + verts[2], uv, Globals::emptyTexIndex, col[2], 1.0f);
    addVertex(origin + verts[3], uv, Globals::emptyTexIndex, col[3], 1.0f);
  }

  // add score to the mesh

  {
    origin = Globals::gameBkPos + glm::vec2(Globals::scoreBarGaps, -Globals::scoreBarGaps);
    const float height = Globals::scoreBarHeight - 2.0f * Globals::scoreBarGaps;
    const float width1 = Globals::scoreBarCaptionWidth - 2.0f * Globals::scoreBarGaps;
    const float width2 = Globals::scoreBarValueWidth - Globals::scoreBarGaps;

    const glm::vec2 verts[8] =
    {
      { 0.0f, 0.0f },
      { 0.0f, -height },
      { width1, 0.0f },
      { width1, -height },
      { 0.0f, 0.0f },
      { 0.0f, -height },
      { width2, 0.0f },
      { width2, -height },
    };

    const glm::vec2 uv(0.5f);

    glm::vec3 color(0.0f);
    const float alpha = 0.6f;

    addVertex(origin + verts[0], uv, Globals::emptyTexIndex, color, alpha);
    addVertex(origin + verts[1], uv, Globals::emptyTexIndex, color, alpha);
    addVertex(origin + verts[2], uv, Globals::emptyTexIndex, color, alpha);
    addVertex(origin + verts[1], uv, Globals::emptyTexIndex, color, alpha);
    addVertex(origin + verts[2], uv, Globals::emptyTexIndex, color, alpha);
    addVertex(origin + verts[3], uv, Globals::emptyTexIndex, color, alpha);

    buildTextMesh("SCORE", Globals::midFontSize, 0.08f, glm::vec3(1.0f), origin.x + 0.5f * Globals::scoreBarCaptionWidth, origin.y - 0.8f * height, otCenter);
    origin.x += Globals::scoreBarCaptionWidth;

    addVertex(origin + verts[4], uv, Globals::emptyTexIndex, color, alpha);
    addVertex(origin + verts[5], uv, Globals::emptyTexIndex, color, alpha);
    addVertex(origin + verts[6], uv, Globals::emptyTexIndex, color, alpha);
    addVertex(origin + verts[5], uv, Globals::emptyTexIndex, color, alpha);
    addVertex(origin + verts[6], uv, Globals::emptyTexIndex, color, alpha);
    addVertex(origin + verts[7], uv, Globals::emptyTexIndex, color, alpha);

    std::string str = std::to_string(gameLogic.curScore);
    buildTextMesh(str.c_str() , Globals::bigFontSize, 0.095f, glm::vec3(1.0f), origin.x + 0.5f * Globals::scoreBarValueWidth, origin.y - 0.86f * height, otCenter);
  }

  // add MENU button to mesh

  {
    glm::vec2 origin = Globals::gameBkPos;
    origin.x += Globals::scoreBarCaptionWidth + Globals::scoreBarValueWidth;

    const glm::vec2 verts[6] =
    {
      { 0.0f, 0.0f },
      { 0.5f * Globals::scoreBarHeight, -0.5f * Globals::scoreBarHeight },
      { 0.0f, -Globals::scoreBarHeight },
      { Globals::scoreBarMenuWidth, 0.0f },
      { Globals::scoreBarMenuWidth - 0.5f * Globals::scoreBarHeight, -0.5f * Globals::scoreBarHeight },
      { Globals::scoreBarMenuWidth, -Globals::scoreBarHeight },
    };

    const glm::vec2 uv[2] =
    {
      { pixSize, pixSize },
      { 0.25f, 0.25f },
    };

    glm::vec3 color(0.3f, 0.6f, 0.9f);
    const float alpha = 1.0f;

    addVertex(origin + verts[0], uv[0], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[1], uv[1], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[2], uv[0], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[0], uv[0], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[1], uv[1], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[3], uv[0], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[1], uv[1], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[3], uv[0], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[4], uv[1], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[3], uv[0], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[4], uv[1], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[5], uv[0], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[1], uv[1], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[2], uv[0], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[4], uv[1], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[2], uv[0], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[4], uv[1], Globals::holdFigureBkTexIndex, color, alpha);
    addVertex(origin + verts[5], uv[0], Globals::holdFigureBkTexIndex, color, alpha);

    buildTextMesh("MENU", Globals::smallFontSize, 0.06f, glm::vec3(1.0f), origin.x + 0.48f * Globals::scoreBarMenuWidth, origin.y - 0.68f * Globals::scoreBarHeight, otCenter);
  }

  // add hold figure and next figure backgrounds to mesh

  {
    origin =
    {
      Globals::glassPos.x - Globals::holdNextBkHorzGap - Globals::holdNextBkSize,
      Globals::glassPos.y - Globals::dafaultCaptionHeight,
    };

    buildTextMesh("HOLD", Globals::smallFontSize, 0.055f, glm::vec3(1.0f), origin.x + 0.5f * Globals::holdNextBkSize, origin.y + 0.1f * Globals::dafaultCaptionHeight, otCenter);

    const glm::vec2 verts[4] =
    {
      { 0.0f, 0.0f },
      { 0.0f, -Globals::holdNextBkSize },
      { Globals::holdNextBkSize, 0.0f },
      { Globals::holdNextBkSize, -Globals::holdNextBkSize },
    };

    const glm::vec2 uv[4] =
    {
      { pixSize, pixSize },
      { pixSize, 1.0f - pixSize },
      { 1.0f - pixSize, pixSize },
      { 1.0f - pixSize, 1.0f - pixSize }
    };

    glm::vec3 color(0.5f);

    if (gameLogic.holdFigure.color != Globals::Color::clNone)
      color = Globals::ColorValues[gameLogic.holdFigure.color];

    addVertex(origin + verts[0], uv[0], Globals::holdFigureBkTexIndex, color, 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::holdFigureBkTexIndex, color, 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::holdFigureBkTexIndex, color, 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::holdFigureBkTexIndex, color, 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::holdFigureBkTexIndex, color, 1.0f);
    addVertex(origin + verts[3], uv[3], Globals::holdFigureBkTexIndex, color, 1.0f);

    origin.x += Globals::holdNextBkSize + Globals::glassSize.x + 2.0f * Globals::holdNextBkHorzGap;

    buildTextMesh("NEXT", Globals::smallFontSize, 0.055f, glm::vec3(1.0f), origin.x + 0.45f * Globals::holdNextBkSize, origin.y + 0.1f * Globals::dafaultCaptionHeight, otCenter);

    if (gameLogic.nextFigures[0].color != Globals::Color::clNone)
      color = Globals::ColorValues[gameLogic.nextFigures[0].color];

    addVertex(origin + verts[0], uv[0], Globals::nextFigureBkTexIndex, color, 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::nextFigureBkTexIndex, color, 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::nextFigureBkTexIndex, color, 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::nextFigureBkTexIndex, color, 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::nextFigureBkTexIndex, color, 1.0f);
    addVertex(origin + verts[3], uv[3], Globals::nextFigureBkTexIndex, color, 1.0f);
  }

// add level and goal backgrounds to mesh

  {
    origin =
    {
      Globals::glassPos.x - Globals::holdNextBkHorzGap - Globals::holdNextBkSize,
      Globals::glassPos.y - 0.4f * Globals::glassSize.y,
    };

    buildTextMesh("LEVEL", Globals::smallFontSize, 0.055f, glm::vec3(1.0f), origin.x + 0.5f * Globals::holdNextBkSize, origin.y - 0.95f * Globals::dafaultCaptionHeight, otCenter);

    origin.y -= Globals::dafaultCaptionHeight;

    const glm::vec2 verts[4] =
    {
      { 0.0f, 0.0f },
      { 0.0f, -0.6f * Globals::holdNextBkSize },
      { Globals::holdNextBkSize, 0.0f },
      { Globals::holdNextBkSize, -0.6f * Globals::holdNextBkSize },
    };

    const glm::vec2 uv[4] =
    {
      { pixSize, pixSize },
      { pixSize, 1.0f - pixSize },
      { 1.0f - pixSize, pixSize },
      { 1.0f - pixSize, 1.0f - pixSize }
    };

    glm::vec3 color(0.2f, 0.45f, 0.8f);

    addVertex(origin + verts[0], uv[0], Globals::levelGoalBkTexIndex, color, 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::levelGoalBkTexIndex, color, 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::levelGoalBkTexIndex, color, 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::levelGoalBkTexIndex, color, 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::levelGoalBkTexIndex, color, 1.0f);
    addVertex(origin + verts[3], uv[3], Globals::levelGoalBkTexIndex, color, 1.0f);

    std::string str = std::to_string(gameLogic.curLevel);
    buildTextMesh(str.c_str(), Globals::bigFontSize, 0.11f, glm::vec3(1.0f), origin.x + 0.5f * Globals::holdNextBkSize, origin.y - 0.45f * Globals::holdNextBkSize, otCenter);

    origin.y -= Globals::holdNextBkSize;

    buildTextMesh("GOAL", Globals::smallFontSize, 0.055f, glm::vec3(1.0f), origin.x + 0.5f * Globals::holdNextBkSize, origin.y - 0.95f * Globals::dafaultCaptionHeight, otCenter);

    origin.y -= Globals::dafaultCaptionHeight;

    addVertex(origin + verts[0], uv[0], Globals::levelGoalBkTexIndex, color, 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::levelGoalBkTexIndex, color, 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::levelGoalBkTexIndex, color, 1.0f);
    addVertex(origin + verts[1], uv[1], Globals::levelGoalBkTexIndex, color, 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::levelGoalBkTexIndex, color, 1.0f);
    addVertex(origin + verts[3], uv[3], Globals::levelGoalBkTexIndex, color, 1.0f);

    str = std::to_string(gameLogic.curGoal);
    buildTextMesh(str.c_str(), Globals::bigFontSize, 0.11f, glm::vec3(1.0f), origin.x + 0.5f * Globals::holdNextBkSize, origin.y - 0.45f * Globals::holdNextBkSize, otCenter);
  }

}

void OpenGLRender::buidGlassShadow()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float shadowWidth = 0.15f;
  const glm::vec3 zeroCol(0.0f, 0.0f, 0.0f);
  const float scale = Globals::glassSize.x / gameLogic.glassWidth;

  for (int y = 0; y < gameLogic.glassHeight; y++)
  for (int x = 0; x < gameLogic.glassWidth; x++)
  {
    Cell * cell = getGlassCell(x, y);
    glm::vec2 origin = Globals::glassPos;
    
    if (gameLogic.rowElevation[y])
      origin.y += scale * gameLogic.rowCurrentElevation[y];

    if (cell->figureId)
    {
      Cell * rightCell = getGlassCell(x + 1, y);
      Cell * rightBottomCell = getGlassCell(x + 1, y + 1);
      Cell * bottomCell = getGlassCell(x, y + 1);

      if (bottomCell && bottomCell->figureId != cell->figureId)
      {
        Cell * leftCell = getGlassCell(x - 1, y);
        Cell * bottomLeftCell = getGlassCell(x - 1, y + 1);
        bool softLeft = !leftCell || leftCell->figureId != cell->figureId;

        glm::vec2 verts[4] =
        {
          { x,        -y - 1.0f + pixSize },
          { x,        -y - 1.0f - shadowWidth },
          { x + 1.0f, -y - 1.0f + pixSize },
          { x + 1.0f, -y - 1.0f - shadowWidth }
        };

        glm::vec2 uv[4] =
        {
          { softLeft ? 1.0f - pixSize: 0.5f, 0.5f },
          { 1.0f - pixSize,                  1.0f - pixSize },
          { 0.5f,                            0.5f },
          { 1.0f - pixSize,                  1.0f - pixSize }
        };

        if (softLeft)
          verts[1].x += shadowWidth;

        if (bottomLeftCell && bottomLeftCell->figureId == cell->figureId)
        {
          verts[0].x -= pixSize;
          verts[1].x += shadowWidth;
        }

        if (rightBottomCell && rightBottomCell->figureId != cell->figureId && rightCell && rightCell->figureId != cell->figureId)
        {
          verts[2].x -= pixSize;
          verts[3].x += shadowWidth;
        }

        addVertex(origin + scale * verts[0], uv[0], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[3], uv[3], Globals::shadowTexIndex, zeroCol, 1.0f);
      }

      if (rightCell && rightCell->figureId != cell->figureId)
      {
        Cell * topCell = getGlassCell(x, y - 1);
        Cell * topRightCell = getGlassCell(x + 1, y - 1);
        bool softTop = !topCell || topCell->figureId != cell->figureId;

        glm::vec2 verts[4] =
        {
          { x + 1.0f - pixSize,     -y },
          { x + 1.0f + shadowWidth, -y },
          { x + 1.0f - pixSize,     -y - 1.0f },
          { x + 1.0f + shadowWidth, -y - 1.0f }
        };

        glm::vec2 uv[4] =
        {
          { softTop ? 1.0f - pixSize: 0.5f, 0.5f },
          { 1.0f - pixSize,                 1.0f - pixSize },
          { 0.5f,                           0.5f },
          { 1.0f - pixSize,                 1.0f - pixSize }
        };

        if (softTop)
          verts[1].y -= shadowWidth;

        if (topRightCell && topRightCell->figureId == cell->figureId)
        {
          verts[0].y += pixSize;
          verts[1].y -= shadowWidth;
        }

        if (rightBottomCell && rightBottomCell->figureId != cell->figureId && bottomCell && bottomCell->figureId != cell->figureId)
        {
          verts[2].y += pixSize;
          verts[3].y -= shadowWidth;
        }

        addVertex(origin + scale * verts[0], uv[0], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::shadowTexIndex, zeroCol, 1.0f);
        addVertex(origin + scale * verts[3], uv[3], Globals::shadowTexIndex, zeroCol, 1.0f);
      }
    }
  }
}

void OpenGLRender::buidGlassBlocks()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float scale = Globals::glassSize.x / gameLogic.glassWidth;

  for (int y = 0; y < gameLogic.glassHeight; y++)
  for (int x = 0; x < gameLogic.glassWidth; x++)
  {
    Cell * cell = getGlassCell(x, y);
    glm::vec2 origin = Globals::glassPos;
    
    if (gameLogic.rowElevation[y])
      origin.y += scale * gameLogic.rowCurrentElevation[y];

    if (cell && cell->figureId)
    {
      const int cellId = cell->figureId;

      for (int i = 0; i < 4; i++)
      {
        int cornerDX = (i & 1) * 2 - 1;
        int cornerDY = (i & 2) - 1;

        Cell * horzAdjCell = getGlassCell(x + cornerDX, y);
        Cell * vertAdjCell = getGlassCell(x, y + cornerDY);
        Cell * cornerAdjCell = getGlassCell(x + cornerDX, y + cornerDY);

        bool haveHorzAdjCell = (horzAdjCell && horzAdjCell->figureId == cellId);
        bool haveVertAdjCell = (vertAdjCell && vertAdjCell->figureId == cellId);
        bool haveCornerAdjCell = (cornerAdjCell && cornerAdjCell->figureId == cellId);

        const glm::vec2 segmentUvArray[3][3] =
        {
          { { 0.5f, 0.5f }, { 0.5f, 1.0f - pixSize }, { 0.0f + pixSize, 1.0f - pixSize }, }, // openSegment
          { { 0.5f, 0.5f }, { 0.0f + pixSize, 0.5f }, { 0.0f + pixSize, 0.0f + pixSize }, }, // partialSegment
          { { 0.5f, 0.5f }, { 0.5f, 0.0f + pixSize }, { 0.0f + pixSize, 0.0f + pixSize }, }, // borderedSegment
        };

        enum SegmentTypes { openSegment, partialSegment, borderedSegment };

        SegmentTypes horzSegmentType, vertSegmentType;

        if (haveHorzAdjCell && haveVertAdjCell && haveCornerAdjCell)
        {
          horzSegmentType = SegmentTypes::openSegment;
          vertSegmentType = SegmentTypes::openSegment;
        }
        else
        {
          if (haveHorzAdjCell)
            horzSegmentType = SegmentTypes::partialSegment;
          else
            horzSegmentType = SegmentTypes::borderedSegment;

          if (haveVertAdjCell)
            vertSegmentType = SegmentTypes::partialSegment;
          else
            vertSegmentType = SegmentTypes::borderedSegment;
        }

        const float dx = float(i & 1);
        const float dy = float((i & 2) >> 1);

        glm::vec2 verts[4] =
        {
          { x + 0.5f, -y - 0.5f },
          { x + 0.5f, -y - dy },
          { x + dx, -y - dy },
          { x + dx, -y - 0.5f },
        };

        const glm::vec3 color = Globals::ColorValues[cell->color];

        addVertex(origin + scale * verts[0], segmentUvArray[vertSegmentType][0], Globals::blockTemplateTexIndex, color, 1.0f);
        addVertex(origin + scale * verts[1], segmentUvArray[vertSegmentType][1], Globals::blockTemplateTexIndex, color, 1.0f);
        addVertex(origin + scale * verts[2], segmentUvArray[vertSegmentType][2], Globals::blockTemplateTexIndex, color, 1.0f);
        addVertex(origin + scale * verts[0], segmentUvArray[horzSegmentType][0], Globals::blockTemplateTexIndex, color, 1.0f);
        addVertex(origin + scale * verts[3], segmentUvArray[horzSegmentType][1], Globals::blockTemplateTexIndex, color, 1.0f);
        addVertex(origin + scale * verts[2], segmentUvArray[horzSegmentType][2], Globals::blockTemplateTexIndex, color, 1.0f);
      }
    }
  }
}

void OpenGLRender::biuldGlassGlow()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float glowWidth = 0.5f;
  const float glowMinAlpha = 0.01f;
  const float glowMaxAlpha = 0.25f;
  const float scale = Globals::glassSize.x / gameLogic.glassWidth;

  for (int y = 0; y < gameLogic.glassHeight; y++)
  for (int x = 0; x < gameLogic.glassWidth; x++)
  {
    Cell * cell = getGlassCell(x, y);
    glm::vec2 origin = Globals::glassPos;

    if (gameLogic.rowElevation[y])
      origin.y += scale * gameLogic.rowCurrentElevation[y];

    if (cell->figureId)
    {
      Cell * leftCell = getGlassCell(x - 1, y);
      Cell * leftTopCell = getGlassCell(x - 1, y - 1);
      Cell * topCell = getGlassCell(x, y - 1);
      Cell * topRightCell = getGlassCell(x + 1, y - 1);
      Cell * rightCell = getGlassCell(x + 1, y);
      Cell * rightBottomCell = getGlassCell(x + 1, y + 1);
      Cell * bottomCell = getGlassCell(x, y + 1);
      Cell * bottomLeftCell = getGlassCell(x - 1, y + 1);

      bool haveLeftCell = leftCell && leftCell->figureId == cell->figureId;
      bool haveLeftTopCell = leftTopCell && leftTopCell->figureId == cell->figureId;
      bool haveTopCell = topCell && topCell->figureId == cell->figureId;
      bool haveTopRightCell = topRightCell && topRightCell->figureId == cell->figureId;
      bool haveRightCell = rightCell && rightCell->figureId == cell->figureId;
      bool haveRightBottomCell = rightBottomCell && rightBottomCell->figureId == cell->figureId;
      bool haveBottomCell = bottomCell && bottomCell->figureId == cell->figureId;
      bool haveBottomLeftCell = bottomLeftCell && bottomLeftCell->figureId == cell->figureId;

      const glm::vec3 color = Globals::ColorValues[cell->color];

      if (leftCell && leftCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x + pixSize,   -y },
          { x - glowWidth, -y },
          { x + pixSize,   -y - 1.0f },
          { x - glowWidth, -y - 1.0f }
        };

        if (haveLeftTopCell)
        {
          verts[0].y += pixSize;
          verts[1].y -= glowWidth;
        }
        else if (topCell && !haveTopCell)
        {
          verts[0].y -= pixSize;
          verts[1].y += glowWidth;
        }

        if (haveBottomLeftCell)
        {
          verts[2].y -= pixSize;
          verts[3].y += glowWidth;
        }
        else if (bottomCell && !haveBottomCell)
        {
          verts[2].y += pixSize;
          verts[3].y -= glowWidth;
        }

        addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
      }

      if (rightCell && rightCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x + 1.0f - pixSize,   -y },
          { x + 1.0f + glowWidth, -y },
          { x + 1.0f - pixSize,   -y - 1.0f },
          { x + 1.0f + glowWidth, -y - 1.0f }
        };

        if (haveTopRightCell)
        {
          verts[0].y += pixSize;
          verts[1].y -= glowWidth;
        }
        else if (topCell && !haveTopCell)
        {
          verts[0].y -= pixSize;
          verts[1].y += glowWidth;
        }

        if (haveRightBottomCell)
        {
          verts[2].y -= pixSize;
          verts[3].y += glowWidth;
        }
        else if (bottomCell && !haveBottomCell)
        {
          verts[2].y += pixSize;
          verts[3].y -= glowWidth;
        }

        addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
      }

      if (topCell && topCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x,        -y - pixSize },
          { x,        -y + glowWidth },
          { x + 1.0f, -y - pixSize },
          { x + 1.0f, -y + glowWidth }
        };

        if (haveLeftTopCell)
        {
          verts[0].x -= pixSize;
          verts[1].x += glowWidth;
        }
        else if (leftCell && !haveLeftCell)
        {
          verts[0].x += pixSize;
          verts[1].x -= glowWidth;
        }

        if (haveTopRightCell)
        {
          verts[2].x += pixSize;
          verts[3].x -= glowWidth;
        }
        else if (rightCell && !haveRightCell)
        {
          verts[2].x -= pixSize;
          verts[3].x += glowWidth;
        }

        addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
      }

      if (bottomCell && bottomCell->figureId != cell->figureId)
      {
        glm::vec2 verts[4] =
        {
          { x,        -y - 1.0f + pixSize },
          { x,        -y - 1.0f - glowWidth },
          { x + 1.0f, -y - 1.0f + pixSize },
          { x + 1.0f, -y - 1.0f - glowWidth }
        };

        if (haveBottomLeftCell)
        {
          verts[0].x -= pixSize;
          verts[1].x += glowWidth;
        }
        else if (leftCell && !haveLeftCell)
        {
          verts[0].x += pixSize;
          verts[1].x -= glowWidth;
        }

        if (haveRightBottomCell)
        {
          verts[2].x += pixSize;
          verts[3].x -= glowWidth;
        }
        else if (rightCell && !haveRightCell)
        {
          verts[2].x -= pixSize;
          verts[3].x += glowWidth;
        }

        addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
        addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
        addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
      }
    }
  }
}

void OpenGLRender::buildFigureBlocks()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float scale = Globals::holdNextBkSize * 0.75f / 4.0f;

  for (int i = -1; i < Globals::nextFiguresCount; i++)
  {

    Figure * figure = NULL;

    if (i < 0)
      figure = &gameLogic.holdFigure;
    else
      figure = &gameLogic.nextFigures[i];

    if (figure->dim)
    {

      glm::vec2 origin;

      int figureLeftGap = figure->dim;
      int figureWidth = 0;
      int figureTopGap = figure->dim;
      int figureHeight = 0;


      for (int x = 0; x < figure->dim; x++)
      {
        bool horzEmpty = true;
        bool vertEmpty = true;

        for (int y = 0; y < figure->dim; y++)
        {
          if (!figure->cells[y + x * figure->dim].isEmpty())
            horzEmpty = false;

          if (!figure->cells[x + y * figure->dim].isEmpty())
            vertEmpty = false;
        }

        if (!horzEmpty)
        {
          if(x < figureTopGap)
            figureTopGap = x;

          if (x > figureHeight)
            figureHeight = x;
        }

        if (!vertEmpty)
        {
          if(x < figureLeftGap)
            figureLeftGap = x;

          if (x > figureWidth)
            figureWidth = x;
        }
      }

      figureHeight -= figureTopGap - 1;
      figureWidth -= figureLeftGap - 1;

      if (i < 0)
      {
        origin.x = Globals::glassPos.x - Globals::holdNextBkHorzGap - 0.5f * Globals::holdNextBkSize - scale * 0.5f * figureWidth - scale * figureLeftGap;
        origin.y = Globals::glassPos.y - Globals::dafaultCaptionHeight - 0.5f * Globals::holdNextBkSize + 0.5f * scale * figureHeight + scale * figureTopGap;
      }
      else
      {
        origin.x = Globals::glassPos.x + Globals::glassSize.x + Globals::holdNextBkHorzGap + 0.5f * Globals::holdNextBkSize - scale * 0.5f * float(figureWidth) - scale * float(figureLeftGap);
        origin.y = Globals::glassPos.y - Globals::dafaultCaptionHeight - 0.5f * Globals::holdNextBkSize + scale * 0.5f * figureHeight + scale * figureTopGap - i * Globals::holdNextBkSize;
      }

      for (int y = 0; y < figure->dim; y++)
      for (int x = 0; x < figure->dim; x++)
      {
        Cell * cell = getFigureCell(*figure, x, y);

        if (cell && !cell->isEmpty())
        {
          for (int i = 0; i < 4; i++)
          {
            int cornerDX = (i & 1) * 2 - 1;
            int cornerDY = (i & 2) - 1;

            Cell * horzAdjCell = getFigureCell(*figure, x + cornerDX, y);
            Cell * vertAdjCell = getFigureCell(*figure, x, y + cornerDY);
            Cell * cornerAdjCell = getFigureCell(*figure, x + cornerDX, y + cornerDY);

            bool haveHorzAdjCell = (horzAdjCell && !horzAdjCell->isEmpty());
            bool haveVertAdjCell = (vertAdjCell && !vertAdjCell->isEmpty());
            bool haveCornerAdjCell = (cornerAdjCell && !cornerAdjCell->isEmpty());

            const glm::vec2 segmentUvArray[3][3] =
            {
              { { 0.5f, 0.5f }, { 0.5f, 1.0f - pixSize }, { 0.0f + pixSize, 1.0f - pixSize }, }, // openSegment
              { { 0.5f, 0.5f }, { 0.0f + pixSize, 0.5f }, { 0.0f + pixSize, 0.0f + pixSize }, }, // partialSegment
              { { 0.5f, 0.5f }, { 0.5f, 0.0f + pixSize }, { 0.0f + pixSize, 0.0f + pixSize }, }, // borderedSegment
            };

            enum SegmentTypes { openSegment, partialSegment, borderedSegment };

            SegmentTypes horzSegmentType, vertSegmentType;

            if (haveHorzAdjCell && haveVertAdjCell && haveCornerAdjCell)
            {
              horzSegmentType = SegmentTypes::openSegment;
              vertSegmentType = SegmentTypes::openSegment;
            }
            else
            {
              if (haveHorzAdjCell)
                horzSegmentType = SegmentTypes::partialSegment;
              else
                horzSegmentType = SegmentTypes::borderedSegment;

              if (haveVertAdjCell)
                vertSegmentType = SegmentTypes::partialSegment;
              else
                vertSegmentType = SegmentTypes::borderedSegment;
            }

            const float dx = float(i & 1);
            const float dy = float((i & 2) >> 1);

            glm::vec2 verts[4] =
            {
              { x + 0.5f, -y - 0.5f },
              { x + 0.5f, -y - dy },
              { x + dx, -y - dy },
              { x + dx, -y - 0.5f },
            };

            const glm::vec3 color = Globals::ColorValues[cell->color];

            addVertex(origin + scale * verts[0], segmentUvArray[vertSegmentType][0], Globals::boldBlockTemplateTexIndex, color, 1.0f);
            addVertex(origin + scale * verts[1], segmentUvArray[vertSegmentType][1], Globals::boldBlockTemplateTexIndex, color, 1.0f);
            addVertex(origin + scale * verts[2], segmentUvArray[vertSegmentType][2], Globals::boldBlockTemplateTexIndex, color, 1.0f);
            addVertex(origin + scale * verts[0], segmentUvArray[horzSegmentType][0], Globals::boldBlockTemplateTexIndex, color, 1.0f);
            addVertex(origin + scale * verts[3], segmentUvArray[horzSegmentType][1], Globals::boldBlockTemplateTexIndex, color, 1.0f);
            addVertex(origin + scale * verts[2], segmentUvArray[horzSegmentType][2], Globals::boldBlockTemplateTexIndex, color, 1.0f);
          }
        }
      }
    }
  }
}

void OpenGLRender::buildFigureGlow()
{
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float glowWidth = 0.3f;
  const float glowMinAlpha = 0.01f;
  const float glowMaxAlpha = 0.25f;
  const float scale = Globals::holdNextBkSize * 0.75f / 4.0f;

  for (int i = -1; i < Globals::nextFiguresCount; i++)
  {

    Figure * figure = NULL;

    if (i < 0)
      figure = &gameLogic.holdFigure;
    else
      figure = &gameLogic.nextFigures[i];

    if (figure->dim)
    {

      glm::vec2 origin;

      int figureLeftGap = figure->dim;
      int figureWidth = 0;
      int figureTopGap = figure->dim;
      int figureHeight = 0;


      for (int x = 0; x < figure->dim; x++)
      {
        bool horzEmpty = true;
        bool vertEmpty = true;

        for (int y = 0; y < figure->dim; y++)
        {
          if (!figure->cells[y + x * figure->dim].isEmpty())
            horzEmpty = false;

          if (!figure->cells[x + y * figure->dim].isEmpty())
            vertEmpty = false;
        }

        if (!horzEmpty)
        {
          if (x < figureTopGap)
            figureTopGap = x;

          if (x > figureHeight)
            figureHeight = x;
        }

        if (!vertEmpty)
        {
          if (x < figureLeftGap)
            figureLeftGap = x;

          if (x > figureWidth)
            figureWidth = x;
        }
      }

      figureHeight -= figureTopGap - 1;
      figureWidth -= figureLeftGap - 1;

      if (i < 0)
      {
        origin.x = Globals::glassPos.x - Globals::holdNextBkHorzGap - 0.5f * Globals::holdNextBkSize - scale * 0.5f * figureWidth - scale * figureLeftGap;
        origin.y = Globals::glassPos.y - Globals::dafaultCaptionHeight - 0.5f * Globals::holdNextBkSize + 0.5f * scale * figureHeight + scale * figureTopGap;
      }
      else
      {
        origin.x = Globals::glassPos.x + Globals::glassSize.x + Globals::holdNextBkHorzGap + 0.5f * Globals::holdNextBkSize - scale * 0.5f * float(figureWidth) - scale * float(figureLeftGap);
        origin.y = Globals::glassPos.y - Globals::dafaultCaptionHeight - 0.5f * Globals::holdNextBkSize + scale * 0.5f * figureHeight + scale * figureTopGap - i * Globals::holdNextBkSize;
      }

      for (int y = 0; y < figure->dim; y++)
      for (int x = 0; x < figure->dim; x++)
      {
        Cell * cell = getFigureCell(*figure, x, y);

        if (cell && !cell->isEmpty())
        {
          Cell * leftCell = getFigureCell(*figure, x - 1, y);
          Cell * leftTopCell = getFigureCell(*figure, x - 1, y - 1);
          Cell * topCell = getFigureCell(*figure, x, y - 1);
          Cell * topRightCell = getFigureCell(*figure, x + 1, y - 1);
          Cell * rightCell = getFigureCell(*figure, x + 1, y);
          Cell * rightBottomCell = getFigureCell(*figure, x + 1, y + 1);
          Cell * bottomCell = getFigureCell(*figure, x, y + 1);
          Cell * bottomLeftCell = getFigureCell(*figure, x - 1, y + 1);

          bool haveLeftCell = leftCell && !leftCell->isEmpty();
          bool haveLeftTopCell = leftTopCell && !leftTopCell->isEmpty();
          bool haveTopCell = topCell && !topCell->isEmpty();
          bool haveTopRightCell = topRightCell && !topRightCell->isEmpty();
          bool haveRightCell = rightCell && !rightCell->isEmpty();
          bool haveRightBottomCell = rightBottomCell && !rightBottomCell->isEmpty();
          bool haveBottomCell = bottomCell && !bottomCell->isEmpty();
          bool haveBottomLeftCell = bottomLeftCell && !bottomLeftCell->isEmpty();

          const glm::vec3 color = Globals::ColorValues[cell->color];

          if (!haveLeftCell)
          {
            glm::vec2 verts[4] =
            {
              { x + pixSize,   -y },
              { x - glowWidth, -y },
              { x + pixSize,   -y - 1.0f },
              { x - glowWidth, -y - 1.0f }
            };

            if (haveLeftTopCell)
            {
              verts[0].y += pixSize;
              verts[1].y -= glowWidth;
            }
            else if (!haveTopCell)
            {
              verts[0].y -= pixSize;
              verts[1].y += glowWidth;
            }

            if (haveBottomLeftCell)
            {
              verts[2].y -= pixSize;
              verts[3].y += glowWidth;
            }
            else if (!haveBottomCell)
            {
              verts[2].y += pixSize;
              verts[3].y -= glowWidth;
            }

            addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
          }

          if (!haveRightCell)
          {
            glm::vec2 verts[4] =
            {
              { x + 1.0f - pixSize,   -y },
              { x + 1.0f + glowWidth, -y },
              { x + 1.0f - pixSize,   -y - 1.0f },
              { x + 1.0f + glowWidth, -y - 1.0f }
            };

            if (haveTopRightCell)
            {
              verts[0].y += pixSize;
              verts[1].y -= glowWidth;
            }
            else if (!haveTopCell)
            {
              verts[0].y -= pixSize;
              verts[1].y += glowWidth;
            }

            if (haveRightBottomCell)
            {
              verts[2].y -= pixSize;
              verts[3].y += glowWidth;
            }
            else if (!haveBottomCell)
            {
              verts[2].y += pixSize;
              verts[3].y -= glowWidth;
            }

            addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
          }

          if (!haveTopCell)
          {
            glm::vec2 verts[4] =
            {
              { x,        -y - pixSize },
              { x,        -y + glowWidth },
              { x + 1.0f, -y - pixSize },
              { x + 1.0f, -y + glowWidth }
            };

            if (haveLeftTopCell)
            {
              verts[0].x -= pixSize;
              verts[1].x += glowWidth;
            }
            else if (!haveLeftCell)
            {
              verts[0].x += pixSize;
              verts[1].x -= glowWidth;
            }

            if (haveTopRightCell)
            {
              verts[2].x += pixSize;
              verts[3].x -= glowWidth;
            }
            else if (!haveRightCell)
            {
              verts[2].x -= pixSize;
              verts[3].x += glowWidth;
            }

            addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
          }

          if (!haveBottomCell)
          {
            glm::vec2 verts[4] =
            {
              { x,        -y - 1.0f + pixSize },
              { x,        -y - 1.0f - glowWidth },
              { x + 1.0f, -y - 1.0f + pixSize },
              { x + 1.0f, -y - 1.0f - glowWidth }
            };

            if (haveBottomLeftCell)
            {
              verts[0].x -= pixSize;
              verts[1].x += glowWidth;
            }
            else if (!haveLeftCell)
            {
              verts[0].x += pixSize;
              verts[1].x -= glowWidth;
            }

            if (haveRightBottomCell)
            {
              verts[2].x += pixSize;
              verts[3].x -= glowWidth;
            }
            else if (!haveRightCell)
            {
              verts[2].x -= pixSize;
              verts[3].x += glowWidth;
            }

            addVertex(origin + scale * verts[0], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[1], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
            addVertex(origin + scale * verts[2], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMaxAlpha, 0.0f);
            addVertex(origin + scale * verts[3], glm::vec2(0.5f), Globals::emptyTexIndex, color * glowMinAlpha, 0.0f);
          }
        }
      }
    }
  }
}

void OpenGLRender::buildDropTrails()
{
  const glm::vec2 origin = Globals::glassPos;
  const float scale = Globals::glassSize.x / gameLogic.glassWidth;
  const float pixSize = Globals::mainArrayTexturePixelSize;

  for (std::list<DropTrail>::iterator it = gameLogic.dropTrails.begin(); it != gameLogic.dropTrails.end(); ++it)
  {
    float currentTrailAlpha = 1.0f - it->getTrailProgress();
    float currentTrailHeight = float(it->height) * currentTrailAlpha;
    const float dx = 0.22f;
    const float dy = 0.1f * currentTrailHeight;

    glm::vec2 verts[4] =
    {
      { glm::max<float>(it->x - dx, 0.0f),                                  -glm::max<float>(it->y - currentTrailHeight, 0.0f)  },
      { glm::min<float>(it->x + 1.0f + 2.0f * dx, (float)gameLogic.glassWidth), -glm::max<float>(it->y - currentTrailHeight, 0.0f) },
      { glm::max<float>(it->x - dx, 0.0f),                                  -glm::min<float>(it->y + dy, (float)gameLogic.glassHeight) },
      { glm::min<float>(it->x + 1.0f + 2.0f * dx, (float)gameLogic.glassWidth), -glm::min<float>(it->y + dy, (float)gameLogic.glassHeight) },
    };

    glm::vec2 uv[4] =
    {
      { pixSize,        pixSize },
      { 1.0f - pixSize, pixSize },
      { pixSize,        1.0f - pixSize },
      { 1.0f - pixSize, 1.0f - pixSize },
    };

    glm::vec3 color = Globals::ColorValues[it->color] * currentTrailAlpha;

    addVertex(origin + scale * verts[0], uv[0], Globals::dropTrailTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[1], uv[1], Globals::dropTrailTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[2], uv[2], Globals::dropTrailTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[1], uv[1], Globals::dropTrailTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[2], uv[2], Globals::dropTrailTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[3], uv[3], Globals::dropTrailTexIndex, color, 0.0f);

    for (int i = 0; i < DropTrail::sparkleQty; i++)
    {
      float sparkleX = it->x + it->sparkles[i].relX;
      float sparkleY = it->y - it->sparkles[i].relY * float(it->height) - it->sparkles[i].speed * it->getTrailProgress();
      const float sparkleSize = 0.1f;

      if (sparkleX < gameLogic.glassWidth - sparkleSize && sparkleY > 0.0f)
      {
        glm::vec2 verts[4] =
        {
          { sparkleX,               -sparkleY },
          { sparkleX + sparkleSize, -sparkleY },
          { sparkleX,               -sparkleY - sparkleSize },
          { sparkleX + sparkleSize, -sparkleY - sparkleSize },
        };

        addVertex(origin + scale * verts[0], uv[0], Globals::dropSparkleTexIndex, color, 0.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::dropSparkleTexIndex, color, 0.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::dropSparkleTexIndex, color, 0.0f);
        addVertex(origin + scale * verts[1], uv[1], Globals::dropSparkleTexIndex, color, 0.0f);
        addVertex(origin + scale * verts[2], uv[2], Globals::dropSparkleTexIndex, color, 0.0f);
        addVertex(origin + scale * verts[3], uv[3], Globals::dropSparkleTexIndex, color, 0.0f);
      }
    }
  }
}

void OpenGLRender::buildRowFlashes()
{
  const glm::vec2 origin = Globals::glassPos;
  const float scale = Globals::glassSize.x / gameLogic.glassWidth;
  const float pixSize = Globals::mainArrayTexturePixelSize;

  float overallProgress = glm::clamp(float(getTimer() - gameLogic.rowsDeleteTimer) / gameLogic.rowsDeletionEffectTime, 0.0f, 1.0f);
  float mul = 1.0f - cos((overallProgress - 0.5f) * (overallProgress < 0.5f ? 0.5f : 2.0f) * (float)M_PI_2);
  float flashAlpha = 1.00f - overallProgress * overallProgress;// mul * mul;
  float dx = 1.0f - mul * 3.0f;
  float dy = 0.25f - 0.75f * mul;

  for (std::vector<int>::iterator it = gameLogic.deletedRows.begin(); it != gameLogic.deletedRows.end(); ++it)
  {
    glm::vec2 verts[4] =
    {
      { 0.0f - dx, -*it + dy },
      { gameLogic.glassWidth + dx, -*it + dy },
      { 0.0f - dx, -*it - 1.0f - dy },
      { gameLogic.glassWidth + dx, -*it - 1.0f - dy },
    };

    glm::vec2 uv[4] =
    {
      { pixSize, pixSize },
      { 1.0f - pixSize, pixSize },
      { pixSize, 1.0f - pixSize },
      { 1.0f - pixSize, 1.0f - pixSize },
    };

    glm::vec3 color(flashAlpha);

    addVertex(origin + scale * verts[0], uv[0], Globals::rowFlashTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[1], uv[1], Globals::rowFlashTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[2], uv[2], Globals::rowFlashTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[1], uv[1], Globals::rowFlashTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[2], uv[2], Globals::rowFlashTexIndex, color, 0.0f);
    addVertex(origin + scale * verts[3], uv[3], Globals::rowFlashTexIndex, color, 0.0f);
  }

  if (!gameLogic.deletedRows.empty())
  {
    float shineProgress = glm::clamp(overallProgress / 0.85f, 0.0f, 1.0f);
    float ltX = (shineProgress - 0.5f) * 3.0f * gameLogic.glassWidth;
    float ltY = 0.5f * (gameLogic.deletedRows.front() + gameLogic.deletedRows.back() + 1.0f);

    for (std::set<GameLogic::CellCoord>::iterator vertGapsIt = gameLogic.deletedRowGaps.begin();
      vertGapsIt != gameLogic.deletedRowGaps.end();
      ++vertGapsIt)
    {
      float gapX = float(vertGapsIt->x);
      float gapY1 = float(vertGapsIt->y);
      float gapY2 = float(vertGapsIt->y + 1);
      float ltDX = gapX - ltX;
      float ltDY1 = gapY1 - ltY;
      float ltDY2 = gapY2 - ltY;
      const float leverRatio = 8.0f;
      float rayEndDX = ltDX * leverRatio;
      float rayEndDY1 = ltDY1 * leverRatio;
      float rayEndDY2 = ltDY2 * leverRatio;

      glm::vec2 uv[4] =
      {
        { 0.5f, pixSize },
        { pixSize, 1.0f - pixSize },
        { 1.0f - pixSize, 1.0f - pixSize },
      };

      float alphaMul = 1.0f - 2.0f * fabs(0.5f - glm::clamp(shineProgress * shineProgress, 0.1f, 1.0f));

      glm::vec3 colors0(0.2f * alphaMul);
      glm::vec3 colors1(0.0f);

      glm::vec2 verts[3] =
      {
        { origin.x + scale * ltX, origin.y - scale * ltY },
        { origin.x + scale * (gapX + rayEndDX), origin.y - scale * (gapY1 + rayEndDY1) },
        { origin.x + scale * (gapX + rayEndDX), origin.y - scale * (gapY2 + rayEndDY2) },
      };

      addVertex(verts[0], uv[0], Globals::rowShineRayTexIndex, colors0, 0.0f);
      addVertex(verts[1], uv[1], Globals::rowShineRayTexIndex, colors1, 0.0f);
      addVertex(verts[2], uv[2], Globals::rowShineRayTexIndex, colors1, 0.0f);
    }

    const float radius = 20.0f;

    glm::vec2 verts[4] =
    {
      { origin.x + scale * (ltX - radius), origin.y - scale * (ltY - radius) },
      { origin.x + scale * (ltX + radius), origin.y - scale * (ltY - radius) },
      { origin.x + scale * (ltX - radius), origin.y - scale * (ltY + radius) },
      { origin.x + scale * (ltX + radius), origin.y - scale * (ltY + radius) },
    };

    glm::vec2 uv[4] =
    {
      { pixSize, pixSize },
      { 1.0f - pixSize, pixSize },
      { pixSize, 1.0f - pixSize },
      { 1.0f - pixSize, 1.0f - pixSize },
    };

    glm::vec3 colors(0.1f * sin(shineProgress * (float)M_PI));

    addVertex(verts[0], uv[0], Globals::rowShineLightTexIndex, colors, 0.0f);
    addVertex(verts[1], uv[1], Globals::rowShineLightTexIndex, colors, 0.0f);
    addVertex(verts[2], uv[2], Globals::rowShineLightTexIndex, colors, 0.0f);
    addVertex(verts[1], uv[1], Globals::rowShineLightTexIndex, colors, 0.0f);
    addVertex(verts[2], uv[2], Globals::rowShineLightTexIndex, colors, 0.0f);
    addVertex(verts[3], uv[3], Globals::rowShineLightTexIndex, colors, 0.0f);
  }
}

void OpenGLRender::buildSidePanel(float x, float y, const char * text, glm::vec3 textColor, bool highlighted)
{
  glm::vec2 origin = Globals::gameBkPos + glm::vec2(x, y);
  const float pixSize = Globals::mainArrayTexturePixelSize;
  const float relWidth = 0.7f;
  const float relHeight = 0.07f;
  const int xTess = 25;
  const int yTess = 4;
  const float width = xTess * 0.03f;// relWidth * Globals::gameBkSize.x;
  const float height = yTess * 0.03f;// relHeight * Globals::gameBkSize.y;
  const float xPart = width / xTess;
  const float yPart = height / yTess;

  for (int y = 0; y < yTess; y++)
  for (int x = 0; x < xTess; x++)
  {
    glm::vec2 verts[4] =
    {
      { x * xPart,        -y * yPart },
      { (x + 1) * xPart,  -y * yPart },
      { x * xPart,        -(y + 1) * yPart },
      { (x + 1) * xPart,  -(y + 1) * yPart },
    };

    const float xReps = 80.0f;
    const float yReps = xReps / Globals::gameBkSize.x * Globals::gameBkSize.y * 0.85f;
    const float texScaleCorrection = 0.56f; // tile deformation to get power of two texture size
    const float xVal = relWidth * xReps * texScaleCorrection;
    const float yVal = relHeight * yReps;

    glm::vec2 uv[4] =
    {
      { xVal * x / xTess, yVal * y / yTess },
      { xVal * (x + 1) / xTess, yVal * y / yTess },
      { xVal * x / xTess, yVal * (y + 1) / yTess },
      { xVal * (x + 1) / xTess, yVal * (y + 1) / yTess },
    };

    glm::vec3 darkColor(0.01f, 0.05f, 0.17f);
    glm::vec3 lightColor(0.4f, 0.7f, 1.2f);

    float lt0 = 1.0f - glm::clamp(2.0f * float(y) / yTess, 0.0f, 1.0f);
    float lt1 = 1.0f - glm::clamp(2.0f * float(y + 1) / yTess, 0.0f, 1.0f);

    glm::vec3 col0 = darkColor + (lightColor - darkColor) * lt0;
    glm::vec3 col1 = darkColor + (lightColor - darkColor) * lt1;

    if (x < xTess - 1 || y)
    {
      addVertex(origin + verts[0], uv[0], Globals::backgroundTexIndex, col0, 1.0f);
      addVertex(origin + verts[1], uv[1], Globals::backgroundTexIndex, col0, 1.0f);
      addVertex(origin + verts[3], uv[3], Globals::backgroundTexIndex, col1, 1.0f);
    }

    addVertex(origin + verts[0], uv[0], Globals::backgroundTexIndex, col0, 1.0f);
    addVertex(origin + verts[2], uv[2], Globals::backgroundTexIndex, col1, 1.0f);
    addVertex(origin + verts[3], uv[3], Globals::backgroundTexIndex, col1, 1.0f);
  }

  buildTextMesh(text, Globals::midFontSize, 0.08f, textColor, origin.x + width * 0.25f, origin.y - height * 0.7f, otLeft);

  const float glowWidth = 0.03f;
  const float angle = atan(yPart / xPart);

  glm::vec2 borderVerts[10] =
  {
    { 0.0f, 0.0f },
    { 0.0f, glowWidth },
    { width - xPart, 0.0f },
    { width - xPart + glowWidth * sin(angle), glowWidth },
    { width, -yPart },
    { width + glowWidth, -yPart + glowWidth * cos(angle) },
    { width, -height},
    { width + glowWidth, -height - glowWidth },
    { 0.0f, -height },
    { 0.0f, -height - glowWidth },
  };

  glm::vec2 borderUV[10] =
  {
    { pixSize, pixSize },
    { pixSize, 1.0f - pixSize },
    { 1.0f - pixSize , pixSize },
    { 1.0f - pixSize, 1.0f - pixSize },
    { 1.0f - pixSize, pixSize },
    { 1.0f - pixSize, 1.0f - pixSize },
    { 1.0f - pixSize, pixSize },
    { 1.0f - pixSize, 1.0f - pixSize },
    { 1.0f - pixSize, pixSize },
    { 1.0f - pixSize, 1.0f - pixSize },
  };

  glm::vec3 color = highlighted ? glm::vec3(2.4f, 2.0f, 0.8f) : glm::vec3(0.4f, 0.7f, 1.2f);

  for (int i = 0; i < 8; i+=2)
  {
    addVertex(origin + borderVerts[i], borderUV[i], Globals::sidePanelGlowingBorderTexIndex, color, 1.0f);
    addVertex(origin + borderVerts[i + 1], borderUV[i + 1], Globals::sidePanelGlowingBorderTexIndex, color, 1.0f);
    addVertex(origin + borderVerts[i + 2], borderUV[i + 2], Globals::sidePanelGlowingBorderTexIndex, color, 1.0f);

    addVertex(origin + borderVerts[i + 1], borderUV[i + 1], Globals::sidePanelGlowingBorderTexIndex, color, 1.0f);
    addVertex(origin + borderVerts[i + 2], borderUV[i + 2], Globals::sidePanelGlowingBorderTexIndex, color, 1.0f);
    addVertex(origin + borderVerts[i + 3], borderUV[i + 3], Globals::sidePanelGlowingBorderTexIndex, color, 1.0f);
  }

  const float innerGlowWidth1 = 0.5f * height;
  const float innerGlowWidth2 = 0.1f * height;

  glm::vec2 innerGlowVerts[14] =
  {
    { 0.0f, 0.0f },
    { 0.0f, -innerGlowWidth1 },
    { 0.75f * width, 0.0f },
    { 0.75f * width, -innerGlowWidth1 },

    { width - xPart,  0.0f },
    { width - xPart,  -innerGlowWidth1 },
    { width,          -yPart },
    { width,          -innerGlowWidth1 - yPart },
    { width,          -yPart },
    { width - innerGlowWidth2, -yPart + innerGlowWidth2 },

    { width, -height },
    { width - innerGlowWidth2, -height + innerGlowWidth2 },
    { 0.0f, -height },
    { 0.0f, -height + innerGlowWidth2 },
  };

  glm::vec2 innerGlowUV[14] =
  {
    { pixSize, pixSize },
    { pixSize, 1.0f - pixSize },
    { 1.0f - pixSize, pixSize },
    { 1.0f - pixSize, 1.0f - pixSize },
    { 1.0f - pixSize, pixSize },
    { 1.0f - pixSize, 1.0f - pixSize },
    { 1.0f - pixSize, pixSize },
    { 1.0f - pixSize, 1.0f - pixSize },
    { 1.0f - pixSize, pixSize },
    { 1.0f - pixSize, 1.0f - pixSize },
    { 1.0f - pixSize, pixSize },
    { 1.0f - pixSize, 1.0f - pixSize },
    { pixSize, pixSize },
    { pixSize, 1.0f - pixSize },
  };

  for (int i = 0; i < 12; i += 2)
  {
    addVertex(origin + innerGlowVerts[i], innerGlowUV[i], Globals::sidePanelInnerGlowTexIndex, color, 1.0f);
    addVertex(origin + innerGlowVerts[i + 1], innerGlowUV[i + 1], Globals::sidePanelInnerGlowTexIndex, color, 1.0f);
    addVertex(origin + innerGlowVerts[i + 2], innerGlowUV[i + 2], Globals::sidePanelInnerGlowTexIndex, color, 1.0f);

    addVertex(origin + innerGlowVerts[i + 1], innerGlowUV[i + 1], Globals::sidePanelInnerGlowTexIndex, color, 1.0f);
    addVertex(origin + innerGlowVerts[i + 2], innerGlowUV[i + 2], Globals::sidePanelInnerGlowTexIndex, color, 1.0f);
    addVertex(origin + innerGlowVerts[i + 3], innerGlowUV[i + 3], Globals::sidePanelInnerGlowTexIndex, color, 1.0f);
  }
}

void OpenGLRender::buildMainMenu()
{
  const glm::vec2 origin = Globals::gameBkPos;

  glm::vec2 v[4] =
  {
    { 0.0f, 0.0f },
    { 0.0f, -Globals::gameBkSize.y },
    { Globals::gameBkSize.x, 0.0f },
    { Globals::gameBkSize.x, -Globals::gameBkSize.y },
  };

  glm::vec2 u(0.5f);

  glm::vec3 color(0.0f);
  const float alpha = 0.75f;

  addVertex(origin + v[0], u, Globals::emptyTexIndex, color, alpha);
  addVertex(origin + v[1], u, Globals::emptyTexIndex, color, alpha);
  addVertex(origin + v[2], u, Globals::emptyTexIndex, color, alpha);
  addVertex(origin + v[1], u, Globals::emptyTexIndex, color, alpha);
  addVertex(origin + v[2], u, Globals::emptyTexIndex, color, alpha);
  addVertex(origin + v[3], u, Globals::emptyTexIndex, color, alpha);

  static float a = 0.0f;
  char * menu[4] =
  {
    "NEW GAME",
    "OPTIONS",
    "CREDITS",
    "QUIT"
  };

  for (int i = 0; i < 4; i++)
  {
    float x = -1.8f *(1.0f - fabs(sin(a + 0.1f * i)));
    glm::vec3 color = ((i == 1) ? (glm::vec3(1.0f, 0.9f, 0.4f)) : (glm::vec3(0.4f, 0.7f, 1.0f)));
    buildSidePanel(0.0f, -0.4f - 0.2f * i, menu[i], color, i == 1);
  }
  a += 0.1f;
}

void OpenGLRender::loadGlyph(char ch, int size)
{
  const int texSize = Globals::mainArrayTextureSize;
  static int fontNextTexIndex = Globals::fontFirstTexIndex;
  static std::vector<uint32_t> destBuf(texSize * texSize);

  FT_Error err = FT_Load_Char(ftFace, (const FT_UInt)ch, FT_LOAD_DEFAULT | FT_LOAD_IGNORE_TRANSFORM | FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);
  assert(!err);
  assert(ftFace->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
  Glyph & glyph = glyphs[ch][size];
  glyph.glyphIndex = FT_Get_Char_Index(ftFace, FT_ULong(ch));
  glyph.texIndex = fontNextTexIndex++;
  ftFace->glyph->bitmap_left;
  ftFace->glyph->bitmap_top;
  glyph.metrics = ftFace->glyph->metrics;
  uint8_t * srcBuff = ftFace->glyph->bitmap.buffer;
  const int srcWidth = ftFace->glyph->bitmap.width;
  const int srcHeight = ftFace->glyph->bitmap.rows;
  const int srcWidthMinusOne = srcWidth - 1;
  const int srcHeightMinusOne = srcHeight - 1;
  const int srcPitch = ftFace->glyph->bitmap.pitch;

  for (int y = 0; y < texSize; y++)
  for (int x = 0; x < texSize; x++)
  {
    const int brd = 2;
    if (x < brd || y < brd || x >= texSize - brd || y >= texSize - brd)
      destBuf[x + y * texSize] = 0xFFFF0000;
    else
    {
      float srcXf = float(x - brd) / (texSize - brd * 2) * (srcWidth + 1) - 1;
      float srcYf = float(y - brd) / (texSize - brd * 2) * (srcHeight + 1) - 1;
      int srcXi = int(glm::floor(srcXf));
      int srcYi = int(glm::floor(srcYf));
      float uRatio = srcXf - srcXi;
      float vRatio = srcYf - srcYi;
      uint8_t * srcPix = srcBuff + srcXi + srcYi * srcPitch;
      float p00 = srcXi >= 0 && srcYi >= 0 ? float(*srcPix) : 0.0f;
      float p10 = srcYi >= 0 && srcXi < srcWidthMinusOne ? float(*(srcPix + 1)) : 0.0f;
      float p01 = srcXi >= 0 && srcYi < srcHeightMinusOne ? float(*(srcPix + srcPitch)) : 0.0f;
      float p11 = srcXi < srcWidthMinusOne && srcYi < srcHeightMinusOne ? float(*(srcPix + 1 + srcPitch)) : 0.0f;
      uint8_t result = uint8_t(glm::mix(glm::mix(p00, p10, uRatio), glm::mix(p01, p11, uRatio), vRatio));
      destBuf[x + y * texSize] = result | ((0xFF - result) << 16) | 0xFF << 24;
    }
  }

  glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, glyph.texIndex, texSize, texSize, 1, GL_RGBA, GL_UNSIGNED_BYTE, destBuf.data());
  assert(!checkGlErrors());
}

void OpenGLRender::buildTextMesh(const char * str, int fontSize, float scale, glm::vec3 color, float originX, float originY, OriginType originType)
{
  float penX = 0;
  char leftChar = 0;
  static std::vector<glm::vec2> verts(1024);
  static std::vector<glm::vec2> uv(1024);
  static std::vector<int> glyphTexIndex(1024);
  verts.clear();
  uv.clear();
  glyphTexIndex.clear();

  for (const char * pch = str, *eof = pch + strlen(str); pch < eof; pch++)
  {
    GlyphCharMap::iterator charIt = glyphs.find(*pch);
    assert(charIt != glyphs.end());

    if (charIt == glyphs.end())
      continue;

    GlyphSizeMap::iterator sizeIt = charIt->second.find(fontSize);
    assert(sizeIt != charIt->second.end());

    if (sizeIt == charIt->second.end())
      continue;

    Glyph & glyph = sizeIt->second;
    FT_Vector kern = { 0, 0 };

    if (leftChar)
    {
      FT_UInt leftGlyphIndex = FT_Get_Char_Index(ftFace, FT_ULong(leftChar));
      FT_UInt rightGlyphIndex = FT_Get_Char_Index(ftFace, FT_ULong(*pch));
      FT_Error err = FT_Get_Kerning(ftFace, leftGlyphIndex, rightGlyphIndex, FT_KERNING_UNFITTED, &kern);
      assert(!err);
    }

    leftChar = *pch;
    float ltx = penX + scale * (kern.x + glyph.metrics.horiBearingX) / 64 / fontSize;
    float lty = scale * glyph.metrics.horiBearingY / 64 / fontSize;
    float rbx = ltx + scale * glyph.metrics.width / 64 / fontSize;
    float rby = lty - scale * glyph.metrics.height / 64 / fontSize;

    const float pixSize = 0.5f * Globals::mainArrayTexturePixelSize;

    verts.push_back({ ltx, lty });
    verts.push_back({ ltx, rby });
    verts.push_back({ rbx, lty });
    verts.push_back({ rbx, rby });

    uv.push_back({ pixSize, pixSize });
    uv.push_back({ pixSize, 1.0f - pixSize });
    uv.push_back({ 1.0f - pixSize, pixSize });
    uv.push_back({ 1.0f - pixSize, 1.0f - pixSize });

    glyphTexIndex.push_back(glyph.texIndex);

    penX += scale * glyph.metrics.horiAdvance / 64 / fontSize;
  }

  float alpha = 0.0f;
  float meshWidth = verts.back().x - verts.front().x;
  glm::vec2 origin(originX, originY);

  if (originType == otRight)
    origin.x -= meshWidth;
  else if (originType == otCenter)
    origin.x -= 0.5f * meshWidth;

  for (int i = 0, cnt = (int)strlen(str); i < cnt; i++)
  {
    addVertex(origin + verts[i * 4 + 0], uv[i * 4 + 0], glyphTexIndex[i], color, alpha);
    addVertex(origin + verts[i * 4 + 1], uv[i * 4 + 1], glyphTexIndex[i], color, alpha);
    addVertex(origin + verts[i * 4 + 2], uv[i * 4 + 2], glyphTexIndex[i], color, alpha);
    addVertex(origin + verts[i * 4 + 1], uv[i * 4 + 1], glyphTexIndex[i], color, alpha);
    addVertex(origin + verts[i * 4 + 2], uv[i * 4 + 2], glyphTexIndex[i], color, alpha);
    addVertex(origin + verts[i * 4 + 3], uv[i * 4 + 3], glyphTexIndex[i], color, alpha);
  }
}
