/*

  This file has been modified specially for pumpkin roll

*/


#include "pPack/textRendering.h"
#include "pPack/windowManager.h"
#include "pPack/shaderHandling.h"

#include <ft2build.h>
#include "freetype/freetype.h"
#include "stb_image.h"
#include "stb_image_write.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <fstream>
#include <vector>

#include "stdlib.h"


using namespace ::pPack;




namespace {



struct BitmapFileInfo {
  unsigned char charLow;
  unsigned char charHigh;
  int fontSize;
  IVector2 size;


  BitmapFileInfo();
};


struct CharacterVertex {
  IVector2 position;
  Vector2 uv;

  CharacterVertex();
  CharacterVertex(IVector2 Position, Vector2 UV);
};


CharacterVertex* AddLineIntoBuffer(CharacterVertex* vertices, const Bitmap& bitmap, IVector2 startAt, IVector4 area, std::string line, int fontSize, IVector3& at, RenderTextInfo& info);
bool FileExists(char const* file);
bool AreaOutOf(IVector4 area, IVector2 vec);
CharacterVertex* AddBarAt(CharacterVertex* vertices, IVector2 pos, RenderTextInfo& info, int fontSize);


const char signature[8] = {'B', 'I', 'T', 'M', 'A', 'P', 'T', 'R'};


std::string vertLocs = "/shaders/textRendering/textRendering.vert";
std::string fragLocs = "/shaders/textRendering/textRendering.frag";


FT_Library library;
FT_Error err;

ShaderHandler shader = ShaderHandler();


unsigned int VAO, VBO;


CharacterVertex* vertices = nullptr;


// Allows for about 1.33... million characters in one batch
// Probably enough
constexpr size_t _VERTEX_BUFFER_SIZE = 128000000;


} // namespace









namespace pPack {





// CharacterMetrics
// --------------------------------------------------

// Constructor
CharacterMetrics::CharacterMetrics() : pixelBottomLeft(0), pixelTopRight(0), uvBottomLeft(0), uvTopRight(0), size(0), bearing(0), advance(0) {}
CharacterMetrics::CharacterMetrics(IVector2 Size, IVector2 Bearing, float Advance)
: pixelBottomLeft(0), pixelTopRight(0), uvBottomLeft(0), uvTopRight(0), size(Size), bearing(Bearing), advance(Advance) {}

// --------------------------------------------------
// CharacterMetrics




// Bitmap
// --------------------------------------------------

void Bitmap::LoadFromFile(const char* file, int filtering) {
  if (file == nullptr) {
    std::cerr << "Null string\n";
    return;
  }

  if (!FileExists(file)) {
    return;
  }

  std::ifstream stream(file, std::ios::binary | std::ios::ate);

  size_t fileLength = stream.tellg();

  uint16_t imageFileLength = 0;
  char* bitmapImageFile = nullptr;
  BitmapFileInfo fileInfo;

  char* buffer = nullptr;
  CharacterMetrics* metricsBuffer = nullptr;
  void* imageBuffer = nullptr;

  int charCount = 0;
  int packAlignment, currentTexture;


  UIVector4 borderColor = {255};


  // Get file textureSize
  stream.seekg(0, std::ios::beg);

  // Needs 8 bytes for signature
  if (fileLength < 8) {
    stream.close();
    std::cerr << "Invalid file\n";
    return;
  }

  // Buffers
  buffer = (char*)calloc(fileLength, sizeof(char));

  // Make intellisense happy
  if (buffer == 0) {
    throw std::bad_alloc();
  }

  metricsBuffer = (CharacterMetrics*)buffer;


  // Get signature
  stream.read(buffer, 8);

  // Check signature
  if (memcmp(signature, buffer, 8) != 0) {
    std::cerr << "Invalid bitmap file\n";
    goto returnJump;
  }


  // string length
  stream.read((char*)&imageFileLength, 2);

  // Get string
  bitmapImageFile = (char*)malloc(imageFileLength + 1);

  // Make intellisense happy
  if (bitmapImageFile == 0) {
    throw std::bad_alloc();
  }

  bitmapImageFile[imageFileLength] = '\0';
  stream.read(bitmapImageFile, imageFileLength);

  if (!FileExists(bitmapImageFile)) {
    std::cerr << "Bitmap image DNE\n";
    goto returnJump;
  }


  // Get basic information
  stream.read((char*)&fileInfo, sizeof(BitmapFileInfo));
  if (fileInfo.charLow > fileInfo.charHigh || fileInfo.size == 0 || fileInfo.fontSize == 0) {
    std::cerr << "Invalid bitmap character range or size\n";
    goto returnJump;
  }


  charCount = (int)fileInfo.charHigh - fileInfo.charLow + 1;


  // Check if file has enough room for stated character range
  if (charCount * sizeof(CharacterMetrics) > fileLength - stream.tellg()) {
    std::cerr << "Invalid bitmap file\n";
    goto returnJump;
  }


  // Read image file
  imageBuffer = stbi_load(bitmapImageFile, &textureSize.x, &textureSize.y, nullptr, 1);
  if (imageBuffer == nullptr) {
    std::cerr << stbi_failure_reason() << '\n';
    goto returnJump;
  }


  // Destory any existing data
  Destroy();


  // Read all metrics
  stream.read(buffer, charCount * sizeof(CharacterMetrics));


  // Loop and set metrics in mapping
  for (unsigned int i = 0; i < charCount; i++, metricsBuffer++) {
    characterMapping[i] = *metricsBuffer;
  }


  glGetIntegerv(GL_UNPACK_ALIGNMENT, &packAlignment);
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);



