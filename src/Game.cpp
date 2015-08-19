#include "Game.h"


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
  glEnable(GL_CULL_FACE);
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

  //blockTexture.generate();

  //const int horzBlocks = 10;
  //const int vertBlocks = 20;
  //glass.resize(horzBlocks * vertBlocks);

  //blockVS.compileFromString(
  //  "#version 330 core\n"
  //  "layout(location = 0) in vec3 vertexPos;\n"
  //  "layout(location = 1) in vec2 vertexUV;\n"
  //  "out vec2 uv;\n"

  //  "void main()\n"
  //  "{\n"
  //  "  gl_Position = /*mvp * */vec4(vertexPos, 1);\n"
  //  "  uv = vertexUV;\n"
  //  "}\n");
  //assert(!blockVS.isError());

  //blockFS.compileFromString(
  //  "#version 330 core\n"
  //  "uniform sampler2D mytexture;\n"
  //  "in vec2 uv;\n"
  //  "out vec3 color;\n"

  //  "void main()\n"
  //  "{\n"
  //  "  color = texture(mytexture, uv).rgb;\n"
  //  "}\n");
  //assert(!blockFS.isError());

  //blockProg.attachShader(blockVS);      assert(!blockProg.isError());
  //blockProg.attachShader(blockFS);      assert(!blockProg.isError());
  //blockProg.link();                     assert(!blockProg.isError()); 

  return true;
}

void Game::resize(float aspect)
{
  glm::mat3 proj(1);

  if (aspect > background.aspect)
  {
    proj[0][0] = 1 / aspect;
    proj[1][1] = 1.0f;
  }
  else
  {
    proj[0][0] = 1 / background.aspect;
    proj[1][1] = aspect / background.aspect;
  }

  background.setProjMatrix(proj);
}

void Game::pulse()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assert(!glGetError());

  background.draw();

  //drawGlass();
}

void drawBackground()
{
  
}

void Game::drawGlass()
{

}
