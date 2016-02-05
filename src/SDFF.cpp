#include "static_headers.h"

#include "SDFF.h"
#include "Crosy.h"

int SDFF_Bitmap::savePNG(const char * fileName)
{
  return stbi_write_png(fileName, width, height, 1, pixels.data(), 0);
}


void SDFF_Bitmap::resize(int width, int height)
{
  this->width = width;
  this->height = height;
  pixels.resize(width * height);
}


SDFF_Font::SDFF_Font() :
  falloff(0.0f),
  maxBearingY(0.0f),
  maxHeight(0.0f)
{

}


const SDFF_Glyph * SDFF_Font::getGlyph(SDFF_Char charCode) const
{
  GlyphMap::const_iterator glyphIt = glyphs.find(charCode);

  if (glyphIt != glyphs.end())
    return &glyphIt->second;
  else
    return NULL;
}


float SDFF_Font::getKerning(SDFF_Char leftChar, SDFF_Char rightChar) const
{

  KerningMap::const_iterator kerningIt = kerning.find({ leftChar, rightChar });

  if (kerningIt != kerning.end())
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
  writer.Double(falloff);
  writer.String("MaxBearingY");
  writer.Double(maxBearingY);
  writer.String("MaxHeight");
  writer.Double(maxHeight);

  writer.String("Glyphs");
  writer.StartArray();

  for (GlyphMap::const_iterator glyphIt = glyphs.begin(); glyphIt != glyphs.end(); ++glyphIt)
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

  for (KerningMap::const_iterator kerningIt = kerning.begin(); kerningIt != kerning.end(); ++kerningIt)
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

  getValue(doc, "Falloff", &falloff);
  getValue(doc, "MaxBearingY", &maxBearingY);
  getValue(doc, "MaxHeight", &maxHeight);
  const rapidjson::Value & glyphArray = getValue(doc, "Glyphs");
  assert(glyphArray.IsArray());

  if (glyphArray.IsArray())
  {
    for (rapidjson::Value::ConstValueIterator glyphIt = glyphArray.Begin();
         glyphIt != glyphArray.End();
         ++glyphIt)
    {
      SDFF_Char charCode;
      charCode = getValue(*glyphIt, "code").GetInt();
      SDFF_Glyph & glyph = glyphs[charCode];
      getValue(*glyphIt, "left", &glyph.left);
      getValue(*glyphIt, "right", &glyph.right);
      getValue(*glyphIt, "top", &glyph.top);
      getValue(*glyphIt, "bottom", &glyph.bottom);
      getValue(*glyphIt, "bearingX", &glyph.bearingX);
      getValue(*glyphIt, "bearingY", &glyph.bearingY);
      getValue(*glyphIt, "advance", &glyph.advance);
      getValue(*glyphIt, "width", &glyph.width);
      getValue(*glyphIt, "height", &glyph.height);
    }
  }

  const rapidjson::Value & kerningArray = getValue(doc, "Kerning");
  assert(kerningArray.IsArray());

  if (kerningArray.IsArray())
  {
    for (rapidjson::Value::ConstValueIterator kerningIt = kerningArray.Begin();
         kerningIt != kerningArray.End();
         ++kerningIt)
    {
      CharPair charPair;
      charPair.left = getValue(*kerningIt, "leftCode").GetInt();
      charPair.right = getValue(*kerningIt, "rightCode").GetInt();
      kerning[charPair] = (float)getValue(*kerningIt, "kerning").GetDouble();
    }
  }

  return 0;
}


const rapidjson::Value & SDFF_Font::getValue(const rapidjson::Value & source, const char * name) const
{
  assert(source.HasMember(name));
  static rapidjson::Value emptyValue;

  if (source.HasMember(name))
    return source[name];
  else
    return emptyValue;
}


void SDFF_Font::getValue(const rapidjson::Value & source, const char * name, float * value) const
{
  assert(source.HasMember(name));

  if (source.HasMember(name))
    *value = float(source[name].GetDouble());
}


