#pragma once
#include "MenuLogic.h"
#include "SettingsLogic.h"
#include "LeaderboardLogic.h"

class InterfaceLogic
{
public:
  enum State { stHidden, stMainMenu, stInGameMenu, stSettings, stLeaderboard, stQuitConfirmation, stRestartConfirmation, stExitToMainConfirmation};
  enum Result { resNone, resNewGame, resContinueGame, resStopGame, resCloseApp };
  static MenuLogic mainMenu;
  static MenuLogic inGameMenu;
  static MenuLogic quitConfirmationMenu;
  static MenuLogic restartConfirmationMenu;
  static MenuLogic exitToMainConfirmationMenu;
  static SettingsLogic settingsLogic;
  static LeaderboardLogic leaderboardLogic;
  static State state;
  static float menuShadeProgress;

  static void init();
  static Result update();
  static void showMainMenu();
  static void showInGameMenu();
  static void showLeaderboard();

private:
  InterfaceLogic();
  ~InterfaceLogic();

  static std::vector<State> statesStack;
  static State prevState;
  static Result result;

  static void mainMenuUpdate();
  static void inGameMenuUpdate();
  static void settingsUpdate();
  static void leaderboardUpdate();
  static void quitConfirmationUpdate();
  static void restartConfirmationUpdate();
  static void exitToMainConfirmationUpdate();
  static void saveConfirmationUpdate();
  static State goNextState(State nextState);
  static State goPreviousState();
  static void closeInterface(Result result);
  static void exitToMainMenu();
};