  // Generate bitmap texture
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, fileInfo.size.x, fileInfo.size.y, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, imageBuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
  glTexParameterIuiv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (unsigned int*)&borderColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


  // Restore
  glPixelStorei(GL_UNPACK_ALIGNMENT, packAlignment);
  glBindTexture(GL_TEXTURE_2D, currentTexture);


  charLow = fileInfo.charLow;
  charHigh = fileInfo.charHigh;
  textureSize = fileInfo.size;
  fontSize = fileInfo.fontSize;


  returnJump:

  // Free resources

  stbi_image_free(imageBuffer);
  free(buffer);
  free(bitmapImageFile);
  stream.close();
}





void Bitmap::SaveToFile(const char* file, const char* imageLocation) {
  if (file == nullptr || imageLocation == nullptr) {
    std::cerr << "Null strings\n";
    return;
  }

  // Check if valid
  if (!Valid()) {
    std::cerr << "Invalid bitmap\n";
    return;
  }

  // Write signature and information
  std::ofstream stream(file, std::ios::binary | std::ios::trunc);
  stream.write(signature, 8);

  uint16_t stringLen = (uint16_t)strlen(imageLocation);
  stream.write((char*)&stringLen, 2);
  stream.write(imageLocation, stringLen);
  stream.write((char*)&charLow, 1);
  stream.write((char*)&charHigh, 1);

  // padding
  stream.seekp(2, std::ios::cur);

  stream.write((char*)&fontSize, sizeof(fontSize));
  stream.write((char*)&textureSize, sizeof(IVector2));

  int charCount = charHigh - charLow + 1;
  // Write metrics
  for (int i = 0; i < charCount; i++) {
    CharacterMetrics& m = characterMapping[i];
    stream.write((char*)&m, sizeof(CharacterMetrics));
  }


  // Close stream
  stream.flush();
  stream.close();



  // Allocate texture data
  void* textureData = malloc(textureSize.x * textureSize.y);


  int packAlignment;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &packAlignment);


  // Get texture data
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glGetTextureImage(texture, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, textureSize.x * textureSize.y, textureData);


  // Restore
  glPixelStorei(GL_PACK_ALIGNMENT, packAlignment);



  // Write texture
  stbi_write_png(imageLocation, textureSize.x, textureSize.y, 1, textureData, textureSize.x);

  // Free texture data
  free(textureData);
}





bool Bitmap::Valid() const {
  return texture != 0 && textureSize != 0 && charLow <= charHigh && !characterMapping.empty();
}





void Bitmap::Destroy() {
  // Delete and reset everything
  glDeleteTextures(1, &texture);
  characterMapping.clear();
  textureSize = 0;
  charLow = 0;
  charHigh = 0;
  texture = 0;
}





int Bitmap::GetFontSize() const { return fontSize; }
unsigned char Bitmap::GetCharLow() const { return charLow; }
unsigned char Bitmap::GetCharHigh() const { return charHigh; }





CharacterMetrics const& Bitmap::operator[](unsigned char c) const {
  if (c < charLow || c > charHigh) {
    return characterMapping[0];
  }

  return characterMapping[(int)c - charLow];
}





// Constructor
Bitmap::Bitmap() : texture(0), textureSize(0), fontSize(0), charLow(0), charHigh(0), characterMapping() {}

