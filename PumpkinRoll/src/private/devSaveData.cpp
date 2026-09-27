#ifdef PUMPKIN_ROLL_DEV

#include "private/devSaveData.h"
#include "private/propertyHolder.h"
#include "private/model.h"
#include "private/shader.h"
#include "private/mesh.h"

#include <filesystem>
#include <sstream>
#include <fstream>
#include <format>
#include <chrono>
#include <assert.h>


using namespace ::pPack;
using namespace ::pumpkin;
using namespace ::pumpkin_private;

namespace {

void CopyObjectToSave(Object* obj, ObjectSaveData& data);
void CopyPropertyHolder(PropertyHolder& from, PropertyHolder& to);

std::string ReadString(std::istream& stream);
inline bool ReadStream(std::istream& stream, void* output, size_t size);


#ifdef PUMPKIN_ROLL_DEV
#define WriteLine(line) stream << line << '\n'
#else
#define WriteLine(line) stream << line
#endif



void Build_CameraHeader(std::ostream& stream, std::string const& cameraName, std::pair<size_t, CameraSaveData> const& save);
void Build_Camera(std::ostream& stream, std::string const& cameraName, std::pair<size_t, CameraSaveData> const& save, std::pair<const size_t, CameraSaveData> const* cmpSave);
void Build_CameraEnd(std::ostream& stream, std::pair<size_t, CameraSaveData> const& save);

void Build_ObjectDelete(std::ostream& stream, std::pair<size_t, ObjectSaveData> const& save);
void Build_ObjectHeader(std::ostream& stream, std::string const& objectName, std::pair<size_t, ObjectSaveData> const& save);
void Build_ObjectSetModel(std::ostream& stream, std::string const& objectName, std::string const& modelName);
void Build_Object(std::ostream& stream, std::string const& objectName, std::pair<size_t, ObjectSaveData> const& save, std::pair<const size_t, ObjectSaveData> const* cmpSave);
void Build_ObjectEnd(std::ostream& stream, std::pair<size_t, ObjectSaveData> const& save);

void Build_ModelHeader(std::ostream& stream, std::string const& shaderName, std::pair<size_t, ModelSaveData> const& save);
void Build_ModelEnd(std::ostream& stream, std::pair<size_t, ModelSaveData> const& save);
void Build_ModelGetProperties(std::ostream& stream, std::string const& shaderName);
void Build_ModelSetShader(std::ostream& stream, std::string const& modelName, std::string const& shaderName);
void Build_ModelSetMesh(std::ostream& stream, std::string const& modelName, std::string const& meshName);

void Build_ShaderHeader(std::ostream& stream, std::string const& shaderName, std::pair<size_t, ShaderSaveData> const& save);
void Build_ShaderEnd(std::ostream& stream, std::pair<size_t, ShaderSaveData> const& save);
void Build_ShaderGetProperties(std::ostream& stream, std::string const& shaderName);

void Build_PropertiesHeader(std::ostream& stream);
void Build_Properties(std::ostream& stream, PropertyHolder const& properties, PropertyHolder const* propertiesCmp, void (*getFunc)(std::ostream& stream, std::string const& name), std::string const& name);

}; // namespace



