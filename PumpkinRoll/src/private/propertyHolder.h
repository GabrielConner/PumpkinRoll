#ifndef PUMPKIN_ROLL_SRC_PRIVATE_PROPERTY_HOLDER_H
#define PUMPKIN_ROLL_SRC_PRIVATE_PROPERTY_HOLDER_H

#include <unordered_map>
#include <string>
#include <algorithm>
#include "pumpkin/types.h"

namespace pumpkin_private {

size_t SizeOfType(::pumpkin::VariableType const& type);

}; // namespace pumpkin_private

namespace pumpkin {

struct Property {
  std::string name = "";
  void* prop = 0;
  size_t typeSize = 0;
  ::pumpkin::VariableType type = VariableType::UNKNOWN;

  bool Create() {
    Delete();
    typeSize = ::pumpkin_private::SizeOfType(type);
    if (typeSize == 0) return false;

    prop = malloc(typeSize);
    return prop;
  }
  void Delete() { free(prop); prop = nullptr; }

  Property() = default;
  Property(std::string Name, void* Prop, ::pumpkin::VariableType Type) : name(Name), prop(Prop), type(Type) {
    typeSize = ::pumpkin_private::SizeOfType(Type);
  }
};


inline bool operator ==(Property const& a, Property const& b) {
  if (a.typeSize == 0 || !a.prop || !b.prop) return false;
  return a.name == b.name && a.type == b.type && memcmp(a.prop, b.prop, a.typeSize);
}




struct PropertyHolder {
  std::unordered_map<size_t, Property> properties;


/*  bool AddProperty(std::string const& name, std::string const& value, ::pumpkin::VariableType type);
  bool SetProperty(std::string const& name, std::string const& value);
  bool SetOrAddProperty(std::string const& name, std::string const& value);*/

  template<typename T>
  T& GetProperty(std::string const& name);

  void PrintAll() const;

  void DeleteProperty(std::string const& name);
  void DeleteAll();



  std::unordered_map<size_t, Property>::iterator begin() { return properties.begin(); }
  std::unordered_map<size_t, Property>::iterator end() { return properties.end(); }
};


inline bool operator ==(PropertyHolder const& a, PropertyHolder const& b) {
  if (&a == &b) return true;
  return std::equal(a.properties.begin(), a.properties.end(), b.properties.begin(), b.properties.end());
}

}; // namespace pumpkin

#endif