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
  for (auto& prop : properties) {
    ApplyShaderVariable(prop.second);
  }

  if (setup) setup();

  // Get camera
  Camera* camera = Pumpkin_GetPrimaryCamera();
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
  properties.DeleteAll();
}



bool Shader::AddModel(Model* model) {
  assert(model);
  return models.insert(model).second;
}



void Shader::RemoveModel(Model* model) {
  assert(model);
  models.erase(model);
}



void Shader::Reload() {
  glDeleteProgram(shader);

  // Ignore the warnings, it works
  ShaderCreateInfo* infos = new ShaderCreateInfo[startInfos.size()];
  for (int i = 0; i < startInfos.size(); i++) {
    infos[i].shaders = new char const*[startInfos[i].shaderCount];
    for (int j = 0; j < startInfos[i].shaderCount; j++) {
      infos[i].shaders[j] = startInfos[i].shaders[j].data();
    }
    infos[i].shaderCount = startInfos[i].shaderCount;
    infos[i].type = startInfos[i].type;
  }

  shader = ShaderHandler::CreateShader(infos, startInfos.size(), Pumpkin_OpenFileFunc, nullptr, Pumpkin_CloseFileFunc);

  for (int i = 0; i < startInfos.size(); i++) {
    delete[](infos[i].shaders);
  }
  delete[](infos);
}


}; // namespace pumpkin