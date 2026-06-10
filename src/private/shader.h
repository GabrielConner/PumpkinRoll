#ifndef PUMPKIN_ROLL_SRC_PRIVATE_SHADER_H
#define PUMPKIN_ROLL_SRC_PRIVATE_SHADER_H

#include "pumpkin/types.h"
#include <string>
#include <map>
#include <set>


namespace pumpkin {

struct Shader {
  unsigned int shader = 0;
  std::map<std::string, VariableType> vars;
  std::set<Model*> models = std::set<Model*>();

  void RenderAll();  
  bool AddModel(Model* model);
  void RemoveModel(Model* model);

  void Delete();


  Shader(unsigned int Shader) : shader(Shader) {}
};

}; // namespace pumpkin

#endif