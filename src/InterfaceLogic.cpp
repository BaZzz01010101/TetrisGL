#include "static_headers.h"

#include "InterfaceLogic.h"
#include "Time.h"

InterfaceLogic::State InterfaceLogic::state = stMainMenu;
InterfaceLogic::State InterfaceLogic::prevState = stMainMenu;
InterfaceLogic::Result InterfaceLogic::result = resNone;
MenuLogic InterfaceLogic::mainMenu(MenuLogic::resNone);
MenuLogic InterfaceLogic::inGameMenu(MenuLogic::resContinue);
MenuLogic InterfaceLogic::quitConfirmationMenu(MenuLogic::resBack);
MenuLogic InterfaceLogic::restartConfirmationMenu(MenuLogic::resBack);
MenuLogic InterfaceLogic::exitToMainConfirmationMenu(MenuLogic::resBack);
SettingsLogic InterfaceLogic::settingsLogic;
float InterfaceLogic::menuShadeProgress = 1.0f;
std::vector<InterfaceLogic::State> InterfaceLogic::statesStack;

void InterfaceLogic::init()
{
  mainMenu.add("NEW GAME", MenuLogic::resNewGame, true);
  mainMenu.add("SETTINGS", MenuLogic::resSettings);
  mainMenu.add("LEADERBOARD", MenuLogic::resLeaderboard);
  mainMenu.add("QUIT", MenuLogic::resQuitConfirmation);

  inGameMenu.add("CONTINUE", MenuLogic::resContinue, true);
  inGameMenu.add("RESTART", MenuLogic::resRestartConfirmation);
  inGameMenu.add("SETTINGS", MenuLogic::resSettings);
  inGameMenu.add("EXIT", MenuLogic::resExitToMainConfirmation);

  quitConfirmationMenu.add("QUIT", MenuLogic::resQuit);
  quitConfirmationMenu.add("BACK", MenuLogic::resBack, true);

  restartConfirmationMenu.add("RESTART", MenuLogic::resRestart);
  restartConfirmationMenu.add("BACK", MenuLogic::resBack, true);

  exitToMainConfirmationMenu.add("EXIT", MenuLogic::resExitToMain);
  exitToMainConfirmationMenu.add("BACK", MenuLogic::resBack, true);
}

InterfaceLogic::Result InterfaceLogic::update()
{
  result = resNone;

  switch (state)
  {
  case stMainMenu:                mainMenuUpdate();               break;
  case stInGameMenu:              inGameMenuUpdate();             break;
  case stSettings:                settingsUpdate();               break;
  case stLeaderboard:             leaderboardUpdate();            break;
  case stQuitConfirmation:        quitConfirmationUpdate();       break;
  case stRestartConfirmation:     restartConfirmationUpdate();    break;
  case stExitToMainConfirmation:  exitToMainConfirmationUpdate(); break;
  case stHidden:                  break;
  default: assert(0);             break;
  }

  return result;
}

void InterfaceLogic::showMainMenu()
{
  statesStack.clear();
  prevState = state;
  state = stMainMenu;
}

void InterfaceLogic::showInGameMenu()
{
  statesStack.clear();
  prevState = state;
  state = stInGameMenu;
}

void InterfaceLogic::mainMenuUpdate()
{
  switch (mainMenu.update())
  {
  case MenuLogic::resNewGame:           closeInterface(resNewGame);       break;
  case MenuLogic::resSettings:          goNextState(stSettings);          break;
  case MenuLogic::resLeaderboard:       goNextState(stLeaderboard);       break;
  case MenuLogic::resQuitConfirmation:  goNextState(stQuitConfirmation);  break;
  default: break;
  }

  if ((mainMenu.state == MenuLogic::stShowing && prevState == stHidden) ||
  (mainMenu.state == MenuLogic::stHiding && mainMenu.result == MenuLogic::resNewGame))
    menuShadeProgress = mainMenu.transitionProgress;
  else
    menuShadeProgress = 1.0f;
}

void InterfaceLogic::inGameMenuUpdate()
{
  switch (inGameMenu.update())
  {
  case MenuLogic::resBack:
  case MenuLogic::resContinue:                closeInterface(resContinueGame);        break;
  case MenuLogic::resRestartConfirmation:     goNextState(stRestartConfirmation);     break;
  case MenuLogic::resSettings:                goNextState(stSettings);                break;
  case MenuLogic::resExitToMainConfirmation:  goNextState(stExitToMainConfirmation);  break;
  default: break;
  }

  if ((inGameMenu.state == MenuLogic::stShowing && prevState == stHidden) ||
  (inGameMenu.state == MenuLogic::stHiding && inGameMenu.result == MenuLogic::resContinue))
    menuShadeProgress = inGameMenu.transitionProgress;
  else
    menuShadeProgress = 1.0f;
}

void InterfaceLogic::settingsUpdate()
{
  if (settingsLogic.update() == SettingsLogic::resClose)
    goPreviousState();
}

void InterfaceLogic::leaderboardUpdate()
{
  goPreviousState();
}

void InterfaceLogic::quitConfirmationUpdate()
{
  switch (quitConfirmationMenu.update())
  {
  case MenuLogic::resBack: goPreviousState();           break;
  case MenuLogic::resQuit: closeInterface(resCloseApp); break;
  default: break;
  }
}

void InterfaceLogic::restartConfirmationUpdate()
{
  switch (restartConfirmationMenu.update())
  {
  case MenuLogic::resBack:    goPreviousState();          break;
  case MenuLogic::resRestart: closeInterface(resNewGame); break;
  default: break;
  }

  if (restartConfirmationMenu.state == MenuLogic::stHiding && 
  restartConfirmationMenu.result == MenuLogic::resRestart)
    menuShadeProgress = restartConfirmationMenu.transitionProgress;
  else
    menuShadeProgress = 1.0f;
}

void InterfaceLogic::exitToMainConfirmationUpdate()
{
  switch (exitToMainConfirmationMenu.update())
  {
  case MenuLogic::resBack:        goPreviousState();  break;
  case MenuLogic::resExitToMain:  exitToMainMenu();   break;
  default: break;
  }
}

InterfaceLogic::State InterfaceLogic::goNextState(InterfaceLogic::State nextState)
{
  statesStack.push_back(state);
  prevState = state;
  state = nextState;

  return statesStack.back();
}

InterfaceLogic::State InterfaceLogic::goPreviousState()
{
  assert(!statesStack.empty());

  if (!statesStack.empty())
  {
    prevState = state;
    state = statesStack.back();
    statesStack.pop_back();
  }

  return state;
}

void InterfaceLogic::closeInterface(Result result)
{
  statesStack.clear();
  prevState = state;
  state = stHidden;
  InterfaceLogic::result = result;
}

void InterfaceLogic::exitToMainMenu()
{
  statesStack.clear();
  prevState = state;
  state = stMainMenu;
  result = resStopGame;
}