namespace pumpkin_private {

void SaveData::Pull(Pumpkin* pumpkin, std::unordered_map<size_t, Object*> const& runtimeObjects) {
  assert(pumpkin);
  Delete();

  for (auto& shaderP : pumpkin->registeredShaders) {
    auto& shader = shaderP.second;

    ShaderSaveData save;
    CopyPropertyHolder(shader->properties, save.properties);
    save.name = shader->name;

    shaderSaves.insert({_PR_STRING_HASHER(save.name), save});
  }


  for (auto& modelP : pumpkin->registeredModels) {
    auto& model = modelP.second;

    ModelSaveData save;
    CopyPropertyHolder(model->properties, save.properties);
    save.shader = model->shader->name;
    save.mesh = model->mesh->name;
    save.name = model->name;

    modelSaves.insert({_PR_STRING_HASHER(save.name), save});
  }


  for (auto& cameraP : pumpkin->registeredCameras) {
    auto& camera = cameraP.second;

    CameraSaveData save;
    save.angleBased = pCamInt(camera)->angleBased;
    save.fov = camera->fov;
    save.aspect = camera->aspect;
    save.far = camera->far;
    save.near = camera->near;
    save.perspective = camera->perspective;

    CopyObjectToSave(camera, save.objectInfo);

    cameraSaves.insert({_PR_STRING_HASHER(save.objectInfo.name), save});
  }
  Camera* pCam = Pumpkin_GetPrimaryCamera();
  if (pCam) {
    primaryCamera = pObjInt(pCam)->name;
  } else {
    primaryCamera = "";
  }

  for (auto& objectP : pumpkin->registeredObjects) {
    auto& object = objectP.second;

    if (dynamic_cast<Camera*>(object)) continue;


    ObjectSaveData save;
    CopyObjectToSave(object, save);
    if (runtimeObjects.contains(_PR_STRING_HASHER(save.name))) {
      save.runtime = true;
    } else {
      save.runtime = false;
    }

    objectSaves.insert({_PR_STRING_HASHER(save.name), save});
  }
}





void SaveData::Push(Pumpkin* pumpkin, std::unordered_map<size_t, Object*>& runtimeObjects) {
  assert(pumpkin);

  Pumpkin_SetPrimaryCamera(nullptr);

  while (!pumpkin->registeredObjects.empty()) { // Delete all objects and cameras
    auto ref = std::next(pumpkin->registeredObjects.begin(), 0);
    ::pumpkin::Pumpkin_DeleteObject(pObjInt(ref->second)->name);
/*    if ((!runtimeObjects.contains(_STRING_HASHER(pObjInt(ref->second)->name)) && !compare.objectSaves.contains(_STRING_HASHER(pObjInt(ref->second)->name))) || dynamic_cast<Camera*>(ref->second)) {
      ::pumpkin::Pumpkin_DeleteObject(pObjInt(ref->second)->name);
      i--;
      continue;
    }*/
  }
  pumpkin->registeredCameras.clear(); // prtodo I knew there was a problem and fixed it with a bandaid


  for (auto& saveP : shaderSaves) { // Shaders
    auto& save = saveP.second;

    Shader* shader = Pumpkin_GetShader(save.name);
    if (!shader) {
      continue;
    }

    CopyPropertyHolder(save.properties, shader->properties);
  }


  for (auto& saveP : modelSaves) { // Models
    auto& save = saveP.second;

    Model* model = Pumpkin_GetModel(save.name);
    if (!model) {
      continue;
    }

    CopyPropertyHolder(save.properties, model->properties);
    Model_SetShader(model, Pumpkin_GetShader(save.shader));
    Model_SetMesh(model, Pumpkin_GetMesh(save.mesh));
  }


  for (auto& saveP : cameraSaves) { // Cameras
    auto& save = saveP.second;

    auto camera = Pumpkin_RegisterCamera(save.objectInfo.name);
    if (!camera) {
      continue;
    }

    pCamInt(camera)->angleBased = save.angleBased;
    camera->fov = save.fov;
    camera->aspect = save.aspect;
    camera->far = save.far;
    camera->near = save.near;
    camera->perspective = save.perspective;

    for (auto const& script : save.objectInfo.scripts) {
      Script* scr = Object_AddScript(camera, script.second.name);
      if (scr) {
        scr->LoadProperties(script.second.properties);
      }
    }
    camera->transform = save.objectInfo.transform;
  }
  Pumpkin_SetPrimaryCamera(Pumpkin_GetCamera(primaryCamera));



  for (auto& saveP : objectSaves) { // Objects
    auto& save = saveP.second;

    Object* object = nullptr;
    object = Pumpkin_RegisterObject(save.name);
    if (!object) continue;

    if ((!pObjInt(object)->model && save.model.size() != 0) || (pObjInt(object)->model && pObjInt(object)->model->name != save.model)) {
      Object_SetModel(object, Pumpkin_GetModel(save.model));
    }

    ClearScripts(object);
    for (auto const& script : save.scripts) {
      Script* scr = Object_AddScript(object, script.second.name);
      if (scr) {
        scr->LoadProperties(script.second.properties);
      }
    }
    object->transform = save.transform;
    if (save.runtime) {
      runtimeObjects.insert({_PR_STRING_HASHER(save.name), object});
    }
  }
}




void SaveData::Save(std::string const& name, std::string const& defaultPrimaryCamera, SaveData const& compare) {
  std::string relative = Pumpkin_ToRelativePath(name + _DEV_SAVE_FILE);
  std::ofstream stream(relative, std::ios::binary | std::ios::trunc);

  // Only object can be created in dev mode
  // So only it can contain differences in elements
  // All others will have differences instead in properties
  //for (auto& diffObject : diff.objectSaves) objectSaves.erase(_STRING_HASHER(diffObject.second.name));
  // Maybe not


  uint32_t variableHolder;
  std::ostringstream tempStream; 


  uint32_t writtenShaders = 0;
  for (auto& shaderP : shaderSaves) {
    auto& save = shaderP.second;

    auto const& cmp = compare.shaderSaves.find(shaderP.first);
    if (cmp != compare.shaderSaves.end() && save.properties == cmp->second.properties) continue;

    tempStream.write(save.name.c_str(), save.name.size() + 1); // Name of shader
    WriteProperties(tempStream, save.properties);
    writtenShaders++;
  }
  stream.write((char*)&writtenShaders, 4); // Number of shaders
  stream << std::move(tempStream).str();
  tempStream.str() = std::string();
  
  

  uint32_t writtenModels = 0;
  for (auto& modelP : modelSaves) {
    auto& save = modelP.second;

    auto const& cmp = compare.modelSaves.find(modelP.first);
    if (cmp != compare.modelSaves.end() && (
      save.shader == cmp->second.shader  ||
      save.mesh == cmp->second.mesh ||
      save.properties == cmp->second.properties
    )) continue;

    tempStream.write(save.name.c_str(), save.name.size() + 1); // Name of model
    tempStream.write(save.shader.c_str(), save.shader.size() + 1); // Shader name
    tempStream.write(save.mesh.c_str(), save.mesh.size() + 1); // Mesh name

    WriteProperties(tempStream, save.properties);
    writtenModels++;
  }
  stream.write((char*)&writtenModels, 4); // Number of models
  stream << std::move(tempStream).str();
  tempStream.str() = std::string();



  variableHolder = static_cast<uint32_t>(cameraSaves.size());
  stream.write((char*)&variableHolder, 4); // Number of cameras
  for (auto& cameraP : cameraSaves) {
    auto& save = cameraP.second;
    stream.write(save.objectInfo.name.c_str(), save.objectInfo.name.size() + 1); // Name of camera

    stream.write((char*)&save.angleBased, 1); // bools shouldn't be more than 1 byte, I will ignore any weird ones
    stream.write((char*)&save.fov, 4);
    stream.write((char*)&save.aspect, 4);
    stream.write((char*)&save.near, 4); // Data for orthogonal of width/height is shared within due to union
    stream.write((char*)&save.far, 4);
    stream.write((char*)&save.perspective, 1);


    auto& object = save.objectInfo;
    WriteObject(stream, object);
  }


  variableHolder = static_cast<uint32_t>(objectSaves.size());
  stream.write((char*)&variableHolder, 4); // Number of objects
  for (auto& objectP : objectSaves) {
    auto& object = objectP.second;

    stream.write(object.name.c_str(), object.name.size() + 1); // Name of object
    WriteObject(stream, object);
  }

  stream.write(defaultPrimaryCamera.c_str(), defaultPrimaryCamera.size() + 1); // Primary camera


  stream.flush();
  stream.close();
}





bool SaveData::Load(std::string const& name) {
  std::string relative = Pumpkin_ToRelativePath(name + _DEV_SAVE_FILE);
  if (!std::filesystem::exists(relative)) return false;

  Delete();

  std::ifstream stream(relative, std::ios::binary);

  uint32_t variableHolder;

  uint32_t shaderCount;
  ReadStream(stream, &shaderCount, 4); // Number of shaders
  for (uint32_t shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++) {
    ShaderSaveData save;
    save.name = ReadString(stream); // Name of shader
    if (!ReadProperties(stream, save.properties)) continue;

    shaderSaves.insert({_PR_STRING_HASHER(save.name), save});
  };


  uint32_t modelCount;
  ReadStream(stream, &modelCount, 4); // Number of models
  for (uint32_t modelIndex = 0; modelIndex < modelCount; modelIndex++) {
    ModelSaveData save;
    save.name = ReadString(stream); // Name of model
    save.shader = ReadString(stream); // Shader name
    save.mesh = ReadString(stream); // LoadedModelMesh name
    if (!ReadProperties(stream, save.properties)) continue;

    modelSaves.insert({_PR_STRING_HASHER(save.name), save});
  }


  uint32_t cameraCount;
  ReadStream(stream, &cameraCount, 4); // Number of cameras
  for (uint32_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
    CameraSaveData save;
    save.objectInfo.name = ReadString(stream); // Name of camera

    ReadStream(stream, &save.angleBased, 1);
    ReadStream(stream, &save.fov, 4);
    ReadStream(stream, &save.aspect, 4);
    ReadStream(stream, &save.near, 4);
    ReadStream(stream, &save.far, 4);
    ReadStream(stream, &save.perspective, 1);

    ObjectSaveData& object = save.objectInfo;
    if (!ReadObject(stream, object)) continue;

    cameraSaves.insert({_PR_STRING_HASHER(save.objectInfo.name), save});
  }



  uint32_t objectCount;
  ReadStream(stream, &objectCount, 4); // Number of objects
  for (uint32_t objectIndex = 0; objectIndex < objectCount; objectIndex++) {
    ObjectSaveData object;
    object.name = ReadString(stream); // Name of object
    if (!ReadObject(stream, object)) continue;

    objectSaves.insert({_PR_STRING_HASHER(object.name), object});
  }


  primaryCamera = ReadString(stream); // Primary camera
  stream.close();

  return true;
}





void SaveData::Delete() {
  for (auto& elem : shaderSaves) {
    for (auto& prop : elem.second.properties) {
      prop.second.Delete();
    }
  }
  for (auto& elem : modelSaves) {
    for (auto& prop : elem.second.properties) {
      prop.second.Delete();
    }
  }
  for (auto& elem : objectSaves) {
    for (auto& script : elem.second.scripts) {
      for (auto& prop : script.second.properties) {
        prop.DeleteIfCreated();
      }
    }
  }
  for (auto& elem : cameraSaves) {
    for (auto& script : elem.second.objectInfo.scripts) {
      for (auto& prop : script.second.properties) {
        prop.DeleteIfCreated();
      }
    }
  }

  shaderSaves.clear();
  modelSaves.clear();
  cameraSaves.clear();
  objectSaves.clear();

  primaryCamera.clear();
}



void SaveData::Build(std::string const& path, SaveData const& compare) {
  std::string relative = std::filesystem::path(Pumpkin_ToRelativePath(path + _DEV_SAVE_FILE) + ".cpp").lexically_normal().string();

  #ifndef PUMPKIN_ROLL_FAUX_BUILD
  std::ofstream stream(relative, std::ios::trunc);
  #else
  std::ostringstream stream;
  #endif



  auto now = std::chrono::system_clock::now();

  Pumpkin_StartMemoryIgnoreBlock();
  // prtodo load tzdb beforehand to prevent this
  auto local = std::chrono::zoned_time{std::chrono::current_zone(), now}; // TZDB singleton gets loaded with 'current_zone'; needs to be ignored by memory leak check
  Pumpkin_EndMemoryIgnoreBlock();

  WriteLine(std::format("/*\n*\n*\n* Pumpkin Roll Build File\n* Built {:%F %I:%M %p}\n*\n*/\n", local));

  WriteLine("#include \"pumpkin/types.h\"");
  WriteLine("#include \"pumpkinFunctions.h\"");
  WriteLine("#include \"pPack/vector.h\"");

  WriteLine("using namespace ::pPack;");
  WriteLine("using namespace ::pumpkin;");

  WriteLine("int SceneBuild() {");
  WriteLine("int tmpInt;float tmpFloat;MatrixWrapper tmpMat;Vector2 tmpVec2;Vector3 tmpVec3;Vector4 tmpVec4;");

  for (auto& save : shaderSaves) {
    std::string shaderName = std::format("shader_{:}", save.first);

    auto cmpSave = compare.shaderSaves.find(save.first);
    PropertyHolder const* propertiesCmp = nullptr;
    if (cmpSave != compare.shaderSaves.end()) {
      propertiesCmp = &cmpSave->second.properties;
    }
    PropertyHolder& properties = save.second.properties;

    std::ostringstream propStream;
    Build_Properties(propStream, properties, propertiesCmp, Build_ShaderGetProperties, shaderName);

    if (propStream.tellp() != 0) {
      Build_ShaderHeader(stream, shaderName, save);
      stream << std::move(propStream).str();
      Build_ShaderEnd(stream, save);
    }
  }


  for (auto& save : modelSaves) {
    std::string modelName = std::format("model_{:}", save.first);
    auto cmpSave = compare.modelSaves.find(save.first);
    PropertyHolder const* propertiesCmp = nullptr;
    if (cmpSave != compare.modelSaves.end()) {
      propertiesCmp = &cmpSave->second.properties;
    }
    PropertyHolder& properties = save.second.properties;

    std::ostringstream modelStream;
    std::ostringstream propStream;

    Build_Properties(propStream, properties, propertiesCmp, Build_ModelGetProperties, modelName);

    if (propStream.str().size() != 0) {
      modelStream << std::move(propStream).str();
    }


    if (cmpSave != compare.modelSaves.end()) {
      if (save.second.shader != cmpSave->second.shader)
        Build_ModelSetShader(modelStream, modelName, save.second.shader);

      if (save.second.mesh != cmpSave->second.mesh)
        Build_ModelSetMesh(modelStream, modelName, save.second.mesh);
    }

    if (modelStream.str().size() != 0) {
      Build_ModelHeader(stream, modelName, save);
      stream << std::move(modelStream).str();
      Build_ModelEnd(stream, save);
    }
  }


  for (auto& delObj : compare.objectSaves) {
    if (objectSaves.contains(delObj.first)) continue;
    Build_ObjectDelete(stream, delObj);
  }
  
  for (auto& save : objectSaves) {
    std::string objectName = std::format("object_{:}", save.first);
    auto cmpSave = compare.objectSaves.find(save.first);

    std::ostringstream objStream;
    Build_Object(objStream, objectName, save, cmpSave == compare.objectSaves.end() ? nullptr : &*cmpSave);

    if (objStream.str().size() != 0) {
      Build_ObjectHeader(stream, objectName, save);
      stream << std::move(objStream).str();
      Build_ObjectEnd(stream, save);
    }
  }


  for (auto& save : cameraSaves) {
    std::string cameraName = std::format("object_{:}", save.first); // uses object name since derived from object
    auto cmpSave = compare.cameraSaves.find(save.first);

    std::ostringstream camStream;
    Build_Camera(camStream, cameraName, save, cmpSave == compare.cameraSaves.end() ? nullptr : &*cmpSave);

    if (camStream.str().size() != 0) {
      Build_CameraHeader(stream, cameraName, save);
      stream << std::move(camStream).str();
      Build_CameraEnd(stream, save);
    }
  }
  WriteLine(std::format("Pumpkin_SetPrimaryCamera(Pumpkin_GetCamera({:?}));", compare.primaryCamera));



  WriteLine("return 0;");
  WriteLine("}");

  stream.flush();

  #ifndef PUMPKIN_ROLL_FAUX_BUILD
  stream.close();
#endif
}



void SaveData::WriteProperties(std::ostream& stream, PropertyHolder const& properties) {
  uint32_t variableHolder = static_cast<uint32_t>(properties.properties.size());
  stream.write((char*)&variableHolder, 4);

  for (auto& propP : properties.properties) {
    auto& prop = propP.second;
    stream.write(prop.name.c_str(), prop.name.size() + 1);

    variableHolder = static_cast<uint32_t>(prop.type);
    stream.write((char*)&variableHolder, 4);
    stream.write((char*)prop.prop, prop.typeSize);
  }
}



bool SaveData::ReadProperties(std::istream& stream, PropertyHolder& properties) {
  uint32_t propertyCount;
  ReadStream(stream, &propertyCount, 4); // Property count

  for (int propertyIndex = 0; propertyIndex < propertyCount; propertyIndex++) { // For all properties
    Property property;
    property.name = ReadString(stream); // Property name

    uint32_t propertyType;
    ReadStream(stream, &propertyType, 4); // Property type
    property.type = static_cast<VariableType>(propertyType);

    if (!property.Create()) continue;
    size_t size = property.typeSize;

    property.typeSize = size;
    ReadStream(stream, property.prop, size); // Property data
    properties.properties.insert({_PR_STRING_HASHER(property.name), property});
  }

  return true;
}



void SaveData::WriteObject(std::ostream& stream, ObjectSaveData const& object) {
  stream.write((char*)&object.runtime, 1);
  stream.write((char*)&object.transform, sizeof(Transform));
  stream.write(object.model.c_str(), object.model.size() + 1);
  uint32_t variableHolder = static_cast<uint32_t>(object.scripts.size());
  stream.write((char*)&variableHolder, 4);

  for (auto const& scrPair : object.scripts) {
    ScriptSaveData const& script = scrPair.second;

    stream.write(script.name.c_str(), script.name.size() + 1);

    variableHolder = static_cast<uint32_t>(script.properties.size());
    stream.write((char*)&variableHolder, 4);

    for (ScriptPropertySaveData const& prop : script.properties) {
      variableHolder = static_cast<uint32_t>(prop.size);
      stream.write((char*)&variableHolder, 4);
      stream.write((char*)prop.data, variableHolder);
    }
  };
}


bool SaveData::ReadObject(std::istream& stream, ObjectSaveData& object) {
  if (!ReadStream(stream, &object.runtime, 1)) { // This is the only one that could cause build errors if read wrong
    return false;
  }

  ReadStream(stream, &object.transform, sizeof(Transform)); // Transform
  object.model = ReadString(stream); // Model type

  uint32_t scriptCount;
  ReadStream(stream, &scriptCount, 4); // Script type

  for (uint32_t scriptIndex = 0; scriptIndex < scriptCount; scriptIndex++) { // For all scripts
    ScriptSaveData data = ScriptSaveData();
    data.name = ReadString(stream); // Script name

    uint32_t propertyCount;
    ReadStream(stream, &propertyCount, 4);  // Script property count

    for (uint32_t propertyIndex = 0; propertyIndex < propertyCount; propertyIndex++) { // For all properties
      ScriptPropertySaveData propertySaveData;
      
      uint32_t propertySize = 0;
      ReadStream(stream, &propertySize, 4); // Property data size
      if (propertySize == 0) continue;

      if (!propertySaveData.Create(propertySize)) continue; // Allocate
      ReadStream(stream, propertySaveData.data, propertySize); // Property data

      data.properties.push_back(propertySaveData);
    }

  }

  return true;
}



}; // namespace pumpkin_private



