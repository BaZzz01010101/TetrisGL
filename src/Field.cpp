#include "static_headers.h"

#include "Field.h"

bool Field::inBounds(int x, int y) const 
{ 
  return (x >= 0 && x < width && y >= 0 && y < height); 
}


Cell * Field::getCell(int x, int y)
{
  if (inBounds(x, y))
    return &cells[x + y * width];
  else
    return NULL;
}


void Field::setCell(const Cell & cell, int x, int y)
{
  assert(inBounds(x, y));

  if (inBounds(x, y))
    cells[x + y * width] = cell;
}


void Field::clear()
{
  for (int i = 0; i < width * height; ++i)
    cells[i].clear();
}


void Field::copyRow(int srcRow, int dstRow)
{
  assert(srcRow >= 0 && srcRow < height && dstRow >= 0 && dstRow < height);

  if (srcRow >= 0 && srcRow < height && dstRow >= 0 && dstRow < height)
  {
    const Cell * srcCell = cells + srcRow * width;
    const Cell * eofCell = srcCell + width;
    Cell * dstCell = cells + dstRow * width;

    while (srcCell < eofCell)
      *dstCell++ = *srcCell++;
  }
}


void Field::clearRows(int beginRow, int endRow)
{
  if (beginRow <= endRow)
  {
    Cell * cell = cells + beginRow * width;
    const Cell * eofCell = cells + (endRow + 1) * width;

    while (cell < eofCell)
      cell++->clear();
  }
}