void SDFF_Font::getValue(const rapidjson::Value & source, const char * name, int * value) const
{
  assert(source.HasMember(name));

  if (source.HasMember(name))
    *value = source[name].GetInt();
}


SDFF_Builder::SDFF_Builder() :
  initialized(false)
{
  FT_Init_FreeType(&ftLibrary);
}


SDFF_Builder::~SDFF_Builder()
{
  FT_Done_FreeType(ftLibrary);
}


SDFF_Error SDFF_Builder::init(int sourceFontSize, int sdfFontSize, float falloff)
{
  assert(sourceFontSize > 0);
  assert(sdfFontSize > 0);
  assert(falloff >= 0);
  initialized = false;

  if (sourceFontSize <= 0 || sdfFontSize <= 0 || falloff < 0)
    return SDFF_INVALID_VALUE;

  this->sourceFontSize = sourceFontSize;
  this->sdfFontSize = sdfFontSize;
  this->falloff = falloff;
  fonts.clear();
  initialized = true;

  return SDFF_OK;
}


SDFF_Error SDFF_Builder::addFont(const char * fileName, int faceIndex, SDFF_Font * out_font)
{
  assert(initialized);

  if (!initialized)
    return SDFF_NOT_INITIALIZED;

  if (fonts.find(out_font) != fonts.end())
    return SDFF_FONT_ALREADY_EXISTS;

  FontData & fontData = fonts[out_font];
  FT_Face & ftFace = fontData.ftFace;
  out_font->falloff = falloff;
  FT_Error ftError;
  ftError = FT_New_Face(ftLibrary, fileName, faceIndex, &ftFace);
  assert(!ftError);

  if (ftError)
    return SDFF_FT_NEW_FACE_ERROR;

  ftError = FT_Set_Char_Size(ftFace, sourceFontSize * 64, sourceFontSize * 64, 64, 64);
  assert(!ftError);

  if (ftError)
    return SDFF_FT_SET_CHAR_SIZE_ERROR;

  return SDFF_OK;
}