namespace {


void Build_CameraHeader(std::ostream& stream, std::string const& cameraName, std::pair<size_t, CameraSaveData> const& save) {
  WriteLine("");
  WriteLine(std::format("// Camera {:}", save.second.objectInfo.name));
  WriteLine("// --------------------------------------------------");
  WriteLine(std::format("Camera* {:} = Pumpkin_GetCamera({:?});", cameraName, save.second.objectInfo.name));
  WriteLine(std::format("if ({:}) {{", cameraName));
}


void Build_Camera(std::ostream& stream, std::string const& cameraName, std::pair<size_t, CameraSaveData> const& save, std::pair<const size_t, CameraSaveData> const* cmpSave) {
  if (!cmpSave) {
    WriteLine(std::format("{:}->angleBased = {:};", cameraName, save.second.angleBased));
    WriteLine(std::format("{:}->fov = {:};", cameraName, save.second.fov));
    WriteLine(std::format("{:}->aspect = {:};", cameraName, save.second.aspect));
    WriteLine(std::format("{:}->near = {:};", cameraName, save.second.near));
    WriteLine(std::format("{:}->far = {:};", cameraName, save.second.far));
    WriteLine(std::format("{:}->perspective = {:};", cameraName, save.second.perspective));

    std::string objectName = std::format("object_{:}", save.first);
    WriteLine(std::format("Object* {:} = (Object*){:}", objectName, cameraName));
    std::pair<size_t, ObjectSaveData> build = {save.first, save.second.objectInfo};
    Build_Object(stream, objectName, build, nullptr);
  } else {
    if (save.second.angleBased != cmpSave->second.angleBased)
      WriteLine(std::format("{:}->angleBased = {:};", cameraName, save.second.angleBased));
    if (save.second.fov != cmpSave->second.fov)
      WriteLine(std::format("{:}->fov = {:};", cameraName, save.second.fov));
    if (save.second.aspect != cmpSave->second.aspect)
      WriteLine(std::format("{:}->aspect = {:};", cameraName, save.second.aspect));
    if (save.second.near != cmpSave->second.near)
      WriteLine(std::format("{:}->near = {:};", cameraName, save.second.near));
    if (save.second.far != cmpSave->second.far)
      WriteLine(std::format("{:}->far = {:};", cameraName, save.second.far));
    if (save.second.perspective != cmpSave->second.perspective)
      WriteLine(std::format("{:}->perspective = {:};", cameraName, save.second.perspective));


    std::ostringstream objStream;
    std::string objectName = std::format("object_{:}", save.first);
    std::pair<size_t, ObjectSaveData> build = {save.first, save.second.objectInfo};
    if (cmpSave) {
      std::pair<const size_t, ObjectSaveData> cmp = {save.first, cmpSave->second.objectInfo};
      Build_Object(objStream, objectName, build, &cmp);
    } else {
      Build_Object(objStream, objectName, build, nullptr);
    }

    if (objStream.str().size() != 0) {
      stream << std::move(objStream).str();
    }
  }
}


void Build_CameraEnd(std::ostream& stream, std::pair<size_t, CameraSaveData> const& save) {
  WriteLine("}");
  WriteLine("// --------------------------------------------------");
  WriteLine(std::format("// Camera {:}", save.second.objectInfo.name));
  WriteLine("");
}





void Build_ObjectDelete(std::ostream& stream, std::pair<size_t, ObjectSaveData> const& save) {
  WriteLine(std::format("DeleteObject({:?});", save.second.name));
}


void Build_ObjectHeader(std::ostream& stream, std::string const& objectName, std::pair<size_t, ObjectSaveData> const& save) {
  WriteLine(std::format("\n// Object {:}", save.second.name));
  WriteLine("// --------------------------------------------------");
  if (save.second.runtime) {
    WriteLine(std::format("Object* {:} = Pumpkin_RegisterObject({:?});", objectName, save.second.name));
  } else {
    WriteLine(std::format("Object* {:} = Pumpkin_GetObject({:?});", objectName, save.second.name));
  }
  WriteLine(std::format("if ({:}) {{", objectName));
}


void Build_ObjectSetModel(std::ostream& stream, std::string const& objectName, std::string const& modelName) {
  WriteLine(std::format("Object_SetModel({:}, Pumpkin_GetModel({:?}));", objectName, modelName));
}


void Build_Object(std::ostream& stream, std::string const& objectName, std::pair<size_t, ObjectSaveData> const& save, std::pair<const size_t, ObjectSaveData> const* cmpSave) {
  if (cmpSave == nullptr || memcmp(&save.second.transform, &cmpSave->second.transform, sizeof(Transform)) != 0) {
    WriteLine(std::format("{:}->transform = {:c};", objectName, save.second.transform));
  }

  if (cmpSave == nullptr || save.second.model != cmpSave->second.model) {
    Build_ObjectSetModel(stream, objectName, save.second.model);
  }

  if (cmpSave != nullptr) {
    for (auto const& script : save.second.scripts) {
      if (save.second.scripts.contains(script.first)) continue;

      WriteLine(std::format("Object_RemoveScript({:}, {:?});", objectName, script.second.name));
    }
  }

  for (auto const& script : save.second.scripts) {
    if (cmpSave == nullptr || !cmpSave->second.scripts.contains(script.first)) {
      WriteLine(std::format("Object_AddScript({:}, {:?});", objectName, script.second.name));
    }
  }
}


void Build_ObjectEnd(std::ostream& stream, std::pair<size_t, ObjectSaveData> const& save) {
  WriteLine("}");
  WriteLine("// --------------------------------------------------");
  WriteLine(std::format("// Object {:}", save.second.name));
  WriteLine("");
}




void Build_ModelHeader(std::ostream& stream, std::string const& modelName, std::pair<size_t, ModelSaveData> const& save) {
  WriteLine("");
  WriteLine(std::format("// Model {:}", save.second.name));
  WriteLine("// --------------------------------------------------");
  WriteLine(std::format("Model* {:} = Pumpkin_GetModel({:?});", modelName, save.second.name));
  WriteLine(std::format("if ({:}) {{", modelName));
}


void Build_ModelEnd(std::ostream& stream, std::pair<size_t, ModelSaveData> const& save) {
  WriteLine("}");
  WriteLine("// --------------------------------------------------");
  WriteLine(std::format("// Model {:}", save.second.name));
  WriteLine("");
}


void Build_ModelGetProperties(std::ostream& stream, std::string const& modelName) {
  WriteLine(std::format("PropertyHolder* prop = Model_GetProperties({:});", modelName));
}


void Build_ModelSetShader(std::ostream& stream, std::string const& modelName, std::string const& shaderName) {
  WriteLine(std::format("Model_SetShader({:}, Pumpkin_GetShader({:?}));", modelName, shaderName));
}


void Build_ModelSetMesh(std::ostream& stream, std::string const& modelName, std::string const& meshName) {
  WriteLine(std::format("Model_SetMesh({:}, Pumpkin_GetMesh({:?}));", modelName, meshName));
}





void Build_ShaderHeader(std::ostream& stream, std::string const& shaderName, std::pair<size_t, ShaderSaveData> const& save) {
  WriteLine("");
  WriteLine(std::format("// Shader {:}", save.second.name));
  WriteLine("// --------------------------------------------------");
  WriteLine(std::format("Shader* {:} = Pumpkin_GetShader({:?});", shaderName, save.second.name));
  WriteLine(std::format("if ({:}) {{", shaderName));
}


void Build_ShaderEnd(std::ostream& stream, std::pair<size_t, ShaderSaveData> const& save) {
  WriteLine("}");
  WriteLine("// --------------------------------------------------");
  WriteLine(std::format("// Shader {:}", save.second.name));
  WriteLine("");
}


void Build_ShaderGetProperties(std::ostream& stream, std::string const& shaderName) {
  WriteLine(std::format("PropertyHolder* prop = Shader_GetProperties({:});", shaderName));
}





void Build_PropertiesHeader(std::ostream& stream) {
  WriteLine("");
  WriteLine("// Properties");
  WriteLine("// ----------");
}


void Build_Properties(std::ostream& stream, PropertyHolder const& properties, PropertyHolder const* propertiesCmp, void (*getFunc)(std::ostream& stream, std::string const& name), std::string const& name) {
  assert(getFunc != nullptr);
  bool didSomething = false;

  if (propertiesCmp != nullptr) {
    for (auto const& cmpPropP : propertiesCmp->properties) {
      if (properties.properties.contains(cmpPropP.first)) continue;

      if (!didSomething) {
        didSomething = true;
        Build_PropertiesHeader(stream);
        getFunc(stream, name);
      }

      WriteLine(std::format("PropertyHolder_DeleteProperty(prop, {:?});", cmpPropP.second.name));
    }
  }

  for (auto const& propP : properties.properties) {

    auto& p = propP.second;

    if (propertiesCmp != nullptr) {
      auto propCmp = propertiesCmp->properties.find(propP.first);
      if (propCmp != propertiesCmp->properties.end() && memcmp(p.prop, propCmp->second.prop, p.typeSize) == 0) {
        continue;
      }
    }

    if (!didSomething) {
      didSomething = true;
      Build_PropertiesHeader(stream);
      getFunc(stream, name);
    }

    switch (p.type) {
      case VariableType::UNKNOWN:
        continue;
      case VariableType::INT:
        WriteLine(std::format("tmpInt = {:};", *(int*)p.prop));
        WriteLine(std::format("PropertyHolder_SetOrAddProperty(prop, {:?}, &tmpInt, VariableType::INT);", p.name));
        break;
      case VariableType::FLOAT:
        WriteLine(std::format("tmpFloat = {:};", *(float*)p.prop));
        WriteLine(std::format("PropertyHolder_SetOrAddProperty(prop, {:?}, &tmpFloat, VariableType::FLOAT);", p.name));
        break;
      case VariableType::MAT4:
        WriteLine(std::format("tmpMat = {:c};", *(MatrixWrapper*)p.prop));
        WriteLine(std::format("PropertyHolder_SetOrAddProperty(prop, {:?}, &tmpMat, VariableType::MAT4);", p.name));
        break;
      case VariableType::VECTOR2:
        WriteLine(std::format("tmpVec2 = {:};", *(Vector2*)p.prop));
        WriteLine(std::format("PropertyHolder_SetOrAddProperty(prop, {:?}, &tmpVec2, VariableType::VECTOR2);", p.name));
        break;
      case VariableType::VECTOR3:
        WriteLine(std::format("tmpVec3 = {:};", *(Vector3*)p.prop));
        WriteLine(std::format("PropertyHolder_SetOrAddProperty(prop, {:?}, &tmpVec3, VariableType::VECTOR3);", p.name));
        break;
      case VariableType::VECTOR4:
        WriteLine(std::format("tmpVec4 = {:};", *(Vector4*)p.prop));
        WriteLine(std::format("PropertyHolder_SetOrAddProperty(prop, {:?}, &tmpVec4, VariableType::VECTOR4);", p.name));
        break;
    }
  }

  if (didSomething) {
    WriteLine("// ----------");
    WriteLine("// Properties");
    WriteLine("");
  }
}







// My interpretation of the std::string template for ifstream formatted extract but with 0 as the delimiter
std::string ReadString(std::istream& stream) {
  std::string ret = std::string();

  int c = stream.get();
  while (c != '\0') {
    if (stream.eof() || !stream.good()) { return ret; }

    ret.append(1, c);
    c = stream.get();
  }

  return ret;
}


bool ReadStream(std::istream& stream, void* output, size_t size) {
  stream.read((char*)output, size);
  if (!stream.good() || stream.gcount() < size) {
    memset(output, 0, size); // Set to 0 for safety
    return false;
  }
  return true;
}



void CopyObjectToSave(Object* obj, ObjectSaveData& data) {
  assert(obj);

  pObjDefInt(obj, i);
  pObjDefExt(obj, e);

  data.transform = obj->transform;
  data.name = i->name;
  data.model = i->model ? i->model->name : "";

  for (auto& scriptP : e->scripts) {
    auto& script = scriptP.second;

    ScriptSaveData scriptSave;
    scriptSave.name = script.name;

    for (auto& prop : script.script->SaveProperties()) {
      ScriptPropertySaveData propSave;
      if (!propSave.Create(prop.size)) continue;
      memcpy(propSave.data, prop.data, prop.size);

      scriptSave.properties.push_back(propSave);
    }

    data.scripts.insert({_PR_STRING_HASHER(script.name), scriptSave});
  }
}





void CopyPropertyHolder(PropertyHolder& from, PropertyHolder& to) {
  to.DeleteAll();

  for (auto& propP : from.properties) {
    auto& prop = propP.second;

    void* data = malloc(SizeOfType(prop.type));
    if (data == nullptr) {
      return;
    }
    memcpy(data, prop.prop, prop.typeSize);

    to.properties.insert({_PR_STRING_HASHER(prop.name), Property(prop.name, data, prop.type)});
  }
}


}; // namespace

#endif