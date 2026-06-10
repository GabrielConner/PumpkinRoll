#include "private/model.h"
#include "private/shader.h"
#include "private/mesh.h"
#include "pumpkin/pumpkinRoll.h"
#include "private/pumpkinRoll.h"
#include "pumpkin/constants.h"
#include "glad/glad.h"

#include "pPack/shaderHandling.h"

#include "glm/ext.hpp"
#include "glm/common.hpp"

#include <assert.h>

using namespace ::pumpkin;
using namespace ::pumpkin_private;
using namespace ::pPack;


namespace pumpkin {

void Model::RenderAll() {
  if (!mesh) return;

  // Apply shader vars
  for (auto& var : vars) {
    ApplyShaderVariable(var.first, var.second);
  }

  mesh->Setup();

  // Draw instances
  MatrixWrapper mat = MatrixWrapper();
  for (auto& obj : instances) {
    Transform_GenerateModel(obj->transform, mat);

    ShaderHandler::SetMat4("model", (float*)&mat);
    
    glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
  }
}



bool Model::AddObject(Object* obj) {
  assert(obj);
  return instances.insert(obj).second;
}



void Model::RemoveObject(Object* obj) {
  assert(obj);
  instances.erase(obj);
}



void Model::Delete() {
  if (shader) shader->RemoveModel(this);
  mesh = nullptr;
  shader = nullptr;

  for (auto& var : vars) {
    if (var.second.type & var_type::CONTROL) delete(var.second.variable);
  }
  vars.clear();
}

}; // namespace pumpkin