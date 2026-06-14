#ifndef PUMPKIN_ROLL_SRC_PRIVATE_PUMPKIN_ROLL_H
#define PUMPKIN_ROLL_SRC_PRIVATE_PUMPKIN_ROLL_H

#include "pumpkin/pumpkinRoll.h"
#include "pumpkin/types.h"
#include "private/propertyHolder.h"
#include "pPack/windowManager.h"
#include "pPack/vector.h"
#include "glad/glad.h"

#include <string>
#include <unordered_map>


namespace pumpkin_private {

struct Pumpkin {
  std::unordered_map<size_t, ::pPack::Window*> registeredWindows = std::unordered_map<size_t, ::pPack::Window*>();
  std::unordered_map<size_t, ::pumpkin::Shader*> registeredShaders = std::unordered_map<size_t, ::pumpkin::Shader*>();
  std::unordered_map<size_t, ::pumpkin::Mesh*> registeredMeshes = std::unordered_map<size_t, ::pumpkin::Mesh*>();
  std::unordered_map<size_t, ::pumpkin::Model*> registeredModels = std::unordered_map<size_t, ::pumpkin::Model*>();
  std::unordered_map<size_t, ::pumpkin::Camera*> registeredCameras = std::unordered_map<size_t, ::pumpkin::Camera*>();
  std::unordered_map<size_t, ::pumpkin::Object*> registeredObjects = std::unordered_map<size_t, ::pumpkin::Object*>();
  std::unordered_map<size_t, GLuint> registeredFormats = std::unordered_map<size_t, GLuint>();

  ::pPack::Window* primaryWindow = nullptr;
  ::pumpkin::Camera* primaryCamera = nullptr;
  std::string exePath = "";
  GLuint globalVBO = 0;

  ::pumpkin::RuntimeSettings runtime = ::pumpkin::RuntimeSettings();
};


bool RegisterWindow(::std::string const& name, ::pPack::Window* data);
::pPack::Window* GetWindow(::std::string const& name);

::pumpkin_private::Pumpkin* GetPumpkin();

void ApplyShaderVariable(::pumpkin::Property const& var);

void DeleteObject(::pumpkin::Object* obj);

}; // namespace pumpkin_private

#endif