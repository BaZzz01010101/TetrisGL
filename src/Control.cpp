#include "static_headers.h"

#include "Control.h"
#include "Crosy.h"
#include "Layout.h"


Control::Control() :
  freq(Crosy::getPerformanceFrequency()),
  repeatDelay(uint64_t(0.2 * freq)),
  repeatInterval(uint64_t(freq / 30)),
  mouseMoved(false)
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
  internalKeyState.keyNextRepeatCounter = currentCounter + repeatDelay;
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
  mouseMoved = true;
}

void Control::mouseDown(Key key)
{
  KeyInternalState & internalKeyState = internalKeyStates[key];
  internalKeyState.keyNextRepeatCounter = currentCounter + repeatDelay;
  internalKeyState.pressCount++;
  internalKeyState.wasChanged = true;
}

void Control::mouseUp(Key key)
{
  KeyInternalState & internalKeyState = internalKeyStates[key];
  internalKeyState.keyNextRepeatCounter = 0;
  internalKeyState.wasChanged = true;
}

void Control::mouseScroll(float dx, float dy)
{
  if (dy > 0)
  {
    keyDown(MOUSE_SCROLL_UP);
    keyUp(MOUSE_SCROLL_UP);
  }
  else if (dy < 0)
  {
    keyDown(MOUSE_SCROLL_DOWN);
    keyUp(MOUSE_SCROLL_DOWN);
  }
}

void Control::update()
{
  //TODO use Timer class instead of direct getPerformanceCounter
  currentCounter = Crosy::getPerformanceCounter();

  switch (InterfaceLogic::state)
  {
  case InterfaceLogic::stHidden:                  updateGameControl();                                                                          break;
  case InterfaceLogic::stMainMenu:                updateMenuControl(InterfaceLogic::mainMenu, "MainMenu");                                      break;
  case InterfaceLogic::stInGameMenu:              updateMenuControl(InterfaceLogic::inGameMenu, "InGameMenu");                                  break;
  case InterfaceLogic::stQuitConfirmation:        updateMenuControl(InterfaceLogic::quitConfirmationMenu, "QuitConfirmationMenu");              break;
  case InterfaceLogic::stRestartConfirmation:     updateMenuControl(InterfaceLogic::restartConfirmationMenu, "RestartConfirmationMenu");        break;
  case InterfaceLogic::stExitToMainConfirmation:  updateMenuControl(InterfaceLogic::exitToMainConfirmationMenu, "ExitToMainConfirmationMenu");  break;
  case InterfaceLogic::stSettings:                updateSettingsControl();                                                                      break;
  case InterfaceLogic::stLeaderboard:             updateLeaderboardControl();                                                                   break;
  default:                                        assert(0);                                                                                    break;
  }

  updateInternalState();
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

void Control::updateInternalState()
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

  mouseMoved = false;
}

void Control::updateGameControl()
{
  if (GameLogic::state != GameLogic::stPlaying)
    return;

  KeyState leftButtonState = getKeyState(MOUSE_LEFT);

  if (mouseMoved || leftButtonState.isPressed)
  {
    GameLogic::menuButtonHighlighted = false;

    if (LayoutObject * gameLayout = Layout::screen.getChild("Game"))
    {
      if (LayoutObject * mouseoverObject = gameLayout->getObjectFromPoint(mouseX, mouseY))
      {
        if (mouseoverObject->name == "ScoreBarMenuButton")
        {
          if (leftButtonState.isPressed)
          {
            GameLogic::pauseGame();
            InterfaceLogic::showInGameMenu();
          }
          else
            GameLogic::menuButtonHighlighted = true;
        }
      }
    }
  }
  
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
      Binding::Action action = Binding::getKeyAction(key);

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


void Control::updateMenuControl(MenuLogic & menu, const char * layoutName)
{
  if (menu.state != MenuLogic::stVisible)
    return;

  KeyState leftButtonState = getKeyState(MOUSE_LEFT);

  if (mouseMoved || leftButtonState.isPressed)
  {
    if (LayoutObject * mainMenuLayout = Layout::screen.getChild(layoutName))
    {
      int row, col;

      if (menu.state == MenuLogic::stVisible && mainMenuLayout->getCellFromPoint(mouseX, mouseY, &row, &col))
      {
        if (leftButtonState.isPressed)
          menu.enter(row);
        else
          menu.select(row);
      }
    }
  }

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

void Control::updateSettingsControl()
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

void Control::updateLeaderboardControl()
{

}