// --------------------------------------------------
// Bitmap



namespace text_rendering { 

void Start(std::string folderLoc) {

  // Start freetype
  err = FT_Init_FreeType(&library);
  if (err) {
    std::cerr << "Failed to initialize freetype\n";
    return;
  }


  // Create shader
  vertLocs = folderLoc + vertLocs;
  fragLocs = folderLoc + fragLocs;

  const char* vertC = vertLocs.c_str();
  const char* fragC = fragLocs.c_str();

  ShaderCreateInfo infos[] = {ShaderCreateInfo(&vertC, 1, GL_VERTEX_SHADER), ShaderCreateInfo(&fragC, 1, GL_FRAGMENT_SHADER)};
  shader = ShaderHandler::CreateShader(infos, 2);


  // Create buffers
  glCreateVertexArrays(1, &VAO);
  glCreateBuffers(1, &VBO);


  vertices = (CharacterVertex*)malloc(_VERTEX_BUFFER_SIZE);
}





void End() {
  FT_Done_FreeType(library);

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shader);

  free(vertices);
}

}; // namespace text_rendering




Bitmap GenerateBitmap(const char* fontFile, int size, unsigned char low, unsigned char high, int filtering) {
  Bitmap bitmap = Bitmap();

  // Check parameters
  if (!FileExists(fontFile) || size <= 0 || low > high || fontFile == nullptr) {
    std::cerr << "Invalid file or parameters\n";
    return bitmap;
  }

  // Load font
  FT_Face fontFace;
  err = FT_New_Face(library, fontFile, 0, &fontFace);
  if (err) {
    std::cerr << "Invalid font file\n";
    return bitmap;
  }
  FT_Set_Pixel_Sizes(fontFace, 0, size);


  int charCount = high - low + 1;
  // Load characters
  for (int i = 0; i < charCount; i++) {
    unsigned char c = i + low;
    // Load without creating buffer
    FT_Load_Char(fontFace, c, FT_LOAD_BITMAP_METRICS_ONLY);

    bitmap.characterMapping.push_back(CharacterMetrics(
      IVector2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
      IVector2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
      fontFace->glyph->advance.x >> 6
    ));
  }


  // Get character metrics and texture size
  int maxWidth = size * ceil(sqrt(high - low + 1));
  IVector2 textureSize = {maxWidth,0};
  int largestOnRow = 0;
  int penX = 0;

  for (int i = 0; i < charCount; i++) {
    CharacterMetrics& metrics = bitmap.characterMapping[i];

    // Add 2 pixels to prevent edges from touching
    if (penX + metrics.size.x + 2 > maxWidth) {
      penX = 0;
      textureSize.y += largestOnRow;
      largestOnRow = 0;
    }

    metrics.pixelBottomLeft = IVector2(penX, textureSize.y + metrics.size.y);
    metrics.pixelTopRight = IVector2(penX + metrics.size.x, textureSize.y);
    penX += metrics.size.x + 2;

    if (metrics.size.y > largestOnRow) {
      largestOnRow = metrics.size.y;
    }
  }
  textureSize.y += largestOnRow;


  // Set character uv coords
  for (int i = 0; i < charCount; i++) {
    CharacterMetrics& metrics = bitmap.characterMapping[i];

    metrics.pixelBottomLeft.y = textureSize.y - metrics.pixelBottomLeft.y;
    metrics.pixelTopRight.y = textureSize.y - metrics.pixelTopRight.y;

    metrics.uvBottomLeft = {(float)metrics.pixelBottomLeft.x / textureSize.x, (float)metrics.pixelTopRight.y / textureSize.y};
    metrics.uvTopRight = {(float)metrics.pixelTopRight.x / textureSize.x, (float)metrics.pixelBottomLeft.y / textureSize.y};
  }




  // Set unpack alignment since characters probably won't be aligned to any boundry
  int packAlignment, currentTexture;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &packAlignment); 
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);



  // Set some data
  bitmap.textureSize = textureSize;
  bitmap.charLow = low;
  bitmap.charHigh = high;
  bitmap.fontSize = size;


  // Create texutre
  UIVector4 borderColor = {255};
  glGenTextures(1, &bitmap.texture);
  glBindTexture(GL_TEXTURE_2D, bitmap.texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, textureSize.x, textureSize.y, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
  glTexParameterIuiv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (unsigned int*)&borderColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // Put each character into texture based on previous generated coords
  for (int i = 0; i < charCount; i++) {
    CharacterMetrics& metrics = bitmap.characterMapping[i];

    // Load second time with buffer
    FT_Load_Char(fontFace, i + low, FT_LOAD_COLOR | FT_LOAD_RENDER);

    glTexSubImage2D(GL_TEXTURE_2D, 0, metrics.pixelBottomLeft.x, metrics.pixelBottomLeft.y,
                    metrics.size.x, metrics.size.y, GL_RED_INTEGER, GL_UNSIGNED_BYTE, fontFace->glyph->bitmap.buffer);
  }



  // Restore
  glPixelStorei(GL_UNPACK_ALIGNMENT, packAlignment);
  glBindTexture(GL_TEXTURE_2D, currentTexture);

  // Free resources
  FT_Done_Face(fontFace);

  return bitmap;
}





