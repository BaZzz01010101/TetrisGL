#pragma once

enum LayoutObjectId;

class LayoutObject
{
private:
  typedef std::map<LayoutObjectId, LayoutObject> ChildMap;
  typedef ChildMap::iterator ChildIterator;
  const LayoutObject * parent;
  ChildMap childList;

  struct ColumnData
  {
    float left;
    float width;
  };

  struct RowData
  {
    float top;
    float height;
  };

  std::vector<RowData> rows;
  std::vector<ColumnData> columns;

public:
  struct Rect
  {
    float left;
    float top;
    float width;
    float height;
  };

  LayoutObjectId id;
  float left;
  float top;
  float width;
  float height;

  LayoutObject(LayoutObjectId id, LayoutObject * parent, float left, float top, float width, float height);
  ~LayoutObject();

  void clear();
  LayoutObject * addChild(LayoutObjectId id, float left, float top, float width, float height);
  LayoutObject * getChild(LayoutObjectId id);
  LayoutObject * getChildRecursive(LayoutObjectId id);
  void addRow(float topGap, float height);
  void addColumn(float leftGap, float width);

  LayoutObject * getObjectFromPoint(float x, float y);
  bool getCellFromPoint(float x, float y, int * cellRow, int * cellColumn) const;

  float getGlobalLeft() const;
  float getGlobalTop() const;
  float getColumnGlobalLeft(int column) const;
  float getRowGlobalTop(int row) const;
  float getColumnWidth(int column) const;
  float getRowHeight(int row) const;
  Rect getGlobalRect() const;
  Rect getCellGlobalRect(int row, int column) const;
  int getRowCount() const;
  int getColumnCount() const;
};

enum LayoutObjectId
{
  loNone = -1,
  loScreen,
  loGame,
  loMainMenu,
  loInGameMenu,
  loQuitConfirmationMenu,
  loRestartConfirmationMenu,
  loExitToMainConfirmationMenu,
  loSaveSettingsMenu,
  loSettings,
  loScoreBarCaption,
  loScoreBarValue,
  loScoreBarMenuButton,
  loGlass,
  loHoldPanelCaption,
  loNextPanelCaption,
  loHoldPanel,
  loNextPanel,
  loLevelPanelCaption,
  loLevelPanel,
  loGoalPanelCaption,
  loGoalPanel,
  loSettingsWindow,
  loSettingsTitle,
  loSettingsTitleShadow,
  loSettingsPanel,
  loSettingsBackButton,
  loVolumeTitle,
  loSoundVolume,
  loMusicVolume,
  loSoundProgressBar,
  loMusicProgressBar,
  loKeyBindingTitle,
  loKeyBindingGrid,
  loBindingMessage,
  loLeaderboard,
  loLeaderboardWindow,
  loLeaderboardTitle,
  loLeaderboardTitleShadow,
  loLeaderboardPanel,
  loLeaderboardBackButton,
};
