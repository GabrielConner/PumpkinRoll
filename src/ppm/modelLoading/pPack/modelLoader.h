#ifndef PPM_INCLUDE_PPACK_MODEL_LOADING_H
#define PPM_INCLUDE_PPACK_MODEL_LOADING_H

#include <string>

#include "pPack/model.h"


namespace pPack {

struct ModelObject {
  ModelObject** children;
  uint32_t* meshIndices;


  uint32_t meshCount;
  uint32_t childrenCount;

  float relativeTransformation[4][4];


  ~ModelObject() {
    if (meshIndices != nullptr && meshCount != 0)
      delete[](meshIndices);
    if (children != nullptr && childrenCount != 0) {
      for (uint32_t i = 0; i < childrenCount; i++)
        if (children[i])
          delete(children[i]);
      delete[](children);
    }
  }
};

struct ModelScene {
  ::pPack::Material** materials;
  ::pPack::Mesh** meshes;
  ::pPack::Texture** textures;
  ModelObject* rootModelObject;

  uint32_t materialCount;
  uint32_t meshCount;
  uint32_t textureCount;


  ~ModelScene() {
    if (rootModelObject != nullptr)
      delete(rootModelObject);
    if (textures != nullptr && textureCount != 0) {
      for (uint32_t i = 0; i < textureCount; i++)
        if (textures[i] != nullptr)
          delete(textures[i]);
      delete[](textures);
    }
    if (materials != nullptr && materialCount != 0) {
      for (uint32_t i = 0; i < materialCount; i++)
        if (materials[i] != nullptr)
          delete(materials[i]);
      delete[](materials);
    }
    if (meshes != nullptr && meshCount != 0) {
      for (uint32_t i = 0; i < meshCount; i++)
        if (meshes[i] != nullptr)
          delete(meshes[i]);
      delete[](meshes);
    }
  }
};


ModelScene* LoadModel(std::string location);

} // namespace pPack

#endif