void RenderText(const Bitmap& bitmap, std::string& text, RenderTextInfo info) {
  if (text.size() == 0) {
    if (info.showAt != nullptr) {
      info.showAt->x = 0;
      info.showAt->y = 0;
      info.showAt->z = 0;
    }
    return;
  }


  if (!bitmap.Valid()) {
    std::cerr << "Invalid bitmap\n";
    return;
  }
  

  // scaling
  info.frameSize.x /= info.fontScale;
  info.frameSize.y /= info.fontScale;


  // Move startpos to [0,0] - [1,1] range
  info.startPos += 1;
  info.startPos /= 2;

  info.textWidth /= 2;

  // Get positioning info in pixels
  int startXPixels = info.startPos.x * info.frameSize.x;
  int startYPixels = info.startPos.y * info.frameSize.y;
  int widthPixels = info.textWidth * info.frameSize.x;
  int maxXPixels = startXPixels + widthPixels;
  int jumpPixels = info.lineSize * bitmap.fontSize;

  if (!info.startTop) {
    startYPixels -= jumpPixels;
  }


  std::string line = std::string();
  std::string word = std::string();
  bool oneWord = true;


  int lineCount = 1;
  
  // Keeps track of where to add next chunk of characters, and character count
  CharacterVertex* addSpot = vertices;

  IVector2 linePos = IVector2(startXPixels, startYPixels);
  IVector2 pen = IVector2(startXPixels, startYPixels);

  Vector4 tmpArea = ((info.area + 1) / 2);
  IVector4 area = IVector4(tmpArea.x * info.frameSize.x, tmpArea.y * info.frameSize.y, tmpArea.z * info.frameSize.x, tmpArea.w * info.frameSize.y);

  // Offset if text is going to be centered vertically
  if (info.centerY) {
    int offset = area.w - startYPixels;
    area.y -= offset;
    area.w -= offset;
  }



  IVector3 at = {1, 0, 1};


  int wordLength = 0;

  bool nl = false;
  bool ws = false;
  bool cr = false;

  for (size_t i = 0; i < text.size(); i++) {
    char c = text[i];
    nl = false;

    CharacterMetrics const& metrics = bitmap[c];

    if (std::isspace(c)) {

      // Skips first white space after a fresh new line
      if (nl) {
        continue;
      }

      if (word.size() != 0) {
        line += word;
        word.clear();
        wordLength = 0;
      }

      ws = true;
      oneWord = false;
    } else {
      ws = false;
    }


    // If new line or needs new line
    if (pen.x + metrics.advance >= maxXPixels || c == '\r' || c == '\n') {
      if (c == '\n' && cr) {
        cr = false;
        at.z++;
        continue;
      }

      // Stops \n if \r happend first
      if (c == '\r') {
        cr = true;
      } else {
        cr = false;
      }

      // Split single word up to multiple lines
      if (oneWord) {
        line += word;
        word.clear();
        wordLength = 0;
        nl = true;

      } 

      // Centers on the line
      if (info.centerX) {
        linePos.x = startXPixels + (maxXPixels - pen.x + wordLength) / 2;
      }




      addSpot = AddLineIntoBuffer(addSpot, bitmap, linePos, area, line, bitmap.fontSize, at, info);
      line.clear();

      if (c == '\n' || c == '\r') {
        if (info.showAt != nullptr && at.y == info.showAt->y) {
          if (at.x <= info.showAt->x) {
            if (info.showAt->z == -1) {
              info.showAt->x = at.x - 1;
              info.showAt->z = at.z - 1;
            } else {
              info.showAt->x = 0;
              info.showAt->y++;
              info.showAt->z = at.z;
            }

          } else if (info.showAt->x < 0) {
            info.showAt->x = at.x - 1;
          }
        }


        at.x = 1;
        at.y++;
        at.z++;
      }


      //if (sInWord) {
      //  showAtPos.x = showAtPos.x - pen.x;
      //  showAtPos.y -= jumpPixels;
      //}

      // Move down
      pen.x = startXPixels + wordLength;
      pen.y -= jumpPixels;
      lineCount++;

      linePos = IVector2(startXPixels, linePos.y - jumpPixels);
      oneWord = true;

      if (ws) {
        continue;
      }
    }
    cr = false;


    word += c;
    pen.x += metrics.advance;

    if (!ws) {
      wordLength += metrics.advance;
    }
  }

  // Get whatever is left over, out
  line += word;
  if (info.centerX) {
    linePos.x = startXPixels + (maxXPixels - pen.x) / 2;
  }

  if (info.showAt != nullptr && at.y < info.showAt->y) {
    info.showAt->y = at.y;
  }

  addSpot = AddLineIntoBuffer(addSpot, bitmap, linePos, area, line, bitmap.fontSize, at, info);


  if (info.showAt != nullptr && (at.x - 1 < info.showAt->x || info.showAt->x < -1) && at.y == info.showAt->y) {
    info.showAt->x = at.x - 1;
  }



  size_t vertexCount = size_t(addSpot - vertices);


  int savedProgram, currentTexture, activeTexture, currentVAO, currentVBO;
  glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
  glGetIntegerv(GL_BUFFER_BINDING, &currentVBO);
  glGetIntegerv(GL_CURRENT_PROGRAM, &savedProgram);


  shader.Active();


  // Set cutoff value
  ShaderHandler::SetFloat("cutoff", info.cutoff);

  // Set area
  ShaderHandler::SetVector4("area", info.area);


  // Transform from pixel space to view space
  glm::mat4 transform = glm::mat4(1);
  transform = glm::translate(transform, glm::vec3(-1.0f, -1.0f, 0.0f));

  transform = glm::scale(transform, glm::vec3(2.0f / info.frameSize.x, 2.0f / info.frameSize.y, 1.0f));


  if (info.centerY) {
    transform = glm::translate(transform, glm::vec3(0, (lineCount * jumpPixels) / 2.f, 0));
  }


  ShaderHandler::SetMat4("transform", glm::value_ptr(transform));
  ShaderHandler::SetVector4("textColor", info.color);
  ShaderHandler::SetInt("bitmapTexture", 0);
  ShaderHandler::SetVector2("textureSize", bitmap.textureSize.ConvertTo<float>());

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, bitmap.texture);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(CharacterVertex), vertices, GL_STREAM_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribIPointer(0, 2, GL_INT, sizeof(CharacterVertex), 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterVertex), (void*)(sizeof(IVector2)));

  glDrawArrays(GL_TRIANGLES, 0, vertexCount);


  // Restore
  ShaderHandler::SetCurrentShader(savedProgram);
  glActiveTexture(activeTexture);
  glBindTexture(GL_TEXTURE_2D, currentTexture);
  glBindBuffer(GL_ARRAY_BUFFER, currentVBO);
  glBindVertexArray(currentVAO);
}



} // namespace pPack










