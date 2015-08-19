#include "Game.h"
#include "Figure.h"


Game::Game() :
  blockVS(GL_VERTEX_SHADER),
  blockFS(GL_FRAGMENT_SHADER),
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

  glGenVertexArrays(1, &vertexArrayId);
  assert(!glGetError());

  glBindVertexArray(vertexArrayId);
  assert(!glGetError());

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  assert(!glGetError());
  glDisable(GL_CULL_FACE);
  assert(!glGetError());
  
  glCullFace(GL_FRONT);
  assert(!glGetError());
  
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  assert(!glGetError());

  glClearDepth(1.0f);
  assert(!glGetError());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  assert(!glGetError());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  assert(!glGetError());

  background.init();
  Figure::init();

  return true;
}

void Game::resize(float aspect)
{
  glm::vec2 screenScale(1);

  if (aspect > background.aspect)
  {
    screenScale.x = 1 / aspect;
    screenScale.y = 1.0f;
  }
  else
  {
    screenScale.x = 1 / background.aspect;
    screenScale.y = aspect / background.aspect;
  }

  background.setScreenScale(screenScale);
  Figure::setScreenScale(screenScale);
}

void Game::pulse()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assert(!glGetError());

  background.draw();

  Figure fig0(3, Figure::clOrange, "010111000");
  fig0.draw(-0.66f, -0.8f, 0.1f);
  Figure fig1(3, Figure::clRed, "110011000");
  fig1.draw(-0.66f + 0.2f, -0.8f, 0.1f);
  Figure fig2(4, Figure::clCyan, "0000111100000000");
  fig2.draw(-0.66f + 0.4f, -0.7f, 0.1f);
  Figure fig3(2, Figure::clYellow, "1111");
  fig3.draw(-0.66f + 0.6f, -0.6f, 0.1f);
}

void drawBackground()
{
  
}

void Game::drawGlass()
{

}
