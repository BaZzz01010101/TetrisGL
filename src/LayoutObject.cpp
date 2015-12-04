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

float LayoutObject::getGlobalLeft() const
{
   return parent ? parent->getGlobalLeft() + left : left;
}

float LayoutObject::getGlobalTop() const
{
  return parent ? parent->getGlobalTop() + top: top;
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
