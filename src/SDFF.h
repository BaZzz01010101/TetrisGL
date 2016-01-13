#pragma once

//-------------------------------------------------------------------------------------------------

enum SDFF_Error 
{ 
  SDFF_OK = 0, 
  SDFF_FONT_ALREADY_EXISTS, 
  SDFF_CHAR_ALREADY_EXISTS, 
  SDFF_FONT_NOT_EXISTS, 
  SDFF_FT_NEW_FACE_ERROR, 
  SDFF_FT_SET_CHAR_SIZE_ERROR, 
  SDFF_FT_LOAD_CHAR_ERROR, 
  SDFF_INVALID_VALUE, 
  SDFF_NOT_INITIALIZED 
};

//-------------------------------------------------------------------------------------------------

typedef unsigned int SDFF_Char;

class SDFF_Bitmap
{
public:
  int width;
  int height;
  void resize(int width, int height);
  int savePNG(const char * fileName);
  unsigned char * data() { return pixels.data(); }
  const unsigned char & operator[](int ind) const { return pixels[ind]; }
  unsigned char & operator[](int ind) { return pixels[ind]; }
private:
  typedef std::vector<unsigned char> SDFF_PixelVector;
  SDFF_PixelVector pixels;
};

//-------------------------------------------------------------------------------------------------

class SDFF_Glyph
{
public:
  float left;
  float top;
  float right;
  float bottom;
  float bearingX;
  float bearingY;
  float advance;
  float width;
  float height;
};

//-------------------------------------------------------------------------------------------------

class SDFF_Font
{
  friend class SDFF_Builder;

public:
  float falloff;
  float maxBearingY;
  float maxHeight;

  SDFF_Font();
  const SDFF_Glyph * getGlyph(SDFF_Char charCode) const;
  float getKerning(SDFF_Char leftChar, SDFF_Char rightChar) const;
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
  GlyphMap glyphs;
  KerningMap kerning;

  const rapidjson::Value & getValue(const rapidjson::Value & source, const char * name) const;
  void getValue(const rapidjson::Value & source, const char * name, float * value) const;
  void getValue(const rapidjson::Value & source, const char * name, int * value) const;
};

//-------------------------------------------------------------------------------------------------

class SDFF_Builder
{
public:
  SDFF_Builder();
  ~SDFF_Builder();

  SDFF_Error init(int sourceFontSize, int sdfFontSize, float falloff);
  SDFF_Error addFont(const char * fileName, int faceIndex, SDFF_Font * out_font);
  SDFF_Error addChar(SDFF_Font & font, SDFF_Char charCode);
  SDFF_Error addChars(SDFF_Font & font, SDFF_Char firstCharCode, SDFF_Char lastCharCode);
  SDFF_Error addChars(SDFF_Font & font, const char * charString);
  SDFF_Error composeTexture(SDFF_Bitmap & bitmap, bool powerOfTwo);

private:

  typedef std::map<SDFF_Char, SDFF_Bitmap> CharMap;
  
  struct FontData
  {
    FT_Face ftFace;
    CharMap chars;
  };

  typedef std::map<SDFF_Font *, FontData> FontMap;
  typedef std::vector<float> DistanceFieldVector;

  FT_Library ftLibrary;
  FontMap fonts;
  int sourceFontSize; 
  int sdfFontSize;
  float falloff;
  bool initialized;
  
  float createSdf(const FT_Bitmap & ftBitmap, int falloff, DistanceFieldVector & result) const;
  float createDf(const FT_Bitmap & ftBitmap, int falloff, bool invert, DistanceFieldVector & result) const;
  void copyBitmap(const SDFF_Bitmap & srcBitmap, SDFF_Bitmap & destBitmap, int posX, int posY) const;
};