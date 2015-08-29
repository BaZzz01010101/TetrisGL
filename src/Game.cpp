#include "Globals.h"
#include "Game.h"
#include "Figure.h"

Game::Game() :
  glassWidth(10),
  glassHeight(20),
  glassSqSize(Globals::glassSize.y / glassHeight),
  maxLevel(100),
  maxStepTime(0.1f),
  minStepTime(0.05f),
  level(1),
  curFigure(NULL),
  gameState(gsStartGame)
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

void Game::pulse()
{
  switch (gameState)
  {
  case gsStartGame:
    beforeStartGame();
    gameState = gsPlayingGame;
    break;
  case gsPlayingGame:
    step();
    draw();
    break;
  case gsGameOver:
    draw();
    break;
  default:
    assert(0);
  }

}

float Game::getTime()
{
  static uint64_t freq = Crosy::getPerformanceFrequency();
  assert(freq);

  return float(Crosy::getPerformanceCounter()) / freq;
}

void Game::beforeStartGame()
{
  glassFallingFigures.clear();
  glassFigures.clear();
  nextFigures.clear();
  nextFigures.emplace(nextFigures.end());
  nextFigures.emplace(nextFigures.end());
  nextFigures.emplace(nextFigures.end());
  curFigure = &nextFigures.front();
  curFigure->col = glassWidth / 2 - curFigure->dim / 2;
  curFigure->row = -1;
}

void Game::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assert(!checkGlErrors());

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());

  background.draw();

  Figure::setScale(glassSqSize);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  assert(!checkGlErrors());

  curFigure->drawShadow(Globals::glassPos.x + curFigure->col * glassSqSize, Globals::glassPos.y - curFigure->row * glassSqSize);

  for (std::list<Figure>::iterator it = glassFigures.begin(); it != glassFigures.end(); ++it)
    it->drawShadow(Globals::glassPos.x + it->col * glassSqSize, Globals::glassPos.y - it->row * glassSqSize);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
  assert(!checkGlErrors());

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
  assert(!checkGlErrors());

  curFigure->drawFigure(Globals::glassPos.x + curFigure->col * glassSqSize, Globals::glassPos.y - curFigure->row * glassSqSize);

  for (std::list<Figure>::iterator it = glassFigures.begin(); it != glassFigures.end(); ++it)
    it->drawFigure(Globals::glassPos.x + it->col * glassSqSize, Globals::glassPos.y - it->row * glassSqSize);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  assert(!checkGlErrors());

  curFigure->drawGlow(Globals::glassPos.x + curFigure->col * glassSqSize, Globals::glassPos.y - curFigure->row * glassSqSize);

  for (std::list<Figure>::iterator it = glassFigures.begin(); it != glassFigures.end(); ++it)
    it->drawGlow(Globals::glassPos.x + it->col * glassSqSize, Globals::glassPos.y - it->row * glassSqSize);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  assert(!checkGlErrors());
}

void Game::step()
{
  float curTime = getTime();

  static float lastMoveTime = getTime();
  const float moveTime = 0.05f;

  if (keyState & kmLeft)
  {
    if (checkPos(-1, 0))
      curFigure->col--;

    keyState &= ~kmLeft;
  }

  if (keyState & kmRight)
  {
    if (checkPos(1, 0))
      curFigure->col++;

    keyState &= ~kmRight;
  }

  if (keyState & kmRotLeft)
  {
    curFigure->rotate(Figure::rotLeft);

    if (!validateRotation())
      curFigure->rotate(Figure::rotRight);
    else
      curFigure->buildMeshes();

    keyState &= ~kmRotLeft;
  }

  if (keyState & kmRotRight)
  {
    curFigure->rotate(Figure::rotRight);

    if (!validateRotation())
      curFigure->rotate(Figure::rotLeft);
    else
      curFigure->buildMeshes();

    keyState &= ~kmRotRight;
  }

  if (keyState & kmDrop)
  {
    drop();

    keyState &= ~kmDrop;
  }

  static float lastStepTime = getTime();
  const float stepTime = maxStepTime - (maxStepTime - minStepTime) * level / maxLevel;

  if (curTime > lastStepTime + stepTime)
  {
    if (checkPos(0, 1))
      curFigure->row++;
    else
      nextFigure();

    lastStepTime = curTime;
  }

}

void Game::drop()
{
  int y0 = curFigure->row;

  do curFigure->row++;
  while (checkPos(0, 1));
}

void Game::nextFigure()
{
  glassFigures.splice(glassFigures.end(), nextFigures, nextFigures.begin());
  nextFigures.emplace(nextFigures.end());
  curFigure = &nextFigures.front();
  curFigure->col = glassWidth / 2 - curFigure->dim / 2;

  for (int i = 0; i < curFigure->dim * curFigure->dim; i++)
  if (curFigure->data[i])
  {
    curFigure->row = -i / curFigure->dim;
    break;
  }

  if (!checkPos(0, 0))
    gameState = gsGameOver;
}

bool Game::validateRotation()
{
  if (checkPos(0, 0))
    return true;

  const int shift = curFigure->dim / 2 + (curFigure->dim & 1);

  for (int dx = 1; dx < shift; dx++)
  {
    if (checkPos(dx, 0))
    {
      curFigure->col += dx;
      return true;
    }

    if (checkPos(-dx, 0))
    {
      curFigure->col -= dx;
      return true;
    }
  }

  return false;
}

bool Game::checkPos(int dx, int dy)
{
  for (int curx = 0; curx < curFigure->dim; curx++)
  for (int cury = 0; cury < curFigure->dim; cury++)
  {
    if (curFigure->data[curx + cury * curFigure->dim])
    {
      if (curFigure->col + curx + dx < 0 ||
        curFigure->col + curx + dx >= glassWidth ||
        curFigure->row + cury + dy < 0 ||
        curFigure->row + cury + dy >= glassHeight)
        return false;
    }
  }

  for (std::list<Figure>::iterator it = glassFigures.begin();
    it != glassFigures.end();
    ++it)
  {
    if (it->col >= curFigure->col + dx + curFigure->dim)
      continue;
    if (it->col + it->dim <= curFigure->col + dx)
      continue;
    if (it->row >= curFigure->row + dy + curFigure->dim)
      continue;
    if (it->row + it->dim <= curFigure->row + dy)
      continue;

    for (int itx = 0; itx < it->dim; itx++)
    for (int ity = 0; ity < it->dim; ity++)
    {
      if (it->data[itx + ity * it->dim])
      {
        for (int curx = 0; curx < curFigure->dim; curx++)
        for (int cury = 0; cury < curFigure->dim; cury++)
        {
          if (curFigure->data[curx + cury * curFigure->dim])
          {
            if (it->col + itx == curFigure->col + curx + dx && it->row + ity == curFigure->row + cury + dy)
              return false;
          }
        }
      }
    }
  }

  return true;
}
