#ifndef PPM_INCLUDE_PPACK_MODEL_H
#define PPM_INCLUDE_PPACK_MODEL_H

#include <string>

#include "pPack/vector.h"


namespace pPack {

enum TextureType { ALBEDO = 0, METALLIC, ROUGHNESS };

enum TextureLocation { INTERNAL = 0, EXTERNAL = 1 };

struct Vertex {
  Vector3 position;
  Vector3 normal;
  Vector4 color;
  Vector2 uv;
};

struct Texture {

  uint32_t width;
  uint32_t height;
  uint8_t channels;

  uint8_t* data;

  ~Texture() {
    if (data && width != 0 && channels != 0)
      delete[](data);
  }
};

struct UnknownTexture {
  TextureLocation location;
  TextureType type;

  union {
    uint32_t textureIndex;
    Texture* texture;
  };
};

struct Material {
  UnknownTexture* textures;
  uint8_t textureCount;

  Vector4 albedo;
  float metallic;
  float roughness;

  ~Material() {
    if (textures != nullptr && textureCount != 0) {
      for (uint32_t i = 0; i < textureCount; i++)
        if (textures[i].location == TextureLocation::EXTERNAL)
          delete(textures[i].texture);
      delete[](textures);
    }
  }
};

struct Mesh {
  uint64_t vertexCount;
  Vertex* vertices;
  uint32_t materialIndex;

  ~Mesh() {
    if (vertices != nullptr && vertexCount != 0)
      delete[](vertices);
  }
};

} // namespace pPack

#endif