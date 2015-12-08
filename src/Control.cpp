#include "static_headers.h"

#include "Control.h"
#include "Crosy.h"
#include "Layout.h"


Control::Control() :
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

  if (InterfaceLogic::state != InterfaceLogic::stHidden)
  {
    int row, col;

    switch (InterfaceLogic::state)
    {
    case InterfaceLogic::stMainMenu:
      if (InterfaceLogic::mainMenu.state == MenuLogic::stVisible &&
      Layout::mainMenuLayout.getCellFromPoint(x, y, &row, &col))
        InterfaceLogic::mainMenu.select(row);
      break;
    default:
      break;
    }
  }
}

void Control::mouseDown()
{
  if (GameLogic::state == GameLogic::stPlaying)
  {
    LayoutObject * mouseOverObject = Layout::gameLayout.getObjectFromPoint(mouseX, mouseY);

    if (mouseOverObject && mouseOverObject->name == "ScoreBarMenuButton")
    {
      GameLogic::pauseGame();
      InterfaceLogic::showInGameMenu();
    }
  }

  if (InterfaceLogic::state != InterfaceLogic::stHidden)
  {
    int row, col;

    switch (InterfaceLogic::state)
    {
    case InterfaceLogic::stMainMenu:
      if (InterfaceLogic::mainMenu.state == MenuLogic::stVisible && 
      Layout::mainMenuLayout.getCellFromPoint(mouseX, mouseY, &row, &col))
        InterfaceLogic::mainMenu.enter(row);
      break;
    default:
      break;
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
  switch (InterfaceLogic::state)
  {
  case InterfaceLogic::stHidden:                  updateGameKeyboard();                                           break;
  case InterfaceLogic::stMainMenu:                updateMenuKeyboard(InterfaceLogic::mainMenu);                    break;
  case InterfaceLogic::stInGameMenu:              updateMenuKeyboard(InterfaceLogic::inGameMenu);                  break;
  case InterfaceLogic::stQuitConfirmation:        updateMenuKeyboard(InterfaceLogic::quitConfirmationMenu);        break;
  case InterfaceLogic::stRestartConfirmation:     updateMenuKeyboard(InterfaceLogic::restartConfirmationMenu);     break;
  case InterfaceLogic::stExitToMainConfirmation:  updateMenuKeyboard(InterfaceLogic::exitToMainConfirmationMenu);  break;
  case InterfaceLogic::stSettings:                updateSettingsKeyboard();                                       break;
  case InterfaceLogic::stLeaderboard:             updateLeaderboardKeyboard();                                    break;
  default:                                        assert(0);                                                      break;
  }
  
  updateKeyStates();
}

void Control::updateGameKeyboard()
{
  if (GameLogic::state != GameLogic::stPlaying)
    return;

  for (Key key = KB_NONE; key < KEY_COUNT; key++)
  {
    KeyState keyState = getKeyState(key);

    if (key == KB_ESCAPE && keyState.pressCount)
    {
      GameLogic::pauseGame();
      InterfaceLogic::showInGameMenu();
    }
    else
    {
      Binding::Action action = bindings.getKeyAction(key);

      if (action != Binding::doNothing)
      {

        if (keyState.pressCount || keyState.repeatCount)
        for (int i = 0, cnt = keyState.pressCount + keyState.repeatCount; i < cnt; i++)
          switch (action)
        {
          case Binding::moveLeft:    GameLogic::shiftCurrentFigureLeft();   break;
          case Binding::moveRight:   GameLogic::shiftCurrentFigureRight();  break;
          case Binding::rotateLeft:  GameLogic::rotateCurrentFigureLeft();  break;
          case Binding::rotateRight: GameLogic::rotateCurrentFigureRight(); break;
          case Binding::fastDown:    GameLogic::fastDownCurrentFigure();    break;
          default: break;
        }

        if (keyState.pressCount)
        for (int i = 0, cnt = keyState.pressCount; i < cnt; i++)
          switch (action)
        {
          case Binding::dropDown: GameLogic::dropCurrentFigure(); break;
          case Binding::swapHold: GameLogic::holdCurrentFigure(); break;
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
  if (InterfaceLogic::settingsLogic.state != SettingsLogic::stVisible)
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
      case KB_ESCAPE: InterfaceLogic::settingsLogic.escape();  break;
      default: break;
    }
  }
}

void Control::updateLeaderboardKeyboard()
{

}
