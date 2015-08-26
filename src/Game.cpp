#include "Globals.h"
#include "Game.h"
#include "Figure.h"


Game::Game() :
  width(0.5f),
  height(1.0f)
{
  vertexArrayId = 0;
}


Game::~Game()
{
}

bool Game::init()
{
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK)
    return false;
  
  // workaround GLEW issue with GL_INVALID_ENUM rising just after glewInit
  glGetError();

  std::string backPath = Crosy::getExePath() + "\\textures\\blocks.png";
  int width, height, channels;
  unsigned char * img = SOIL_load_image(backPath.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
  assert(width == Globals::mainArrayTextureSize);
  assert(!(height % Globals::mainArrayTextureSize));

  glGenTextures(1, &Globals::mainArrayTextureId);
  assert(!checkGlErrors());
  glBindTexture(GL_TEXTURE_2D_ARRAY, Globals::mainArrayTextureId);
  assert(!checkGlErrors());
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, Globals::mainArrayTextureSize, Globals::mainArrayTextureSize, height / Globals::mainArrayTextureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
  assert(!checkGlErrors());
  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  assert(!checkGlErrors());
  SOIL_free_image_data(img);

  glGenVertexArrays(1, &vertexArrayId);
  assert(!checkGlErrors());

  glBindVertexArray(vertexArrayId);
  assert(!checkGlErrors());

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  assert(!checkGlErrors());
  glDisable(GL_DEPTH_TEST);
  assert(!checkGlErrors());

  glEnable(GL_BLEND);
  assert(!checkGlErrors());

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());

  glDisable(GL_CULL_FACE);
  assert(!checkGlErrors());
  
  //glCullFace(GL_FRONT);
  //assert(!checkGlErrors());
  
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  assert(!checkGlErrors());

  glClearDepth(1.0f);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_LOD_BIAS, -1);
  assert(!checkGlErrors());

  background.init();
  Figure::init();

  return true;
}

void Game::resize(float aspect)
{
  glm::vec2 screen(1.0f);

  if (aspect > background.aspect)
  {
    screen.x = 1.0f / aspect;
    screen.y = 1.0f;
  }
  else
  {
    screen.x = 1.0f / background.aspect;
    screen.y = aspect / background.aspect;
  }

  background.setScreen(screen);
  Figure::setScreen(screen);
}

void Game::pulse()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assert(!checkGlErrors());

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());

  background.draw();

  static Figure fig0(3, Figure::clPurple, "010111000");
  static Figure fig1(3, Figure::clRed, "110011000");
  static Figure fig2(4, Figure::clCyan, "0000111100000000");
  static Figure fig3(2, Figure::clYellow, "1111");
  static Figure fig4(3, Figure::clOrange, "001111000");

  fig0.setPos(-0.66f, -0.7f);
  fig1.setPos(-0.66f + 0.2f, -0.7f);
  fig2.setPos(-0.66f + 0.4f, -0.6f);
  fig3.setPos(-0.66f + 0.6f, -0.5f);
  fig4.setPos(-0.66f + 0.5f, -0.0f);

  Figure::setScale(0.1f);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  assert(!checkGlErrors());

  fig0.drawShadow();
  fig1.drawShadow();
  fig2.drawShadow();
  fig3.drawShadow();
  fig4.drawShadow();

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
  assert(!checkGlErrors());

  fig0.drawFigure();
  fig1.drawFigure();
  fig2.drawFigure();
  fig3.drawFigure();
  fig4.drawFigure();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  assert(!checkGlErrors());

  fig0.drawGlow();
  fig1.drawGlow();
  fig2.drawGlow();
  fig3.drawGlow();
  fig4.drawGlow();
}

void drawBackground()
{
  
}

void Game::drawGlass()
{

}
