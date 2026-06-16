#ifndef PUMPKIN_ROLL_SRC_PRIVATE_PROPERTY_HOLDER_H
#define PUMPKIN_ROLL_SRC_PRIVATE_PROPERTY_HOLDER_H

#include <unordered_map>
#include <string>
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
  bool handle = false;

  void Delete() { if (handle) free(prop); }

  Property(std::string Name, void* Prop, size_t TypeSize, ::pumpkin::VariableType Type, bool Handle) : name(Name), prop(Prop), typeSize(TypeSize), type(Type), handle(Handle) {}
};



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



}; // namespace pumpkin

#endif