#include "private/propertyHolder.h"
#include "pumpkin/constants.h"
#include "pumpkin/pumpkinRoll.h"

#include "pPack/vector.h"

#include <new>
#include <ranges>
#include <assert.h>

using namespace ::pumpkin;
using namespace ::pumpkin_private;


namespace pumpkin {

bool PropertyHolder_AddProperty(PropertyHolder* holder, std::string const& name, void* value, VariableType type) {
  pNullCheck(holder, false);

  if (type == VariableType::UNKNOWN) {
    pWarn("Invalid variable type");
    return false;
  }

  size_t typeSize = SizeOfType(type);

  auto ret = holder->properties.insert({_STRING_HASHER(name), Property(name, nullptr, type)});
  if (!ret.second) return false;

  ret.first->second.prop = calloc(1, typeSize);
  if (ret.first->second.prop == nullptr) {
    return false;
  }
  if (value) {
    memcpy(ret.first->second.prop, value, typeSize);
  }

  return true;
}



bool PropertyHolder_SetProperty(PropertyHolder* holder, std::string const& name, void* value) {
  pNullCheck(holder, false);
  pNullCheck(value, false);

  auto find = holder->properties.find(_STRING_HASHER(name));
  if (find == holder->properties.end()) {
    pWarn("Not a valid property name");
    return false;
  }

  std::memcpy(find->second.prop, value, find->second.typeSize);
  return true;
}



bool PropertyHolder_SetOrAddProperty(PropertyHolder* holder, std::string const& name, void* value, VariableType type) {
  pNullCheck(holder, false);


  // Combination of both Set and Add

  auto find = holder->properties.find(_STRING_HASHER(name));
  if (find == holder->properties.end()) {
    if (type == VariableType::UNKNOWN) {
      pWarn("Invalid variable type");
      return false;
    }

    size_t typeSize = SizeOfType(type);

    auto ret = holder->properties.insert({_STRING_HASHER(name), Property(name, nullptr, type)});
    if (!ret.second) return false;

    ret.first->second.prop = calloc(1, typeSize);
    if (ret.first->second.prop == nullptr) {
      return false;
    }
    if (value) {
      memcpy(ret.first->second.prop, value, typeSize);
    }

    return true;
  }
  pNullCheck(value, false);

  std::memcpy(find->second.prop, value, find->second.typeSize);
  return true;
}





void PropertyHolder_DeleteProperty(PropertyHolder* holder, std::string const& name) {
  assert(holder);
  holder->DeleteProperty(name);
}





void* PropertyHolder_GetProperty(PropertyHolder* holder, std::string const& name) {
  pNullCheck(holder, nullptr);

  auto find = holder->properties.find(_STRING_HASHER(name));
  if (find == holder->properties.end()) return nullptr;
  return find->second.prop;
}




template<typename T>
T& PropertyHolder::GetProperty(std::string const& name) {

}



void PropertyHolder::PrintAll() const {
  size_t numElements = 0;
  size_t i = 0;
  bool intType = false;
  for (auto& prop : properties) {
    std::cout << prop.second.name << " : \n";
    numElements = prop.second.typeSize / sizeof(float);
    intType = prop.second.type == VariableType::INT;

    if (prop.second.type == VariableType::MAT4) { // Had to be different to display as column-major (i think)
      MatrixWrapper* wrap = (MatrixWrapper*)prop.second.prop;
      for (int j = 0; j < 4; j++) {
        std::cout << "[ ";
        for (int i = 0; i < 4; i++) {
          std::cout << *((float*)&wrap->cols[i] + j);
          if (i != 3) {
            std::cout << ", ";
          }
        }
        std::cout << " ]";
        if (j != 3) std::cout << "\n";
      }
    } else {
      std::cout << "[ ";
      for (i = 0; i < numElements; i++) {
        if (intType) std::cout << *((int32_t*)prop.second.prop + i);
        else  std::cout << *((float*)prop.second.prop + i);
        if (i != numElements - 1) std::cout << ", ";
      }
      std::cout << " ]";
    }
    std::cout << "\n\n\n";
  }
}


void PropertyHolder::DeleteProperty(std::string const& name) {
  auto find = properties.find(_STRING_HASHER(name));
  if (find == properties.end()) return;
  find->second.Delete();
  properties.erase(find);
}


void PropertyHolder::DeleteAll() {
  for (auto& prop : properties) {
    prop.second.Delete();
  }
  properties.clear();
}

}; // namespace pumpkin






namespace pumpkin_private {

size_t SizeOfType(VariableType const& type) {
  switch (type) {
      return 0;
    case VariableType::INT:
      return sizeof(int);
    case VariableType::FLOAT:
      return sizeof(float);
    case VariableType::MAT4:
      return sizeof(MatrixWrapper);
    case VariableType::VECTOR2:
      return sizeof(::pPack::Vector2);
    case VariableType::VECTOR3:
      return sizeof(::pPack::Vector3);
    case VariableType::VECTOR4:
      return sizeof(::pPack::Vector4);
    case VariableType::UNKNOWN:
    default:
      return 0;
  }
}


}; // namespace pumpkin_private