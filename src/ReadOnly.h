#pragma once

template <typename T, class C>
class ReadOnly
{
friend C;

private:
  T val;
  ReadOnly & operator =(const T & rval) { val = rval; return *this; }
  ReadOnly & operator =(const ReadOnly & rval) { val = rval.val; return *this; }

  ReadOnly operator ++(int) { return ReadOnly<T, C>(val++); }
  ReadOnly & operator ++() { ++this->val; return *this; }
  ReadOnly operator --(int) { return ReadOnly<T, C>(val--); }
  ReadOnly & operator --() { --this->val; return *this; }
  ReadOnly & operator +=(const T & rval) { this->val += rval; return *this; }
  ReadOnly & operator -=(const T & rval) { this->val -= rval; return *this; }

public:
  inline operator const T&() const { return val; }
  inline ReadOnly() {};
  ReadOnly(const T & val) : val(val) {};
};

