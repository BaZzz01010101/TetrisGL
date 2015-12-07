#pragma once

class MenuLogic
{
public:
  enum State { stHidden, stShowing, stVisible, stHiding };
  enum Result { resNone, resNewGame, resSettings, resLeaderboard, resQuitConfirmation, resQuit, resContinue, resRestartConfirmation, resRestart, resExitToMainConfirmation, resExitToMain, resSave, resDontSave, resBack };

  State state;
  Result result;
  float transitionProgress;
  int rowCount;
  int selectedRow;

  MenuLogic(Result escapeResult);
  ~MenuLogic();

  Result update();
  void add(char * caption, Result result, bool isDefault = false);
  const char * getText(int row) const;
  void next();
  void prior();
  void select(int row);
  void enter();
  void enter(int row);
  void escape();

private:
  class Item;
  std::vector<Item> itemList;
  int defaultRow;
  Result escapeResult;
};

class MenuLogic::Item
{
public:
  const char * caption;
  Result result;
  Item(const char * caption, Result result) : caption(caption), result(result) {};
};
