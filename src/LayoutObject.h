#pragma once

class LayoutObject
{
private:
  typedef std::map<std::string, LayoutObject> ChildMap;
  typedef ChildMap::iterator ChildIterator;
  const LayoutObject * parent;
  ChildMap childList;

public:
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
  float getGlobalLeft() const;
  float getGlobalTop() const;
  LayoutObject * getObjectFromPoint(float x, float y);
};
