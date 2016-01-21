#include "static_headers.h"

#include "Control.h"
#include "Crosy.h"
#include "Layout.h"
#include "Time.h"


Control::Control() :
  mouseMoved(false),
  mouseDoubleClicked(false),
  fastDownBlocked(false)
{
  repeatDelay = glm::max<uint64_t>(0.2 * Time::freq, 1);
  repeatInterval = glm::max<uint64_t>(Time::freq / 30, 1);
}


Control::~Control()
{
}

void Control::init()
{
  memset(internalKeyStates, 0, sizeof(internalKeyStates));
}

void Control::keyDown(Key key)
{
  KeyInternalState & internalKeyState = internalKeyStates[key];
  internalKeyState.keyNextRepeatCounter = Time::counter + repeatDelay;
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
  internalKeyState.keyNextRepeatCounter = Time::counter + repeatDelay;
  internalKeyState.pressCount++;
  internalKeyState.wasChanged = true;
  const double doubleclickTime = 0.4;
  const float doubleclickRange = 0.005f;
  mouseDoubleClicked = (Time::timer - lastLButtonClickTimer < doubleclickTime &&
    glm::length(glm::vec2(mouseX, mouseY) - lastLButtonClickPos) < doubleclickRange);

  if (!mouseDoubleClicked)
  {
    lastLButtonClickTimer = Time::timer;
    lastLButtonClickPos = glm::vec2(mouseX, mouseY);
  }
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
  switch (InterfaceLogic::state)
  {
  case InterfaceLogic::stHidden:                  updateGameControl();                                                                          break;
  case InterfaceLogic::stMainMenu:                updateMenuControl(InterfaceLogic::mainMenu, loMainMenu);                                      break;
  case InterfaceLogic::stInGameMenu:              updateMenuControl(InterfaceLogic::inGameMenu, loInGameMenu);                                  break;
  case InterfaceLogic::stQuitConfirmation:        updateMenuControl(InterfaceLogic::quitConfirmationMenu, loQuitConfirmationMenu);              break;
  case InterfaceLogic::stRestartConfirmation:     updateMenuControl(InterfaceLogic::restartConfirmationMenu, loRestartConfirmationMenu);        break;
  case InterfaceLogic::stExitToMainConfirmation:  updateMenuControl(InterfaceLogic::exitToMainConfirmationMenu, loExitToMainConfirmationMenu);  break;

  case InterfaceLogic::stSettings:
    switch (InterfaceLogic::settingsLogic.state)
    {
    case SettingsLogic::stSaveConfirmation:       updateMenuControl(InterfaceLogic::settingsLogic.saveConfirmationMenu, loSaveSettingsMenu);    break;
    case SettingsLogic::stVisible:                updateSettingsControl();                                                                      break;
    case SettingsLogic::stKeyWaiting:             updateSettingsKeyBindControl();                                                               break;
    default: break;
    }
    break;

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
    (keyState.isPressed && Time::counter > keyInternalState.keyNextRepeatCounter && repeatInterval) ?
    int((Time::counter - keyInternalState.keyNextRepeatCounter) / repeatInterval + 1) : 0;

  return keyState;
}

void Control::updateInternalState()
{
  for (Key key = FIRST_KEY; key < KEY_COUNT; key++)
  {
    KeyInternalState & keyInternalState = internalKeyStates[key];
    bool isPressed = (keyInternalState.keyNextRepeatCounter > 0);

    if (isPressed && Time::counter > keyInternalState.keyNextRepeatCounter && repeatInterval)
      keyInternalState.keyNextRepeatCounter = keyInternalState.keyNextRepeatCounter + 
      ((Time::counter - keyInternalState.keyNextRepeatCounter) / repeatInterval + 1) * repeatInterval;

    keyInternalState.pressCount = 0;
    keyInternalState.wasChanged = false;
  }

  mouseMoved = false;
  mouseDoubleClicked = false;
}

void Control::updateGameControl()
{
  if (GameLogic::state != GameLogic::stPlaying)
    return;

  KeyState leftButtonState = getKeyState(MOUSE_LEFT);

  if (mouseMoved || leftButtonState.isPressed)
  {
    GameLogic::menuButtonHighlighted = false;

    if (LayoutObject * gameLayout = Layout::screen.getChild(loGame))
    {
      if (LayoutObject * mouseoverObject = gameLayout->getObjectFromPoint(mouseX, mouseY))
      {
        if (mouseoverObject->id == loScoreBarMenuButton)
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
  
  for (Key key = FIRST_KEY; key < KEY_COUNT; key++)
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

      if (!GameLogic::haveFallingRows && action != Binding::doNothing)
      {
        if (keyState.pressCount || keyState.repeatCount)
        for (int i = 0, cnt = keyState.pressCount + keyState.repeatCount; i < cnt; i++)
          switch (action)
        {
          case Binding::moveLeft:    GameLogic::shiftCurrentFigureLeft();   break;
          case Binding::moveRight:   GameLogic::shiftCurrentFigureRight();  break;
          case Binding::rotateLeft:  GameLogic::rotateCurrentFigureLeft();  break;
          case Binding::rotateRight: GameLogic::rotateCurrentFigureRight(); break;
          case Binding::fastDown:    
            if (!fastDownBlocked)
              fastDownBlocked = GameLogic::fastDownCurrentFigure();    
            break;
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

      if (action == Binding::fastDown && !keyState.isPressed)
        fastDownBlocked = false;
    }
  }
}


void Control::updateMenuControl(MenuLogic & menu, LayoutObjectId layoutObjectId)
{
  if (menu.state != MenuLogic::stVisible)
    return;

  KeyState leftButtonState = getKeyState(MOUSE_LEFT);

  if (LayoutObject * mainMenuLayout = Layout::screen.getChild(layoutObjectId))
  {
    int row, col;

    if (menu.state == MenuLogic::stVisible && mainMenuLayout->getCellFromPoint(mouseX, mouseY, &row, &col))
    {
      if (leftButtonState.isPressed)
        menu.enter(row);
      else if (mouseMoved)
        menu.select(row);
    }
  }

  for (Key key = FIRST_KEY; key < KEY_COUNT; key++)
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
  SettingsLogic & settingsLogic = InterfaceLogic::settingsLogic;

  if (LayoutObject * settingsLayout = Layout::screen.getChild(loSettings))
  {
    KeyState mouseLButtonState = getKeyState(MOUSE_LEFT);
    LayoutObject * mouseoverObject = settingsLayout->getObjectFromPoint(mouseX, mouseY);
    bool rowHighlighed = (mouseoverObject != NULL);
    bool rowClicked = mouseLButtonState.isPressed && mouseLButtonState.wasChanged && mouseoverObject;
    bool rowDoubleclicked = rowClicked && mouseDoubleClicked;

    if (rowClicked)
    {
      if (mouseoverObject->id == loSoundProgressBar || mouseoverObject->id == loMusicProgressBar)
        draggedProgressBarId = mouseoverObject->id;
    }
    else if (!mouseLButtonState.isPressed)
      draggedProgressBarId = loNone;

    if (rowHighlighed)
    {
      int row, col;
      settingsLogic.backButtonHighlighted = false;
      settingsLogic.highlightedControl = SettingsLogic::ctrlNone;

      switch (mouseoverObject->id)
      {
      case loSettingsBackButton:
        settingsLogic.backButtonHighlighted = true;
        break;

      case loSoundVolume:
      case loSoundProgressBar:
        settingsLogic.highlightedControl = SettingsLogic::ctrlSoundVolume;
        break;

      case loMusicVolume:
      case loMusicProgressBar:
        settingsLogic.highlightedControl = SettingsLogic::ctrlMusicVolume;
        break;

      case loKeyBindingGrid:
        settingsLogic.highlightedControl = SettingsLogic::ctrlKeyBindTable;

        if (mouseoverObject->getCellFromPoint(mouseX, mouseY, &row, &col))
          settingsLogic.highlightedAction = (Binding::Action)row;
        break;

      default:
        break;
      }
    }

    if (rowClicked)
    {
      int row, col;

      switch (mouseoverObject->id)
      {
      case loSettingsBackButton:
        settingsLogic.escape();
        break;

      case loSoundVolume:
        settingsLogic.selectedControl = SettingsLogic::ctrlSoundVolume;
        break;

      case loSoundProgressBar:
        settingsLogic.selectedControl = SettingsLogic::ctrlSoundVolume;
        break;

      case loMusicVolume:
        settingsLogic.selectedControl = SettingsLogic::ctrlMusicVolume;
        break;

      case loMusicProgressBar:
        settingsLogic.selectedControl = SettingsLogic::ctrlMusicVolume;
        break;

      case loKeyBindingGrid:
        settingsLogic.selectedControl = SettingsLogic::ctrlKeyBindTable;

        if (mouseoverObject->getCellFromPoint(mouseX, mouseY, &row, &col))
          settingsLogic.selectedAction = (Binding::Action)row;
        break;

      default:
        break;
      }
    }

    if (rowDoubleclicked && mouseoverObject->id == loKeyBindingGrid)
      settingsLogic.state = SettingsLogic::stKeyWaiting;

    if (draggedProgressBarId != loNone)
    {
      LayoutObject * progressBarLayout = settingsLayout->getChildRecursive(draggedProgressBarId);
      const float progressBarLeft = progressBarLayout->getGlobalLeft() + Layout::settingsProgressBarBorder + Layout::settingsProgressBarInnerGap;
      const float progressBarWidth = progressBarLayout->width - 2.0f * Layout::settingsProgressBarBorder - 2.0f * Layout::settingsProgressBarInnerGap;
      const float progress = glm::clamp((mouseX - progressBarLeft) / progressBarWidth, 0.0f, 1.0f);

      switch (draggedProgressBarId)
      {
      case loSoundProgressBar:
        settingsLogic.setSoundVolume(progress);
        break;
      case loMusicProgressBar:
        settingsLogic.setMusicVolume(progress);
        break;
      default:
        assert(0);
      }
    }

    for (Key key = FIRST_KEY; key < KEY_COUNT; key++)
    {
      KeyState keyState = getKeyState(key);

      if (keyState.pressCount || keyState.repeatCount)
        switch (key)
      {
        case KB_UP:     settingsLogic.selectPrevious(); break;
        case KB_DOWN:   settingsLogic.selectNext(); break;
        case KB_LEFT:
          switch (settingsLogic.selectedControl)
          {
          case SettingsLogic::ctrlSoundVolume:
            settingsLogic.setSoundVolume(glm::clamp(settingsLogic.getSoundVolume() - 0.05f, 0.0f, 1.0f));
            break;
          case SettingsLogic::ctrlMusicVolume:
            settingsLogic.setMusicVolume(glm::clamp(settingsLogic.getMusicVolume() - 0.05f, 0.0f, 1.0f));
            break;
          default: break;
          }
          break;
        case KB_RIGHT:
          switch (settingsLogic.selectedControl)
          {
          case SettingsLogic::ctrlSoundVolume:
            settingsLogic.setSoundVolume(glm::clamp(settingsLogic.getSoundVolume() + 0.05f, 0.0f, 1.0f));
            break;
          case SettingsLogic::ctrlMusicVolume:
            settingsLogic.setMusicVolume(glm::clamp(settingsLogic.getMusicVolume() + 0.05f, 0.0f, 1.0f));
            break;
          default: break;
          }
          break;
        case KB_ENTER:
        case KB_KP_ENTER:
          if (settingsLogic.selectedControl == SettingsLogic::ctrlKeyBindTable)
            settingsLogic.state = SettingsLogic::stKeyWaiting;
          break;
        case KB_ESCAPE: settingsLogic.escape();  break;
        default: break;
      }
    }
  }
}

void Control::updateSettingsKeyBindControl()
{
  for (Key key = FIRST_KEY; key < KEY_COUNT; key++)
  {
    KeyState keyState = getKeyState(key);

    if (keyState.isPressed && keyState.wasChanged)
    {
      if (key != KB_ESCAPE)
        InterfaceLogic::settingsLogic.setCurrentActionKey(key);

      InterfaceLogic::settingsLogic.state = SettingsLogic::stVisible;
      break;
    }
  }
}

void Control::updateLeaderboardControl()
{
  LeaderboardLogic & leaderboardLogic = InterfaceLogic::leaderboardLogic;

  if (LayoutObject * leaderboardLayout = Layout::screen.getChild(loLeaderboard))
  {
    KeyState mouseLButtonState = getKeyState(MOUSE_LEFT);
    LayoutObject * mouseoverObject = leaderboardLayout->getObjectFromPoint(mouseX, mouseY);

    leaderboardLogic.backButtonHighlighted = (mouseoverObject && mouseoverObject->id == loLeaderboardBackButton && leaderboardLogic.editRow < 0);

    if (mouseLButtonState.isPressed && 
    mouseLButtonState.wasChanged && 
    mouseoverObject && 
    mouseoverObject->id == loLeaderboardBackButton && 
    leaderboardLogic.editRow < 0)
      leaderboardLogic.escape();
  }

  for (Key key = FIRST_KEY; key < KEY_COUNT; key++)
  {
    KeyState keyState = getKeyState(key);

    if (keyState.pressCount || keyState.repeatCount)
    {
      if (leaderboardLogic.editRow >= 0)
      {
      if (key == KB_BACKSPACE)
        leaderboardLogic.deleteChar();
      else if (key == KB_ENTER || key == KB_KP_ENTER)
        leaderboardLogic.commit();
      else if (key == KB_SPACE)
        leaderboardLogic.addChar(' ');
      else if (key >= KB_A && key <= KB_Z)
        leaderboardLogic.addChar('A' + key - KB_A);
      else if (key >= KB_0 && key <= KB_9)
        leaderboardLogic.addChar('0' + key - KB_0);
      }
      else if (key == KB_ESCAPE && leaderboardLogic.editRow < 0)
        leaderboardLogic.escape();
    }
  }
}

