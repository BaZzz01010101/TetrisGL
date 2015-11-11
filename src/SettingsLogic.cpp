#include "static_headers.h"

#include "SettingsLogic.h"
#include "Globals.h"
#include "Time.h"


SettingsLogic::SettingsLogic() :
  changed(false),
  state(stHidden),
  result(resNone)
{
}


SettingsLogic::~SettingsLogic()
{
}

SettingsLogic::Result SettingsLogic::update()
{
  Result retResult = resNone;

  switch (state)
  {
  case stHidden:
    result = resNone;
    state = stShowing;
    break;

  case stShowing:
    if ((transitionProgress += Time::timerDelta / Globals::settingsShowingTime) >= 1.0f)
    {
      transitionProgress = 1.0f;
      state = stVisible;
    }

    break;

  case stVisible:
    break;

  case stHiding:
    if ((transitionProgress -= Time::timerDelta / Globals::settingsHidingTime) <= 0.0f)
    {
      transitionProgress = 0.0f;
      state = stHidden;
      retResult = result;
    }

    break;

  default:
    assert(0);
    break;
  }

  return retResult;
}

void SettingsLogic::escape()
{
  assert(state == stVisible);

  if (changed)
  {
    result = resSaveConfirmation;
    state = stSaveConfirmation;
  }
  else
  {
    result = resClose;
    state = stHiding;
  }
}
