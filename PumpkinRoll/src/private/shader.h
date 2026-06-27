#ifndef PUMPKIN_ROLL_SRC_PRIVATE_SHADER_H
#define PUMPKIN_ROLL_SRC_PRIVATE_SHADER_H

#include "pumpkin/types.h"
#include "private/propertyHolder.h"
#include <string>
#include <map>
#include <set>
#include <vector>


namespace pumpkin {

struct Shader {
  std::vector<ShaderInfo> startInfos;
  unsigned int shader = 0;
  PropertyHolder properties;
  std::set<Model*> models = std::set<Model*>();
  std::string name = "";

  void (*setup)() = nullptr;

  void RenderAll();  
  bool AddModel(Model* model);
  void RemoveModel(Model* model);

  void Reload();

  void Delete();


};

}; // namespace pumpkin

#endif