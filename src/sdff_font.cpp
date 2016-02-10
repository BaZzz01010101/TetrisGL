#include "static_headers.h"

#include "sdff_font.h"

SDFF_Font::SDFF_Font() :
  falloff_(0.0f),
  maxBearingY_(0.0f),
  maxHeight_(0.0f)
{

}


const SDFF_Glyph * SDFF_Font::getGlyph(SDFF_Char charCode) const
{
  GlyphMap::const_iterator glyphIt = glyphs_.find(charCode);

  if (glyphIt != glyphs_.end())
    return &glyphIt->second;
  else
    return NULL;
}


float SDFF_Font::getKerning(SDFF_Char leftChar, SDFF_Char rightChar) const
{

  KerningMap::const_iterator kerningIt = kerning_.find({ leftChar, rightChar });

  if (kerningIt != kerning_.end())
    return kerningIt->second;
  else
    return NULL;
}


int SDFF_Font::save(const char * fileName) const
{
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

  writer.StartObject();

  writer.String("Falloff");
  writer.Double(falloff_);
  writer.String("MaxBearingY");
  writer.Double(maxBearingY_);
  writer.String("MaxHeight");
  writer.Double(maxHeight_);

  writer.String("Glyphs");
  writer.StartArray();

  for (GlyphMap::const_iterator glyphIt = glyphs_.begin(); glyphIt != glyphs_.end(); ++glyphIt)
  {
    SDFF_Char charCode = glyphIt->first;
    const SDFF_Glyph & glyph = glyphIt->second;
    writer.StartObject();
    writer.String("code");
    writer.Int(charCode);
    writer.String("left");
    writer.Double(glyph.left);
    writer.String("top");
    writer.Double(glyph.top);
    writer.String("right");
    writer.Double(glyph.right);
    writer.String("bottom");
    writer.Double(glyph.bottom);
    writer.String("bearingX");
    writer.Double(glyph.bearingX);
    writer.String("bearingY");
    writer.Double(glyph.bearingY);
    writer.String("advance");
    writer.Double(glyph.advance);
    writer.String("width");
    writer.Double(glyph.width);
    writer.String("height");
    writer.Double(glyph.height);
    writer.EndObject();
  }

  writer.EndArray();

  writer.String("Kerning");
  writer.StartArray();

  for (KerningMap::const_iterator kerningIt = kerning_.begin(); kerningIt != kerning_.end(); ++kerningIt)
  {
    SDFF_Char leftCharCode = kerningIt->first.left;
    SDFF_Char rightCharCode = kerningIt->first.right;
    writer.StartObject();
    writer.String("leftCode");
    writer.Int(leftCharCode);
    writer.String("rightCode");
    writer.Int(rightCharCode);
    writer.String("kerning");
    writer.Double(kerningIt->second);
  }

  writer.EndArray();

  writer.EndObject();

  FILE * file = fopen(fileName, "wb+");
  assert(file);

  if (file)
  {
    int success = (int)fwrite(buffer.GetString(), buffer.GetSize(), 1, file);
    assert(success);
    fclose(file);
    return success;
  }

  return 0;
}


int SDFF_Font::load(const char * fileName)
{
  rapidjson::Document doc;
  FILE * file = fopen(fileName, "rb");
  assert(file);
  const int bufSize = 16384;
  char buf[bufSize];
  rapidjson::FileReadStream frstream(file, buf, bufSize);
  doc.ParseStream<rapidjson::FileReadStream>(frstream);
  fclose(file);

  getJsonValue(doc, "Falloff", &falloff_);
  getJsonValue(doc, "MaxBearingY", &maxBearingY_);
  getJsonValue(doc, "MaxHeight", &maxHeight_);
  const rapidjson::Value & glyphArray = getJsonValue(doc, "Glyphs");
  assert(glyphArray.IsArray());

  if (glyphArray.IsArray())
  {
    for (rapidjson::Value::ConstValueIterator glyphIt = glyphArray.Begin();
    glyphIt != glyphArray.End();
      ++glyphIt)
    {
      SDFF_Char charCode;
      charCode = getJsonValue(*glyphIt, "code").GetInt();
      SDFF_Glyph & glyph = glyphs_[charCode];
      getJsonValue(*glyphIt, "left", &glyph.left);
      getJsonValue(*glyphIt, "right", &glyph.right);
      getJsonValue(*glyphIt, "top", &glyph.top);
      getJsonValue(*glyphIt, "bottom", &glyph.bottom);
      getJsonValue(*glyphIt, "bearingX", &glyph.bearingX);
      getJsonValue(*glyphIt, "bearingY", &glyph.bearingY);
      getJsonValue(*glyphIt, "advance", &glyph.advance);
      getJsonValue(*glyphIt, "width", &glyph.width);
      getJsonValue(*glyphIt, "height", &glyph.height);
    }
  }

  const rapidjson::Value & kerningArray = getJsonValue(doc, "Kerning");
  assert(kerningArray.IsArray());

  if (kerningArray.IsArray())
  {
    for (rapidjson::Value::ConstValueIterator kerningIt = kerningArray.Begin();
    kerningIt != kerningArray.End();
      ++kerningIt)
    {
      CharPair charPair;
      charPair.left = getJsonValue(*kerningIt, "leftCode").GetInt();
      charPair.right = getJsonValue(*kerningIt, "rightCode").GetInt();
      kerning_[charPair] = (float)getJsonValue(*kerningIt, "kerning").GetDouble();
    }
  }

  return 0;
}


const rapidjson::Value & SDFF_Font::getJsonValue(const rapidjson::Value & source, const char * name) const
{
  assert(source.HasMember(name));
  static rapidjson::Value emptyValue;

  if (source.HasMember(name))
    return source[name];
  else
    return emptyValue;
}


void SDFF_Font::getJsonValue(const rapidjson::Value & source, const char * name, float * value) const
{
  assert(source.HasMember(name));

  if (source.HasMember(name))
    *value = float(source[name].GetDouble());
}


void SDFF_Font::getJsonValue(const rapidjson::Value & source, const char * name, int * value) const
{
  assert(source.HasMember(name));

  if (source.HasMember(name))
    *value = source[name].GetInt();
}