SDFF_Error SDFF_Builder::addChar(SDFF_Font & font, SDFF_Char charCode)
{
  assert(initialized);

  if (!initialized)
    return SDFF_NOT_INITIALIZED;

  if (fonts.find(&font) == fonts.end())
    return SDFF_FONT_NOT_EXISTS;

  FontData & fontData = fonts[&font];
  FT_Face & ftFace = fontData.ftFace;
  CharMap & chars = fontData.chars;

  if (chars.find(charCode) != chars.end())
    return SDFF_CHAR_ALREADY_EXISTS;

  FT_Error ftError = FT_Load_Char(ftFace, (const FT_UInt)charCode, FT_LOAD_DEFAULT | 
                                  FT_LOAD_IGNORE_TRANSFORM | FT_LOAD_RENDER | 
                                  FT_LOAD_TARGET_MONO | FT_LOAD_FORCE_AUTOHINT);
  assert(!ftError);

  if (ftError)
    return SDFF_FT_SET_CHAR_SIZE_ERROR;

  SDFF_Bitmap & charBitmap = chars[charCode];

  if (ftFace->glyph->bitmap.width && ftFace->glyph->bitmap.rows)
  {
    static DistanceFieldVector srcSdf;
    static DistanceFieldVector destSdf;
    // TODO: reserve for max bitmap size
    //int maxWidth = fontData.width * (ftFace->bbox.xMax - ftFace->bbox.xMin) / ftFace->units_per_EM;
    //int maxHeight = fontData.height * (ftFace->bbox.yMax - ftFace->bbox.yMin) / ftFace->units_per_EM;
    //srcSdf.reserve(maxWidth * maxHeight);
    int srcFalloff = int(falloff * sourceFontSize);
    createSdf(ftFace->glyph->bitmap, srcFalloff, srcSdf);

    int srcWidth = ftFace->glyph->bitmap.width + 2 * srcFalloff;
    int srcHeight = ftFace->glyph->bitmap.rows + 2 * srcFalloff;
    float fontScale = (float)sdfFontSize / sourceFontSize;
    int destWidth = (int)glm::ceil(srcWidth * fontScale);
    int destHeight = (int)glm::ceil(srcHeight * fontScale);
    float horzScale = float(destWidth) / srcWidth;
    float vertScale = float(destHeight) / srcHeight;
    float horzOpScale = 1.0f / horzScale;
    float vertOpScale = 1.0f / vertScale;
    destSdf.assign(destWidth * destHeight, 0.0f);

    for (int y = 0; y < srcHeight; y++)
    {
      for (int x = 0; x < srcWidth; x++)
      {
        int srcIndex = x + y * srcWidth;

        float destLeftf = x * horzScale;
        float destRightf = (x + 1) * horzScale;
        float destTopf = y * vertScale;
        float destBottomf = (y + 1) * vertScale;

        int destLefti = (int)destLeftf;
        int destRighti = (int)destRightf;
        int destTopi = (int)destTopf;
        int destBottomi = (int)destBottomf;

        if (destLefti == destRighti || destRighti >= destWidth)
        {
          if (destTopi == destBottomi || destBottomi >= destHeight)
          {
            int destIndex = destLefti + destTopi * destWidth;
            destSdf[destIndex] += srcSdf[srcIndex];
          }
          else
          {
            float topChunk = (destBottomi - destTopf) * vertOpScale;
            float bottomChunk = (destBottomf - destBottomi) * vertOpScale;
            int destIndex0 = destLefti + destTopi * destWidth;
            int destIndex1 = destLefti + destBottomi * destWidth;
            destSdf[destIndex0] += topChunk * srcSdf[srcIndex];
            destSdf[destIndex1] += bottomChunk * srcSdf[srcIndex];
          }
        }
        else
        {
          if (destTopi == destBottomi || destBottomi >= destHeight)
          {
            float leftChunk = (destRighti - destLeftf) * horzOpScale;
            float rightChunk = (destRightf - destRighti) * horzOpScale;
            int destIndex0 = destLefti + destTopi * destWidth;
            int destIndex1 = destRighti + destTopi * destWidth;
            destSdf[destIndex0] += leftChunk * srcSdf[srcIndex];
            destSdf[destIndex1] += rightChunk * srcSdf[srcIndex];
          }
          else
          {
            float leftChunk = (destRighti - destLeftf) * horzOpScale;
            float rightChunk = (destRightf - destRighti) * horzOpScale;
            float topChunk = (destBottomi - destTopf) * vertOpScale;
            float bottomChunk = (destBottomf - destBottomi) * vertOpScale;
            int destIndex00 = destLefti + destTopi * destWidth;
            int destIndex10 = destRighti + destTopi * destWidth;
            int destIndex01 = destLefti + destBottomi * destWidth;
            int destIndex11 = destRighti + destBottomi * destWidth;
            destSdf[destIndex00] += leftChunk * topChunk * srcSdf[srcIndex];
            destSdf[destIndex10] += rightChunk * topChunk * srcSdf[srcIndex];
            destSdf[destIndex01] += leftChunk * bottomChunk * srcSdf[srcIndex];
            destSdf[destIndex11] += rightChunk * bottomChunk * srcSdf[srcIndex];
          }
        }
      }
    }

    charBitmap.resize(destWidth, destHeight);
    float sqScale = horzScale * vertScale;

    for (int y = 0; y < destHeight; y++)
      for (int x = 0; x < destWidth; x++)
      {
        int ind = x + y * destWidth;
        charBitmap[ind] = (unsigned char)glm::clamp(128 - int(destSdf[ind] * sqScale * 127 / srcFalloff), 
                                                    0, 255);
      }
  }
  else 
    charBitmap.resize(0, 0);
  
  if (chars.size() > 1)
  {
    for (CharMap::iterator charIt = chars.begin(); charIt != chars.end(); ++charIt)
    {
      if (charIt->first != charCode)
      {
        FT_UInt glyphIndex1 = FT_Get_Char_Index(ftFace, FT_ULong(charIt->first));
        FT_UInt glyphIndex2 = FT_Get_Char_Index(ftFace, FT_ULong(charCode));
        SDFF_Font::CharPair charPair = { charIt->first, charCode };
        FT_Vector kern = { 0, 0 };
        ftError = FT_Get_Kerning(ftFace, glyphIndex1, glyphIndex2, FT_KERNING_UNFITTED, &kern);
        assert(!ftError);

        if (kern.x)
          font.kerning[charPair] = float(kern.x) / sourceFontSize;

        charPair = { charCode, charIt->first };
        kern = { 0, 0 };
        ftError = FT_Get_Kerning(ftFace, glyphIndex2, glyphIndex1, FT_KERNING_UNFITTED, &kern);
        assert(!ftError);

        if (kern.x)
          font.kerning[charPair] = float(kern.x) / sourceFontSize;
      }
    }
  }

  SDFF_Glyph & glyph = font.glyphs[charCode];
  glyph.bearingX = float(ftFace->glyph->metrics.horiBearingX) / 64 / sourceFontSize;
  glyph.bearingY = float(ftFace->glyph->metrics.horiBearingY) / 64 / sourceFontSize;
  glyph.advance = float(ftFace->glyph->metrics.horiAdvance) / 64 / sourceFontSize;
  glyph.width = float(ftFace->glyph->metrics.width) / 64 / sourceFontSize;
  glyph.height = float(ftFace->glyph->metrics.height) / 64 / sourceFontSize;
  font.maxBearingY = glm::max(font.maxBearingY, glyph.bearingY);
  font.maxHeight = glm::max(font.maxHeight, glyph.height);

  return SDFF_OK;
}


