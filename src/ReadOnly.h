#pragma once

template <typename T, class C>
class ReadOnly
{
friend C;

private:
  T value;
  ReadOnly & operator =(const T & rval) { value = rval; return *this; }
  ReadOnly & operator =(const ReadOnly & rval) { value = rval.value; return *this; }

  ReadOnly operator ++(int) { return ReadOnly<T, C>(value++); }
  ReadOnly & operator ++() { ++this->value; return *this; }
  ReadOnly operator --(int) { return ReadOnly<T, C>(value--); }
  ReadOnly & operator --() { --this->value; return *this; }
  ReadOnly & operator +=(const T & rval) { this->value += rval; return *this; }
  ReadOnly & operator -=(const T & rval) { this->value -= rval; return *this; }

public:
  inline operator const T&() const { return value; }
  inline ReadOnly() {};
  ReadOnly(const T & value) : value(value) {};
};

