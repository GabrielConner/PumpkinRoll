#include "private/shader.h"
#include "private/model.h"
#include "pumpkin/pumpkinRoll.h"
#include "private/pumpkinRoll.h"

#include "pPack/shaderHandling.h"

#include "glad/glad.h"

#include <assert.h>
#include <vector>

using namespace ::pPack;
using namespace ::pumpkin;
using namespace ::pumpkin_private;



namespace pumpkin {

void Shader::RenderAll() {
  assert(shader);
  ::pPack::ShaderHandler::SetCurrentShader(shader);

  // Apply shader vars
  for (auto& var : vars) {
    ApplyShaderVariable(var.first, var.second);
  }

  // Get camera
  Camera* camera = GetPrimaryCamera();
  assert(camera);

  // Apply projection
  ShaderHandler::SetMat4("proj", (float*)&camera->proj);

  // Apply view
  ShaderHandler::SetMat4("view", (float*)&camera->view);

  // Render models
  for (auto& model : models) {
    model->RenderAll();
  }
}



void Shader::Delete() {
  assert(shader);
  glDeleteProgram(shader);

  for (auto& var : vars) {
    if (var.second.type & var_type::CONTROL) delete(var.second.variable);
  }
  vars.clear();
}



bool Shader::AddModel(Model* model) {
  assert(model);
  return models.insert(model).second;
}



void Shader::RemoveModel(Model* model) {
  assert(model);
  models.erase(model);
}


}; // namespace pumpkin