SDFF_Error SDFF_Builder::addChars(SDFF_Font & font, SDFF_Char firstCharCode, SDFF_Char lastCharCode)
{
  for (SDFF_Char charCode = firstCharCode; charCode <= lastCharCode; charCode++)
  {
    SDFF_Error error = addChar(font, charCode);

    if (error != SDFF_OK)
      return error;
  }

  return SDFF_OK;
}


SDFF_Error SDFF_Builder::composeTexture(SDFF_Bitmap & bitmap, bool powerOfTwo)
{
  assert(initialized);

  if (!initialized)
    return SDFF_NOT_INITIALIZED;

  typedef std::multimap<int, Rect> CharRectMap;
  typedef std::unordered_set<Rect> FreeRectSet;
  typedef std::vector<FreeRectSet::iterator> EraseVector;
  typedef std::vector<Rect> InsertVector;
  CharRectMap charRects;
  FreeRectSet freeRects;
  EraseVector eraseVector;
  InsertVector insertVector;
  eraseVector.reserve(1024);
  insertVector.reserve(1024);

  // add all our char rects into the array
  for (FontMap::iterator fontIt = fonts.begin(); fontIt != fonts.end(); ++fontIt)
  {
    CharMap & chars = fontIt->second.chars;

    for (CharMap::iterator charIt = chars.begin(); charIt != chars.end(); ++charIt)
    {
      int width = charIt->second.width;
      int height = charIt->second.height;
      Rect charRect = { 0, 0, width, height, fontIt->first, charIt->first };
      charRects.insert(std::make_pair(width * height, charRect));
    }
  }

  // statring with one free very big rect
  freeRects.insert({ 0, 0, INT_MAX, INT_MAX });
  int maxRight = 0;
  int maxBottom = 0;

  // enumerating all chars
  for (CharRectMap::reverse_iterator charRectIt = charRects.rbegin(); 
       charRectIt != charRects.rend(); 
       ++charRectIt)
  {
    if (!charRectIt->second.width || !charRectIt->second.height)
      continue;

    char cc = charRectIt->second.charCode;

    // find best fit free rect for placing our char
    Rect & charRect = charRectIt->second;
    const Rect * bestRectPtr = NULL;
    float bestEstimator = FLT_MAX;

    // for each char searching for most appropriate free rect using estimator
    for (FreeRectSet::iterator freeRectIt = freeRects.begin(); 
         freeRectIt != freeRects.end(); 
         ++freeRectIt)
    {
      freeRects;
      const Rect & freeRect = *freeRectIt;

      if (charRect.fitIn(freeRect))
      {
        int thisRight = freeRect.left + charRect.width;
        int thisBottom = freeRect.top + charRect.height;
        int thisMaxRight = glm::max(maxRight, thisRight);
        int thisMaxBottom = glm::max(maxBottom, thisBottom);
        int minBounds = glm::max(thisMaxRight, thisMaxBottom);
        int minLeftTop = (freeRect.left + freeRect.top);
        float thisEstimator = 10.0f * minBounds + 0.1f * minLeftTop;

        if (thisEstimator < bestEstimator)
        {
          bestEstimator = thisEstimator;
          bestRectPtr = &freeRect;
        }
      }
    }

    assert(bestRectPtr);

    if (bestRectPtr)
    {
      // placing char into selected best free rect
      charRect.left = bestRectPtr->left;
      charRect.top = bestRectPtr->top;
      maxRight = glm::max(maxRight, charRect.left + charRect.width);
      maxBottom = glm::max(maxBottom, charRect.top + charRect.height);

      eraseVector.clear();
      insertVector.clear();

      // excluding char rect from any free rects that intersects with it
      for (FreeRectSet::iterator freeRectIt = freeRects.begin(); 
           freeRectIt != freeRects.end(); ++freeRectIt)
      {
        const Rect & freeRect = *freeRectIt;

        if (charRect.intersect(freeRect))
        {
          bool leftTopIn = freeRect.contain(charRect.left, charRect.top);
          bool leftBottomIn = freeRect.contain(charRect.left, charRect.bottom());
          bool rightTopIn = freeRect.contain(charRect.right(), charRect.top);
          bool rightBottomIn = freeRect.contain(charRect.right(), charRect.bottom());
          int vertexInCount = (int)leftTopIn + (int)leftBottomIn + (int)rightTopIn + (int)rightBottomIn;

          if (vertexInCount == 0)
          {
            bool leftSideCross = charRect.left >= freeRect.left && 
                                 charRect.left <= freeRect.right();
            bool rightSideCross = charRect.right() >= freeRect.left && 
                                  charRect.right() <= freeRect.right();
            bool topSideCross = charRect.top >= freeRect.top && 
                                charRect.top <= freeRect.bottom();
            bool bottomSideCross = charRect.bottom() >= freeRect.top && 
                                   charRect.bottom() <= freeRect.bottom();
            int crossCount = (int)leftSideCross + 
                             (int)rightSideCross + 
                             (int)topSideCross + 
                             (int)bottomSideCross;

            if (crossCount == 1)
            {
              Rect newRect = freeRect;

              if (leftSideCross)
                newRect.width = charRect.left - newRect.left;
              else if (topSideCross)
                newRect.height = charRect.top - newRect.top;
              else if (rightSideCross)
              {
                int dLeft = charRect.left + charRect.width - newRect.left;
                newRect.left += dLeft;
                newRect.width -= dLeft;
              }
              else if (bottomSideCross)
              {
                int dTop = charRect.top + charRect.height - newRect.top;
                newRect.top += dTop;
                newRect.height -= dTop;
              }
              else 
                assert(0);

              if (newRect.width && newRect.height)
                insertVector.push_back(newRect);
            }
            else if (crossCount == 2)
            {
              Rect newRect1 = freeRect;
              Rect newRect2 = freeRect;
              
              if (leftSideCross) // and respectively rightSideCross
              {
                newRect1.width = charRect.left - newRect1.left;
                int dLeft = charRect.left + charRect.width - newRect2.left;
                newRect2.left += dLeft;
                newRect2.width -= dLeft;
              }
              else if (topSideCross) // and respectively bottomSideCross
              {
                newRect1.height = charRect.top - newRect1.top;
                int dTop = charRect.top + charRect.height - newRect2.top;
                newRect2.top += dTop;
                newRect2.height -= dTop;
              }
              else 
                assert(0);

              if (newRect1.width && newRect1.height)
                insertVector.push_back(newRect1);

              if (newRect2.width && newRect2.height)
                insertVector.push_back(newRect2);
            }
            else 
              assert(0);

          }
          else if(vertexInCount == 1)
          {
            Rect newRect1 = freeRect;
            Rect newRect2 = freeRect;
            
            if (leftTopIn)
            {
              newRect1.height = charRect.top - newRect1.top;
              newRect2.width = charRect.left - newRect2.left;

              if (newRect1.height)
                insertVector.push_back(newRect1);

              if (newRect2.width)
                insertVector.push_back(newRect2);
            }
            else if (leftBottomIn)
            {
              newRect1.width = charRect.left - newRect1.left;
              int dTop = charRect.top + charRect.height - newRect2.top;
              newRect2.top += dTop;
              newRect2.height -= dTop;
            }
            else if (rightTopIn)
            {
              newRect1.height = charRect.top - newRect1.top;
              int dLeft = charRect.left + charRect.width - newRect2.left;
              newRect2.left += dLeft;
              newRect2.width -= dLeft;
            }
            else if (rightBottomIn)
            {
              int dLeft = charRect.left + charRect.width - newRect1.left;
              newRect1.left += dLeft;
              newRect1.width -= dLeft;
              int dTop = charRect.top + charRect.height - newRect2.top;
              newRect2.top += dTop;
              newRect2.height -= dTop;
            }
            else 
              assert(0);

            if (newRect1.width && newRect1.height)
              insertVector.push_back(newRect1);

            if (newRect2.width && newRect2.height)
              insertVector.push_back(newRect2);
          }
          else if (vertexInCount == 2)
          {
            bool leftSideIn = leftTopIn && leftBottomIn;
            bool topSideIn = leftTopIn && rightTopIn;
            bool rightSideIn = rightTopIn && rightBottomIn;
            bool bottomSideIn = leftBottomIn && rightBottomIn;
            Rect newRect1 = freeRect;
            Rect newRect2 = freeRect;
            Rect newRect3 = freeRect;
            
            if (leftSideIn)
            {
              newRect1.width = charRect.left - newRect1.left;
              newRect2.height = charRect.top - newRect2.top;
              int dTop = charRect.top + charRect.height - newRect3.top;
              newRect3.top += dTop;
              newRect3.height -= dTop;
            }
            else if (topSideIn)
            {
              newRect1.width = charRect.left - newRect1.left;
              newRect2.height = charRect.top - newRect2.top;
              int dLeft = charRect.left + charRect.width - newRect3.left;
              newRect3.left += dLeft;
              newRect3.width -= dLeft;
            }
            else if (rightSideIn)
            {
              newRect1.height = charRect.top - newRect1.top;
              int dLeft = charRect.left + charRect.width - newRect2.left;
              newRect2.left += dLeft;
              newRect2.width -= dLeft;
              int dTop = charRect.top + charRect.height - newRect3.top;
              newRect3.top += dTop;
              newRect3.height -= dTop;
            }
            else if (bottomSideIn)
            {
              newRect1.width = charRect.left - newRect1.left;
              int dLeft = charRect.left + charRect.width - newRect2.left;
              newRect2.left += dLeft;
              newRect2.width -= dLeft;
              int dTop = charRect.top + charRect.height - newRect3.top;
              newRect3.top += dTop;
              newRect3.height -= dTop;
            }
            else 
              assert(0);

            if (newRect1.width && newRect1.height)
              insertVector.push_back(newRect1);

            if (newRect2.width && newRect2.height)
              insertVector.push_back(newRect2);

            if (newRect3.width && newRect3.height)
              insertVector.push_back(newRect3);
          }
          else if (vertexInCount == 4)
          {
            Rect newRect1 = freeRect;
            Rect newRect2 = freeRect;
            Rect newRect3 = freeRect;
            Rect newRect4 = freeRect;

            newRect1.width = charRect.left - newRect1.left;
            newRect2.height = charRect.top - newRect2.top;
            int dLeft = charRect.left + charRect.width - newRect3.left;
            newRect3.left += dLeft;
            newRect3.width -= dLeft;
            int dTop = charRect.top + charRect.height - newRect4.top;
            newRect4.top += dTop;
            newRect4.height -= dTop;

            if (newRect1.width)
              insertVector.push_back(newRect1);

            if (newRect2.height)
              insertVector.push_back(newRect2);

            if (newRect3.width)
              insertVector.push_back(newRect3);

            if (newRect4.height)
              insertVector.push_back(newRect4);
          }
          else 
            assert(0);

          eraseVector.push_back(freeRectIt);
        }
      }

      for (EraseVector::iterator eraseIt = eraseVector.begin();
           eraseIt != eraseVector.end(); ++eraseIt)
      {
        freeRects.erase(*eraseIt);
      }

      for (InsertVector::iterator insertIt = insertVector.begin(); 
           insertIt != insertVector.end(); ++insertIt)
      {
        freeRects.insert(*insertIt);
      }
    }
  }

  int width = maxRight + 1;
  int height = maxBottom + 1;

  if (powerOfTwo)
  {
    width = firstPowerOfTwoGreaterThen(width);
    height = firstPowerOfTwoGreaterThen(height);
  }
  
  bitmap.resize(width, height);

  for (CharRectMap::iterator charRectIt = charRects.begin(); 
       charRectIt != charRects.end(); ++charRectIt)
  {
    Rect & charRect = charRectIt->second;
    FontMap::iterator fontIt = fonts.find(charRect.font);

    if (fontIt != fonts.end())
    {
      CharMap::iterator charIt = fontIt->second.chars.find(charRect.charCode);

      if (charIt != fontIt->second.chars.end())
      {
        SDFF_Font * font = fontIt->first;
        SDFF_Char charCode = charIt->first;
        SDFF_Glyph & glyph = font->glyphs[charCode];
        glyph.left = float(charRect.left) / width;
        glyph.right = float(charRect.right() + 1) / width;
        glyph.top = float(charRect.top) / height;
        glyph.bottom = float(charRect.bottom() + 1) / height;

        SDFF_Bitmap & charBitmap = charIt->second;
        copyBitmap(charBitmap, bitmap, charRect.left, charRect.top);
      }
    }
  }

  return SDFF_OK;
}


