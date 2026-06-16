#ifndef PUMPKIN_ROLL_SRC_PRIVATE_MESH_H
#define PUMPKIN_ROLL_SRC_PRIVATE_MESH_H

#include "pumpkin/types.h"
#include "glad/glad.h"
#include <assert.h>

namespace pumpkin {

struct Mesh {
  GLuint format = 0, VBO = 0, *pVBO = nullptr;
  void* vertices = nullptr;
  size_t vertexCount = 0;
  size_t vertexSize = 0;
  size_t bufferSize = 0;
  size_t offset = 0;
  bool dynamic = false;

  std::string name = "";

  void Setup();

  GLuint GetVBO() const { assert(!dynamic || pVBO); return dynamic ? VBO : *pVBO ; }
  void Delete();
};


}; // namespace pumpkin

#endif