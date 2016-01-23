#include "static_headers.h"

#include "Logic.h"

Logic::Result Logic::result = resNone;

void Logic::init()
{
  InterfaceLogic::init();
  GameLogic::init();
}


void Logic::update()
{
  switch (GameLogic::update())
  {
    case GameLogic::resGameOver:

      if (InterfaceLogic::leaderboardLogic.addResult(GameLogic::curLevel, GameLogic::curScore))
        InterfaceLogic::showLeaderboard();
      else
        InterfaceLogic::showMainMenu();

      break;

    case GameLogic::resNone:
      break;
    default:
      assert(0);
      break;
  }

  switch (InterfaceLogic::update())
  {
    case InterfaceLogic::resNewGame:
      GameLogic::newGame();
      break;
    case InterfaceLogic::resContinueGame:
      GameLogic::continueGame();
      break;
    case InterfaceLogic::resStopGame:
      GameLogic::stopGame();
      break;
    case InterfaceLogic::resCloseApp:
      result = resExitApp;
      break;
    case InterfaceLogic::resNone:
      break;
    default: 
      assert(0);
      break;
  }
}
