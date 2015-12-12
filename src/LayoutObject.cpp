#include "static_headers.h"

#include "LayoutObject.h"

LayoutObject::LayoutObject(const char * name, LayoutObject * parent, float left, float top, float width, float height) :
  name(name),
  parent(parent),
  left(left),
  top(top),
  width(width),
  height(height)
{
}

LayoutObject::~LayoutObject()
{
}

void LayoutObject::clear()
{
  childList.clear();
  rows.clear();
  columns.clear();
}

LayoutObject & LayoutObject::addChild(const char * name, float left, float top, float width, float height)
{
  ChildIterator childIt = childList.emplace(name, LayoutObject(name, this, left, top, width, height)).first;
  return childIt->second;
}

LayoutObject * LayoutObject::getChild(const char * name)
{
  ChildIterator childIt = childList.find(name);
  assert(childIt != childList.end());

  if (childIt == childList.end())
    return NULL;
  
  return &childIt->second;
}

void LayoutObject::addRow(float topGap, float height)
{
  const float top = rows.empty() ? 0.0f : rows.back().top + rows.back().height;
  rows.emplace_back();
  rows.back().top = top + topGap;
  rows.back().height = height;
}

void LayoutObject::addColumn(float leftGap, float width)
{
  const float left = columns.empty() ? 0.0f : columns.back().left + columns.back().width;
  columns.emplace_back();
  columns.back().left = left + leftGap;
  columns.back().width = width;
}

LayoutObject * LayoutObject::getObjectFromPoint(float x, float y) 
{
  for (ChildIterator childIt = childList.begin(); childIt != childList.end(); ++childIt)
  {
    LayoutObject * object = childIt->second.getObjectFromPoint(x, y);

    if (object)
      return object;
  }

  const float left = getGlobalLeft();
  const float top = getGlobalTop();
  const float right = left + width;
  const float bottom = top - height;

  if (x >= left && x <= right && y <= top && y >= bottom)
    return this;

  return NULL;
}

bool LayoutObject::getCellFromPoint(float x, float y, int * cellRow, int * cellColumn) const 
{
  const float clientX = x - getGlobalLeft();
  const float clientY = getGlobalTop() - y;

  if (clientX < 0 || clientX > width || clientY < 0 || clientY > height)
    return false;

  int row = 0;
  int rowCount = rows.size();

  while(row < rowCount)
  {
    const RowData & rowData = rows[row];

    if (clientY >= rowData.top && clientY <= rowData.top + rowData.height)
      break;

    ++row;
  }

  if (row == rowCount)
    return false;

  int column = 0;
  int columnCount = columns.size();

  while (column < columnCount)
  {
    const ColumnData & columnData = columns[column];

    if (clientX >= columnData.left && clientX <= columnData.left + columnData.width)
      break;

    ++column;
  }

  if (column == columnCount)
    return false;

  if (cellRow)
    *cellRow = row;

  if (cellColumn)
    *cellColumn = column;

  return true;
}

float LayoutObject::getGlobalLeft() const
{
  return parent ? parent->getGlobalLeft() + left : left;
}

float LayoutObject::getGlobalTop() const
{
  return parent ? parent->getGlobalTop() + top : top;
}

float LayoutObject::getColumnGlobalLeft(int column) const
{
  return getGlobalLeft() + columns[column].left;
}

float LayoutObject::getRowGlobalTop(int row) const
{
  return getGlobalTop() - rows[row].top;
}

float LayoutObject::getColumnWidth(int column) const
{
  return columns[column].width;
}

float LayoutObject::getRowHeight(int row) const
{
  return rows[row].height;
}

LayoutObject::Rect LayoutObject::getGlobalRect() const
{
  return { getGlobalLeft(), getGlobalTop(), width, height };
}

LayoutObject::Rect LayoutObject::getCellGlobalRect(int row, int column) const
{
  const ColumnData & columnData = columns[column];
  const RowData & rowData = rows[row];

  return{ getGlobalLeft() + columnData.left, getGlobalTop() - rowData.top, columnData.width, rowData.height };
}

int LayoutObject::getRowCount()
{
  return rows.size();
}

int LayoutObject::getColumnCount()
{
  return columns.size();
}
