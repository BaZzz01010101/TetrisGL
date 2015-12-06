#pragma once

class LayoutObject
{
private:
  typedef std::map<std::string, LayoutObject> ChildMap;
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

  std::string name;
  ReadOnly<float, LayoutObject> left;
  ReadOnly<float, LayoutObject> top;
  ReadOnly<float, LayoutObject> width;
  ReadOnly<float, LayoutObject> height;

  LayoutObject(const char * name, LayoutObject * parent, float left, float top, float width, float height);
  ~LayoutObject();

  void clear();
  LayoutObject & addChild(const char * name, float left, float top, float width, float height);
  LayoutObject * getChild(const char * name);
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
  int getRowCount();
  int getColumnCount();
};
