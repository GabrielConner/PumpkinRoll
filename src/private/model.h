#ifndef PUMPKIN_ROLL_SRC_PRIVATE_MODEL_H
#define PUMPKIN_ROLL_SRC_PRIVATE_MODEL_H

#include "pumpkin/types.h"

#include <set>
#include <string>
#include <map>


namespace pumpkin {

struct Model {
  std::set<Object*> instances;
  std::map<std::string, VariableType> vars;
  Mesh* mesh = nullptr;
  Shader* shader = nullptr;


  void RenderAll();
  bool AddObject(Object* obj);
  void RemoveObject(Object* obj);
  void Delete();
};

}; // namepspace pumpkin

#endif