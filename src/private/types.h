#ifndef PUMPKIN_ROLL_SRC_PRIVATE_TYPES_H
#define PUMPKIN_ROLL_SRC_PRIVATE_TYPES_H

#include "private/propertyHolder.h"
#include "pumpkin/types.h"
#include "pPack/vector.h"
#include <unordered_map>
#include <set>
#include <string>

namespace pumpkin_private {


struct ScriptAddPair {
  ::pumpkin::Script* script = nullptr;
  std::string name = "";

  ScriptAddPair() = default;
  ScriptAddPair(::pumpkin::Script* Script, std::string const& Name) : script(Script), name(Name) {}
};

struct ObjectExternal {
  std::unordered_map<size_t, ScriptAddPair> scripts = std::unordered_map<size_t, ScriptAddPair>();
  std::set<std::pair<::pumpkin::ObjectDeleteCallback, int>> deleteCallbacks = std::set<std::pair<::pumpkin::ObjectDeleteCallback, int>>();
};

struct ObjectInternal {
  char* name;
  ::pumpkin::Model* model;
  ObjectExternal* external;
};

struct CameraInternal {
  ::pPack::Vector3* lookAt;
  ::pPack::Vector3 forward;
  ::pPack::Vector3 right;
  bool angleBased = false;
};


struct ScriptInfoPair {
  ::pumpkin::ScriptAllocateFunction allocate = nullptr;
  std::string name;
  size_t size = 0;

  ScriptInfoPair() = default;
  ScriptInfoPair(::pumpkin::ScriptAllocateFunction const& Allocate, std::string const& Name, size_t const& Size) : allocate(Allocate), name(Name), size(Size) {}
};

#define pObjInt(obj) ((ObjectInternal*)obj->internal)
#define pObjDefInt(obj, name) ObjectInternal* name = (ObjectInternal*)obj->internal
#define pObjExt(obj) (pObjInt(obj)->external)
#define pObjDefExt(obj, name) ObjectExternal* name = (pObjInt(obj)->external)

#define pCamInt(cam) ((CameraInternal*)cam->camInternal)
#define pCamDefInt(cam, name) CameraInternal* name = (CameraInternal*)cam->camInternal


}; // namespace pumpkin_private

#endif