namespace {





// BitmapFileInfo
// --------------------------------------------------

// Constructor
BitmapFileInfo::BitmapFileInfo() : charLow(0), charHigh(0), fontSize(0), size(0) {}

// --------------------------------------------------
// BitmapFileInfo





// CharacterVertex
// --------------------------------------------------

CharacterVertex::CharacterVertex() : position(0), uv(0) {}
CharacterVertex::CharacterVertex(IVector2 Position, Vector2 UV) : position(Position), uv(UV) {}

// --------------------------------------------------
// CharacterVertex





CharacterVertex* AddLineIntoBuffer(CharacterVertex* vertices, Bitmap const& bitmap, IVector2 startAt, IVector4 area, std::string line, int fontSize, IVector3& at, RenderTextInfo& info) {
  if (line.size() == 0) {
    if (info.showAt != nullptr && info.showAt->x == 0 && at.x == 1 && at.y == info.showAt->y) {
      vertices = AddBarAt(vertices, {startAt.x, startAt.y}, info, fontSize);
      info.showAt->z = at.z - 1;
    }

    return vertices;
  }

  IVector2 pen = startAt;

  CharacterVertex verts[6];

  // First character shouldn't move forward by it's bearing
  CharacterMetrics const& metrics = bitmap[line[0]];



  if (info.showAt != nullptr) {


    if (at.y == info.showAt->y) {
      if (at.x == info.showAt->x) {
        vertices = AddBarAt(vertices, {pen.x + metrics.size.x, pen.y}, info, fontSize);
        info.showAt->z = at.z;
      } else if (info.showAt->x == 0) {
        vertices = AddBarAt(vertices, {pen.x, pen.y}, info, fontSize);
        info.showAt->z = at.z - 1;
      }
    }
  }
  at.x++;
  at.z++;



  // Only difference here
  if (!std::isspace(line[0])) {
    IVector2 bl = {pen.x, pen.y - (metrics.size.y - metrics.bearing.y)};
    IVector2 tr = bl + metrics.size;

    if (AreaOutOf(area, bl) && AreaOutOf(area, tr)) {
      goto skipFirst;
    }

    verts[0] = CharacterVertex(bl, metrics.uvBottomLeft);
    verts[1] = CharacterVertex({tr.x, bl.y}, {metrics.uvTopRight.x, metrics.uvBottomLeft.y});
    verts[2] = CharacterVertex({bl.x, tr.y}, {metrics.uvBottomLeft.x, metrics.uvTopRight.y});

    verts[3] = CharacterVertex({bl.x, tr.y}, {metrics.uvBottomLeft.x, metrics.uvTopRight.y});
    verts[4] = CharacterVertex({tr.x, bl.y}, {metrics.uvTopRight.x, metrics.uvBottomLeft.y});
    verts[5] = CharacterVertex(tr, metrics.uvTopRight);

    memcpy(vertices, verts, 6 * sizeof(CharacterVertex));


    pen.x += metrics.advance;
    vertices += 6;
  }
skipFirst:


  for (size_t i = 1; i < line.size(); i++) {
    unsigned char c = line[i];
    CharacterMetrics const& metrics = bitmap[c];

    if (info.showAt != nullptr && at.x == info.showAt->x && at.y == info.showAt->y) {
      vertices = AddBarAt(vertices, {pen.x + metrics.advance, pen.y}, info, fontSize);
      info.showAt->z = at.z;
    }
    at.x++;
    at.z++;


    if (std::isspace(c)) {
      pen.x += metrics.advance;
      continue;
    }


    IVector2 bl = {pen.x + metrics.bearing.x, pen.y - (metrics.size.y - metrics.bearing.y)};
    IVector2 tr = bl + metrics.size;

    pen.x += metrics.advance;
    if (AreaOutOf(area, bl) && AreaOutOf(area, tr)) {
      continue;
    }

    verts[0] = CharacterVertex(bl, metrics.uvBottomLeft);
    verts[1] = CharacterVertex({tr.x, bl.y}, {metrics.uvTopRight.x, metrics.uvBottomLeft.y});
    verts[2] = CharacterVertex({bl.x, tr.y}, {metrics.uvBottomLeft.x, metrics.uvTopRight.y});

    verts[3] = CharacterVertex({bl.x, tr.y}, {metrics.uvBottomLeft.x, metrics.uvTopRight.y});
    verts[4] = CharacterVertex({tr.x, bl.y}, {metrics.uvTopRight.x, metrics.uvBottomLeft.y});
    verts[5] = CharacterVertex(tr, metrics.uvTopRight);

    memcpy(vertices, verts, 6 * sizeof(CharacterVertex));

    vertices += 6;
  }

  return vertices;
}




CharacterVertex* AddBarAt(CharacterVertex* vertices, IVector2 pos, RenderTextInfo& info, int fontSize) {
  IVector2 tr = {pos.x + 2, pos.y + fontSize};

  CharacterVertex verts[6] = {
    CharacterVertex(pos, -2),
    CharacterVertex({tr.x, pos.y}, -2),
    CharacterVertex({pos.x, tr.y}, -2),


    CharacterVertex({pos.x, tr.y}, -2),
    CharacterVertex({tr.x, pos.y}, -2),
    CharacterVertex(tr, -2),
  };

  memcpy(vertices, verts, 6 * sizeof(CharacterVertex));
  vertices += 6;
  return vertices;
}




bool FileExists(char const* file) {
  struct _stat buffer;
  if (_stat(file, &buffer) == 0)
    return true;
  return false;
}





bool AreaOutOf(IVector4 area, IVector2 vec) {
  return vec.x < area.x || vec.y < area.y || vec.x > area.z || vec.y > area.w;
}




} // namespace