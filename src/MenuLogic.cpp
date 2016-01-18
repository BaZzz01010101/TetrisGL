#include "static_headers.h"

#include "MenuLogic.h"
#include "Globals.h"
#include "Time.h"

MenuLogic::MenuLogic(Result escapeResult) :
  state(stHidden),
  result(resNone),
  escapeResult(escapeResult),
  defaultRow(0),
  selectedRow(0),
  showingTime(0.4f),
  hidingTime(0.3f),
  transitionProgress(0.0f)
{
  assert(showingTime > VERY_SMALL_NUMBER);
  assert(hidingTime > VERY_SMALL_NUMBER);
}


MenuLogic::~MenuLogic()
{
}

MenuLogic::Result MenuLogic::update()
{
  Result retResult = resNone;

  switch (state)
  {
  case stHidden:
    result = resNone;
    state = stShowing;
    selectedRow = defaultRow;
    break;

  case stShowing:
    if ((transitionProgress += Time::timerDelta / showingTime) >= 1.0f)
    {
      transitionProgress = 1.0f;
      state = stVisible;
    }

    break;

  case stVisible:
    break;

  case stHiding:
    if ((transitionProgress -= Time::timerDelta / hidingTime) <= 0.0f)
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

void MenuLogic::add(char * caption, MenuLogic::Result result, bool isDefault)
{
  itemList.push_back(Item(caption, result));

  if (isDefault)
    defaultRow = rowCount;

  ++rowCount;
}

const char * MenuLogic::getText(int row) const
{
  assert(row >= 0);
  assert(row < (int)itemList.size());

  return itemList[glm::clamp<int>(row, 0, rowCount - 1)].caption;
}

void MenuLogic::next()
{
  assert(state == stVisible);

  ++selectedRow;

  if (selectedRow >= rowCount)
    selectedRow = 0;
}

void MenuLogic::prior()
{
  assert(state == stVisible);

  --selectedRow;

  if (selectedRow < 0)
    selectedRow = rowCount - 1;
}

void MenuLogic::select(int row)
{
  assert(state == stVisible);
  assert(row >= 0);
  assert(row < (int)itemList.size());

  selectedRow = glm::clamp<int>(row, 0, rowCount - 1);
}

void MenuLogic::enter()
{
  assert(state == stVisible);
  assert(itemList.size() > 0);

  result = itemList[selectedRow].result;
  state = stHiding;
}

void MenuLogic::enter(int row)
{
  assert(state == stVisible);
  assert(row >= 0);
  assert(row < (int)itemList.size());

  select(row);
  enter();
}

void MenuLogic::escape()
{
  assert(state == stVisible);

  result = escapeResult;

  if (escapeResult != resNone)
    state = stHiding;
}

