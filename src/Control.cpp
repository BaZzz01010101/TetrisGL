#include "static_headers.h"

#include "Control.h"
#include "Crosy.h"
#include "Layout.h"


Control::Control(GameLogic & gameLogic, InterfaceLogic & interfaceLogic) :
  gameLogic(gameLogic),
  interfaceLogic(interfaceLogic),
  freq(Crosy::getPerformanceFrequency()),
  repeatDelay(uint64_t(0.2 * freq)),
  repeatInterval(uint64_t(freq / 30))
{
  assert(freq);
  memset(internalKeyStates, 0, sizeof(internalKeyStates));
}


Control::~Control()
{
}

void Control::keyDown(Key key)
{
  KeyInternalState & internalKeyState = internalKeyStates[key];
  internalKeyState.keyNextRepeatCounter = currentCounter/*Crosy::getPerformanceCounter()*/ + repeatDelay;
  internalKeyState.pressCount++;
  internalKeyState.wasChanged = true;
}

void Control::keyUp(Key key)
{
  KeyInternalState & internalKeyState = internalKeyStates[key];
  internalKeyState.keyNextRepeatCounter = 0;
  internalKeyState.wasChanged = true;
}

void Control::mouseMove(float x, float y)
{
  mouseX = x;
  mouseY = y;
}

void Control::mouseDown()
{
  if (gameLogic.state == GameLogic::stPlaying)
  {
    LayoutObject * mouseOverObject = Layout::gameLayout.getObjectFromPoint(mouseX, mouseY);

    if (mouseOverObject && mouseOverObject->name == "ScoreBarMenuButton")
    {
      gameLogic.pauseGame();
      interfaceLogic.showInGameMenu();
    }
  }
}

void Control::update()
{
  currentCounter = Crosy::getPerformanceCounter();
  updateKeyboard();
}

Control::KeyState Control::getKeyState(Key key) const
{
  KeyState keyState;
  const KeyInternalState & keyInternalState = internalKeyStates[key];
  keyState.isPressed = (keyInternalState.keyNextRepeatCounter > 0);
  keyState.wasChanged = keyInternalState.wasChanged;
  keyState.pressCount = keyInternalState.pressCount;
  keyState.repeatCount = 
    (keyState.isPressed && currentCounter > keyInternalState.keyNextRepeatCounter && repeatInterval) ?
    int((currentCounter - keyInternalState.keyNextRepeatCounter) / repeatInterval + 1) : 0;

  return keyState;
}

void Control::updateKeyStates()
{
  for (Key key = KB_NONE; key < KEY_COUNT; key++)
  {
    KeyInternalState & keyInternalState = internalKeyStates[key];
    bool isPressed = (keyInternalState.keyNextRepeatCounter > 0);

    if (isPressed && currentCounter > keyInternalState.keyNextRepeatCounter && repeatInterval)
      keyInternalState.keyNextRepeatCounter = keyInternalState.keyNextRepeatCounter + 
      ((currentCounter - keyInternalState.keyNextRepeatCounter) / repeatInterval + 1) * repeatInterval;

    keyInternalState.pressCount = 0;
    keyInternalState.wasChanged = false;
  }
}

void Control::updateKeyboard()
{
  switch (interfaceLogic.state)
  {
  case InterfaceLogic::stHidden:                  updateGameKeyboard();                                           break;
  case InterfaceLogic::stMainMenu:                updateMenuKeyboard(interfaceLogic.mainMenu);                    break;
  case InterfaceLogic::stInGameMenu:              updateMenuKeyboard(interfaceLogic.inGameMenu);                  break;
  case InterfaceLogic::stQuitConfirmation:        updateMenuKeyboard(interfaceLogic.quitConfirmationMenu);        break;
  case InterfaceLogic::stRestartConfirmation:     updateMenuKeyboard(interfaceLogic.restartConfirmationMenu);     break;
  case InterfaceLogic::stExitToMainConfirmation:  updateMenuKeyboard(interfaceLogic.exitToMainConfirmationMenu);  break;
  case InterfaceLogic::stSettings:                updateSettingsKeyboard();                                       break;
  case InterfaceLogic::stLeaderboard:             updateLeaderboardKeyboard();                                    break;
  default:                                        assert(0);                                                      break;
  }
  
  updateKeyStates();
}

void Control::updateGameKeyboard()
{
  if (gameLogic.state != GameLogic::stPlaying)
    return;

  for (Key key = KB_NONE; key < KEY_COUNT; key++)
  {
    KeyState keyState = getKeyState(key);

    if (key == KB_ESCAPE && keyState.pressCount)
    {
      gameLogic.pauseGame();
      interfaceLogic.showInGameMenu();
    }
    else
    {
      Action action = bindings.getKeyAction(key);

      if (action)
      {

        if (keyState.pressCount || keyState.repeatCount)
        for (int i = 0, cnt = keyState.pressCount + keyState.repeatCount; i < cnt; i++)
          switch (action)
        {
          case moveLeft:    gameLogic.shiftCurrentFigureLeft();   break;
          case moveRight:   gameLogic.shiftCurrentFigureRight();  break;
          case rotateLeft:  gameLogic.rotateCurrentFigureLeft();  break;
          case rotateRight: gameLogic.rotateCurrentFigureRight(); break;
          case fastDown:    gameLogic.fastDownCurrentFigure();    break;
          default: break;
        }

        if (keyState.pressCount)
        for (int i = 0, cnt = keyState.pressCount; i < cnt; i++)
          switch (action)
        {
          case dropDown: gameLogic.dropCurrentFigure(); break;
          case swapHold: gameLogic.holdCurrentFigure(); break;
          default: break;
        }
      }
    }
  }
}


void Control::updateMenuKeyboard(MenuLogic & menu)
{
  if (menu.state != MenuLogic::stVisible)
    return;

  for (Key key = KB_NONE; key < KEY_COUNT; key++)
  {
    KeyState keyState = getKeyState(key);

    if (keyState.pressCount || keyState.repeatCount)
      switch (key)
    {
      case KB_UP:     menu.prior(); break;
      case KB_DOWN:   menu.next();  break;
      case KB_ENTER:
      case KB_KP_ENTER:
      case KB_SPACE:  menu.enter(); break;
      case KB_ESCAPE: menu.escape();  break;
      default: break;
    }
  }
}

void Control::updateSettingsKeyboard()
{
  if (interfaceLogic.settingsLogic.state != SettingsLogic::stVisible)
    return;

  for (Key key = KB_NONE; key < KEY_COUNT; key++)
  {
    KeyState keyState = getKeyState(key);

    if (keyState.pressCount || keyState.repeatCount)
      switch (key)
    {
      case KB_UP:     break;
      case KB_DOWN:   break;
      case KB_ENTER:
      case KB_KP_ENTER:
      case KB_SPACE:  break;
      case KB_ESCAPE: interfaceLogic.settingsLogic.escape();  break;
      default: break;
    }
  }
}

void Control::updateLeaderboardKeyboard()
{

}