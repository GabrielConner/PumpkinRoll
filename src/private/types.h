#ifndef PUMPKIN_ROLL_SRC_PRIVATE_TYPES_H
#define PUMPKIN_ROLL_SRC_PRIVATE_TYPES_H

#include "pumpkin/types.h"

namespace pumpkin_private {

struct ObjectInternal {
  char* name;
  ::pumpkin::Model* model;
};

#define pObjInt(obj) ((ObjectInternal*)obj)
#define pObjDefInt(obj, name) ObjectInternal* name = (ObjectInternal*)obj->internal

}; // namespace pumpkin_private

#endif