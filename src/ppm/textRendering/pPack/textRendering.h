#ifndef PPM_INCLUDE_PPACK_TEXT_RENDERING_H
#define PPM_INCLUDE_PPACK_TEXT_RENDERING_H

#include "pPack/vector.h"
#include "glad/glad.h"
#include <vector>

namespace pPack {


struct CharacterMetrics {
  IVector2 pixelBottomLeft;
  IVector2 pixelTopRight;

  Vector2 uvBottomLeft;
  Vector2 uvTopRight;

  IVector2 size;
  IVector2 bearing;
  int advance;


  CharacterMetrics();
  CharacterMetrics(IVector2 Size, IVector2 Bearing, float Advance);
};



struct RenderTextInfo {
  float fontScale = 1;
  float lineSize = 1;
  float textWidth = 2;
  float cutoff = 0;

  IVector3* showAt = nullptr;

  bool centerX = false;
  bool centerY = false;

  bool startTop = false;

  Vector2 startPos = {-1.f, 1.f};
  Vector4 area = {-1.f, -1.f, 1.f, 1.f};
  Vector4 color = {0,0,0,1};

  IVector2 frameSize;
};




class Bitmap {
  unsigned int texture;
  IVector2 textureSize;
  int fontSize;

  unsigned char charLow;
  unsigned char charHigh;

  friend Bitmap GenerateBitmap(const char* fontFile, int size, unsigned char low, unsigned char high, int filtering);
  friend void RenderText(const Bitmap& bitmap, std::string& text, RenderTextInfo info);

public:
  std::vector<CharacterMetrics> characterMapping;

  void LoadFromFile(const char* file, int filtering);
  void SaveToFile(const char* file, const char* imageLocation);


  bool Valid() const;


  void Destroy();


  int GetFontSize() const;
  unsigned char GetCharLow() const;
  unsigned char GetCharHigh() const;



  CharacterMetrics const& operator[](unsigned char c) const;



  Bitmap();
};

namespace text_rendering {

void Start(std::string folderLoc);
void End();

}; // namespace text_rendering

Bitmap GenerateBitmap(const char* fontFile, int size, unsigned char low, unsigned char high, int filtering);
void RenderText(const Bitmap& bitmap, std::string& text, RenderTextInfo info);

} // namespace pPack

#endif