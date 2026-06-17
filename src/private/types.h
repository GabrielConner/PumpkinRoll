#ifndef PUMPKIN_ROLL_SRC_PRIVATE_TYPES_H
#define PUMPKIN_ROLL_SRC_PRIVATE_TYPES_H

#include "private/propertyHolder.h"
#include "pumpkin/types.h"
#include "pPack/vector.h"
#include <unordered_map>
#include <set>

namespace pumpkin_private {

struct ObjectInternal {
  char* name;
  ::pumpkin::Model* model;
};

struct CameraInternal {
  ::pPack::Vector3* lookAt;
  ::pPack::Vector3 forward;
  ::pPack::Vector3 right;
  bool angleBased = false;
};

#define pObjInt(obj) ((ObjectInternal*)obj)
#define pObjDefInt(obj, name) ObjectInternal* name = (ObjectInternal*)obj->internal

#define pCamInt(cam) ((CameraInternal*)cam->camInternal)
#define pCamDefInt(cam, name) CameraInternal* name = (CameraInternal*)cam->camInternal


}; // namespace pumpkin_private

#endif