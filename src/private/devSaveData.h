#ifndef PUMPKIN_ROLL_SRC_PRIVATE_DEV_SAVE_DATA_H
#define PUMPKIN_ROLL_SRC_PRIVATE_DEV_SAVE_DATA_H

#include "pumpkin/types.h"
#include "private/types.h"
#include "private/pumpkinRoll.h"
#include "private/propertyHolder.h"
#include <unordered_map>
#include <string>

namespace pumpkin_private {


struct ShaderSaveData {
  ::pumpkin::PropertyHolder properties;
  std::vector<::pumpkin::ShaderInfo> startInfos;
  std::string name;
};

struct ModelSaveData {
  ::pumpkin::PropertyHolder properties;
  std::string shader;
  std::string mesh;
  std::string name;
};

struct ObjectSaveData {
  ::pumpkin::Transform transform;
  std::string name;
  std::string model;
  std::vector<std::string> scripts = std::vector<std::string>();
};

struct CameraSaveData {
  ObjectSaveData objectInfo;

  bool angleBased;
  float fov;
  float aspect;
  float near;
  float far;
  bool perspective;
};




struct SaveData {

  std::unordered_map<size_t, ShaderSaveData> shaderSaves = std::unordered_map<size_t, ShaderSaveData>();
  std::unordered_map<size_t, ModelSaveData> modelSaves = std::unordered_map<size_t, ModelSaveData>();
  std::unordered_map<size_t, CameraSaveData> cameraSaves = std::unordered_map<size_t, CameraSaveData>();
  std::unordered_map<size_t, ObjectSaveData> objectSaves = std::unordered_map<size_t, ObjectSaveData>();


  void Pull(Pumpkin* pumpkin);
  void Push(Pumpkin* pumpkin);

  void Save(SaveData& diff);
  void Load();

  void Delete();
};

}; // namespace pumpkin_private

#endif