int SDFF_Builder::firstPowerOfTwoGreaterThen(int value)
{
  value--;
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  value++;

  return value;
}


void SDFF_Builder::copyBitmap(const SDFF_Bitmap & srcBitmap, SDFF_Bitmap & destBitmap, 
                              int xPos, int yPos) const
{
  assert(xPos >= 0);
  assert(yPos >= 0);
  assert(xPos + srcBitmap.width <= destBitmap.width);
  assert(yPos + srcBitmap.height <= destBitmap.height);

  int srcIndex = 0;
  int destIndex = yPos * destBitmap.width + xPos;

  for (int y = 0; y < srcBitmap.height; y++)
  {
    for (int x = 0; x < srcBitmap.width; x++)
    {
      destBitmap[destIndex] = destBitmap[destIndex] ? destBitmap[destIndex] : srcBitmap[srcIndex];
      srcIndex++;
      destIndex++;
    }

    destIndex += destBitmap.width - srcBitmap.width;
  }
}


float SDFF_Builder::createSdf(const FT_Bitmap & ftBitmap, int falloff, 
                              DistanceFieldVector & result) const
{
  float maxDist = createDf(ftBitmap, falloff, false, result);

  DistanceFieldVector invResult;
  float maxDistInv = createDf(ftBitmap, falloff, true, invResult);

  for (int i = 0, cnt = (int)result.size(); i < cnt; i++)
  {
    if (result[i] < 0.5f)
      result[i] = 1 - invResult[i];
  }

  return glm::max(maxDist, maxDistInv - 1);
}


