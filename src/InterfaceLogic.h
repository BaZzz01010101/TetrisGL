#pragma once
#include "MenuLogic.h"

class InterfaceLogic
{
public:
  enum State { stHidden, stMainMenu, stInGameMenu, stSettings, stLeaderboard, stQuitConfirmation, stRestartConfirmation, stExitToMainConfirmation };
  enum Result { resNone, resNewGame, resContinueGame, resStopGame, resCloseApp };
  MenuLogic mainMenu;
  MenuLogic inGameMenu;
  MenuLogic quitConfirmationMenu;
  MenuLogic restartConfirmationMenu;
  MenuLogic exitToMainConfirmationMenu;
  MenuLogic saveSettingsMenu;
  ReadOnly<State, InterfaceLogic> state;
  ReadOnly<float, InterfaceLogic> menuShadeProgress;

  InterfaceLogic();
  ~InterfaceLogic();

  Result update();
  void showMainMenu();
  void showInGameMenu();

private:
  std::vector<State> statesStack;
  State prevState;
  Result result;

  void mainMenuUpdate();
  void inGameMenuUpdate();
  void settingsUpdate();
  void leaderboardUpdate();
  void quitConfirmationUpdate();
  void restartConfirmationUpdate();
  void exitToMainConfirmationUpdate();
  State goNextState(State nextState);
  State goPreviousState();
  void closeInterface(Result result);
  void exitToMainMenu();
};

