#ifndef PUMPKIN_ROLL_SRC_PRIVATE_DEV_SAVE_DATA_H
#define PUMPKIN_ROLL_SRC_PRIVATE_DEV_SAVE_DATA_H

#include "pumpkin/types.h"
#include "private/types.h"
#include "private/pumpkinRoll.h"
#include "private/propertyHolder.h"
#include <set>
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
  std::set<std::string> scripts = std::set<std::string>();
  bool runtime;
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
  std::unordered_map<size_t, ObjectSaveData> objectSaves = std::unordered_map<size_t, ObjectSaveData>();
  std::unordered_map<size_t, CameraSaveData> cameraSaves = std::unordered_map<size_t, CameraSaveData>();

  std::string primaryCamera;


  void Pull(Pumpkin* pumpkin, std::unordered_map<size_t, ::pumpkin::Object*> const& runtimeObjects);
  void Push(Pumpkin* pumpkin, std::unordered_map<size_t, ::pumpkin::Object*>& runtimeObjects);

  // Saves the current pulled data, make sure to pull right before save to get latest
  void Save(std::string const& name, std::string const& defaultPrimaryCamera);
  bool Load(std::string const& name);

  void Delete();

  void Build(std::string const& path, SaveData const& compare);
};

}; // namespace pumpkin_private

#endif