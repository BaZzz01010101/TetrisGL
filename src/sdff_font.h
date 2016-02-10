#pragma once

#include "sdff_glyph.h"

typedef unsigned int SDFF_Char;

class SDFF_Font
{
  friend class SDFF_Builder;

public:
  SDFF_Font();
  const SDFF_Glyph * getGlyph(SDFF_Char charCode) const;
  float getKerning(SDFF_Char leftChar, SDFF_Char rightChar) const;
  float falloff() { return falloff_; };
  float maxBearingY() { return maxBearingY_; };
  float maxHeight() { return maxHeight_; };
  int save(const char * fileName) const;
  int load(const char * fileName);

private:
  struct CharPair
  {
    SDFF_Char left;
    SDFF_Char right;
    bool operator <(const CharPair & val) const { return val.left < left || (val.left == left && val.right < right); }
  };

  typedef std::map<SDFF_Char, SDFF_Glyph> GlyphMap;
  typedef std::map<CharPair, float> KerningMap;

  float falloff_;
  float maxBearingY_;
  float maxHeight_;
  GlyphMap glyphs_;
  KerningMap kerning_;

  const rapidjson::Value & getJsonValue(const rapidjson::Value & source, const char * name) const;
  void getJsonValue(const rapidjson::Value & source, const char * name, float * value) const;
  void getJsonValue(const rapidjson::Value & source, const char * name, int * value) const;
};