//  Based on "General algorithm for computing distance transforms in linear time"
//  by A.MEIJSTER‚ J.B.T.M.ROERDINK† and W.H.HESSELINK†
//  University of Groningen
//  http://www.rug.nl/research/portal/publications/a-general-algorithm-for-computing-distance-transforms-in-linear-time(15dd2ec9-d221-45da-b2b0-1164978717dc).html

float SDFF_Builder::createDf(const FT_Bitmap & ftBitmap, int falloff, bool invert, 
                             DistanceFieldVector & result) const
{
  assert(ftBitmap.width > 0);
  assert(ftBitmap.rows > 0);
  assert(falloff >= 0);
  int width = ftBitmap.width + 2 * falloff;
  int height = ftBitmap.rows + 2 * falloff;
  unsigned char * pixels = ftBitmap.buffer;
  int pitch = ftBitmap.pitch;
  result.resize(width * height);

  struct
  {
    int operator()(int x, int i, int g_i) 
    { 
      return (x - i) * (x - i) + g_i * g_i; 
    }
  } edt;

  struct 
  {
    int operator()(int i, int u, int g_i, int g_u) 
    { 
      return (u * u - i * i + g_u * g_u - g_i * g_i) / (2 * (u - i)); 
    }
  } sep;

  // First stage
  const int inf = width + height;
  std::vector<int> g(width * height);
  int invertValue = (int)invert;

  for (int x = 0; x < width; x++)	
  {
    int pix = invertValue;

    if (!falloff)
    {
      int ind = x / 8;
      int shr = 7 - x % 8;
      unsigned char byte = pixels[ind];
      pix = ((byte >> shr) & 1) ^ invertValue;
    }

    if (pix)
      g[x] = 0;
    else
      g[x] = inf;

    // Scan 1
    for (int y = 1; y < height; y++)	
    {
      int pix = invertValue;

      if (x >= falloff && x < width - falloff && y >= falloff && y < height - falloff)
      {
        int ind = (y - falloff) * pitch + (x - falloff) / 8;
        int shr = 7 - (x - falloff) % 8;
        unsigned char byte = pixels[ind];
        pix = ((byte >> shr) & 1) ^ invertValue;
      }

      if (pix)
        g[x + y * width] = 0;
      else
        g[x + y * width] = 1 + g[x + (y - 1) * width];
    }

    // Scan 2
    for (int y = height - 2; y >= 0; y--)	
    {
      if (g[x + (y + 1) * width] < g[x + y * width])
        g[x + y * width] = 1 + g[x + (y + 1) * width];
    }
  }

  // Second stage
  std::vector<int> s(width);
  std::vector<int> t(width);
  int q = 0;
  int w;
  float maxDistance = 0.0f;

  for (int y = 0; y < height; y++)	
  {
    q = 0;
    s[0] = 0;
    t[0] = 0;

      // Scan 3
    for (int x = 1; x < width; x++)	
    {
      while (q >= 0 && edt(t[q], s[q], g[s[q] + y * width]) > edt(t[q], x, g[x + y * width]))
        q--;

      if (q < 0)	
      {
        q = 0;
        s[0] = x;
      }
      else	
      {
        w = 1 + sep(s[q], x, g[s[q] + y * width], g[x + y * width]);

        if (w < width)	
        {
          q++;
          s[q] = x;
          t[q] = w;
        }
      }
    }

    // Scan 4
    for (int x = width - 1; x >= 0; x--)	
    {
      float distance = std::sqrtf((float)edt(x, s[q], g[s[q] + y * width]));
      result[x + y * width] = distance;

      if (distance > maxDistance)
        maxDistance = distance;

      if (x == t[q])
        q--;
    }
  }

  return maxDistance;
}


bool SDFF_Builder::Rect::intersect(const Rect & rect) const 
{ 
  return 
    rect.left < left + width && 
    rect.left + rect.width > left && 
    rect.top < top + height && 
    rect.top + rect.height > top; 
}


bool SDFF_Builder::Rect::contain(int x, int y) const 
{ 
  return 
    x >= left && 
    x < left + width && 
    y >= top && 
    y < top + height; 
}


bool SDFF_Builder::Rect::fitIn(const Rect & rect) const 
{ 
  return rect.width >= width && rect.height >= height; 
}


bool SDFF_Builder::Rect::operator ==(const Rect & rect) const 
{ 
  return 
    rect.left == left && 
    rect.top == top && 
    rect.width == width && 
    rect.height == height; 
}

