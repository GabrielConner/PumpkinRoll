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

bool PropertyHolder_AddProperty(PropertyHolder* holder, std::string const& name, void* value, VariableType type, bool handle) {
  pNullCheck(holder, false);

  if (type == VariableType::UNKNOWN) {
    pWarn("Invalid variable type");
    return false;
  }

  size_t typeSize = SizeOfType(type);

  if (value == nullptr || handle) {
    void* tmpValue = calloc(1, typeSize);
    if (tmpValue == nullptr) {
      throw std::bad_alloc();
    }

    if (value) {
      memcpy(tmpValue, value, typeSize);
    }
    value = tmpValue;
    handle = true;
  }

  return holder->properties.insert({_STRING_HASHER(name), Property(name, value, SizeOfType(type), type, handle)}).second;
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



bool PropertyHolder_SetOrAddProperty(PropertyHolder* holder, std::string const& name, void* value, VariableType type, bool handle) {
  pNullCheck(holder, false);


  // Combination of both Set and Add

  auto find = holder->properties.find(_STRING_HASHER(name));
  if (find == holder->properties.end()) {
    if (type == VariableType::UNKNOWN) {
      pWarn("Invalid variable type");
      return false;
    }

    if (value == nullptr) {
      handle = true;
      value = calloc(1, SizeOfType(type));
    }

    return holder->properties.insert({_STRING_HASHER(name), Property(name, value, SizeOfType(type), type, handle)}).second;
  }
  pNullCheck(value, false);

  std::memcpy(find->second.prop, value, find->second.typeSize);
  return true;
}



void* PropertyHolder_GetProperty(PropertyHolder* holder, std::string const& name) {
  pNullCheck(holder, nullptr);

  auto find = holder->properties.find(_STRING_HASHER(name));
  if (find == holder->properties.end()) return nullptr;
  return find->second.prop;
}








/*bool PropertyHolder::AddProperty(std::string const& name, std::string const& prop, VariableType type) {
  
}


bool PropertyHolder::SetProperty(std::string const& name, std::string const& value) {

}



bool PropertyHolder::SetOrAddProperty(std::string const& name, std::string const& value) {

}*/



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

    if (prop.second.type == VariableType::MAT4) {
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
    case VariableType::UNKNOWN:
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
  }
}



/*void ParseVariable(void* output, std::string const& string, ::pumpkin::VariableType const& type) {
  switch (type) {
    case VariableType::UNKNOWN:
      return;

    case VariableType::INT:
      *(int*)output = std::stoi(string);
      break;

    case VariableType::FLOAT:
      *(int*)output = std::stof(string);
      break;

    case VariableType::MAT4:
    {
      // Split string by special delimiter for strings
      auto split = std::views::split(string, _DELIMITER);
      MatrixWrapper* matrix = (MatrixWrapper*)output;

      int i = 0;
      for (auto it : split) {

        // Goes through max 4 split strings
        if (i >= 4) break;

        std::string_view v = std::string_view(it);

        // Parse string into corresponding matrix column
        matrix->cols[i] = ::pPack::stov4(std::string(std::string_view(it)));
        i++;
      }
    }


    break;

    case VariableType::VECTOR2:

      break;

    case VariableType::VECTOR3:

      break;

    case VariableType::VECTOR4:

      break;
  }
}*/



}; // namespace pumpkin_private