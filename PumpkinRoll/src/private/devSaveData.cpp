#ifdef PUMPKIN_ROLL_DEV

#include "private/devSaveData.h"
#include "private/propertyHolder.h"
#include "private/model.h"
#include "private/shader.h"
#include "private/mesh.h"

#include <filesystem>
#include <fstream>
#include <format>
#include <chrono>
#include <assert.h>


using namespace ::pPack;
using namespace ::pumpkin;
using namespace ::pumpkin_private;

namespace {

void CopyObject(Object* obj, ObjectSaveData& data);
void CopyPropertyHolder(PropertyHolder& from, PropertyHolder& to);

std::string ReadString(std::ifstream& stream);


#ifdef PUMPKIN_ROLL_DEV
#define WriteLine(line) stream << line << '\n'
#else
#define WriteLine(line) stream << line
#endif


#define WriteProperties \
variableHolder = static_cast<uint32_t>(save.properties.properties.size()); \
stream.write((char*)&variableHolder, 4); \
for (auto& propP : save.properties) { \
  auto& prop = propP.second; \
  \
  stream.write(prop.name.c_str(), prop.name.size() + 1); \
  variableHolder = static_cast<uint32_t>(prop.type); \
  stream.write((char*)&variableHolder, 4); \
  stream.write((char*)prop.prop, prop.typeSize); \
}


#define WriteObject \
stream.write((char*)&object.runtime, 1); \
stream.write((char*)&object.transform, sizeof(Transform)); \
stream.write(object.model.c_str(), object.model.size() + 1); \
variableHolder = static_cast<uint32_t>(object.scripts.size()); \
stream.write((char*)&variableHolder, 4); \
for (std::string const& script : object.scripts) { \
  stream.write(script.c_str(), script.size() + 1); \
}



#define ReadStream(output, size)  \
stream.read((char*)output, size); \
if (!stream.good() || stream.gcount() < size) { stream.close(); Delete(); return false; }




#define ReadProperties \
uint32_t propertyCount; \
ReadStream(&propertyCount, 4); \
for (int propertyIndex = 0; propertyIndex < propertyCount; propertyIndex++) { \
  Property property; \
  property.name = ReadString(stream); \
  ReadStream(&variableHolder, 4); \
  property.type = static_cast<VariableType>(variableHolder); \
  size_t size = SizeOfType(property.type); \
  if (size == 0) { \
    stream.close(); \
    Delete(); \
    return false; \
  } \
  property.prop = malloc(size); \
  property.typeSize = size; \
  ReadStream(property.prop, size); \
  save.properties.properties.insert({_STRING_HASHER(property.name), property}); \
}



#define ReadObject \
ReadStream(&object.runtime, 1); \
ReadStream(&object.transform, sizeof(Transform)); \
object.model = ReadString(stream); \
uint32_t scriptCount; \
ReadStream(&scriptCount, 4); \
for (uint32_t scriptIndex = 0; scriptIndex < scriptCount; scriptIndex++) { \
  object.scripts.insert(ReadString(stream)); \
}



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
    save.startInfos = shader->startInfos;

    shaderSaves.insert({_STRING_HASHER(save.name), save});
  }


  for (auto& modelP : pumpkin->registeredModels) {
    auto& model = modelP.second;

    ModelSaveData save;
    CopyPropertyHolder(model->properties, save.properties);
    save.shader = model->shader->name;
    save.mesh = model->mesh->name;
    save.name = model->name;

    modelSaves.insert({_STRING_HASHER(save.name), save});
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

    CopyObject(camera, save.objectInfo);

    cameraSaves.insert({_STRING_HASHER(save.objectInfo.name), save});
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
    CopyObject(object, save);
    if (runtimeObjects.contains(_STRING_HASHER(save.name))) {
      save.runtime = true;
    } else {
      save.runtime = false;
    }

    objectSaves.insert({_STRING_HASHER(save.name), save});
  }
}





void SaveData::Push(Pumpkin* pumpkin, std::unordered_map<size_t, Object*>& runtimeObjects, SaveData const& compare) {
  assert(pumpkin);

  Pumpkin_SetPrimaryCamera(nullptr);

  for (int i = 0; i < pumpkin->registeredObjects.size(); i++) {
    auto ref = std::next(pumpkin->registeredObjects.begin(), i);
    if ((!runtimeObjects.contains(_STRING_HASHER(pObjInt(ref->second)->name)) && !compare.objectSaves.contains(_STRING_HASHER(pObjInt(ref->second)->name))) || dynamic_cast<Camera*>(ref->second)) {
      ::pumpkin::Pumpkin_DeleteObject(pObjInt(ref->second)->name);
      i--;
      continue;
    }
  }
  pumpkin->registeredCameras.clear();


  for (auto& saveP : shaderSaves) {
    auto& save = saveP.second;

    Shader* shader = Pumpkin_GetShader(save.name);
    if (!shader) {
      continue;
    }

    CopyPropertyHolder(save.properties, shader->properties);
    shader->startInfos = save.startInfos;
    shader->Reload();
  }


  for (auto& saveP : modelSaves) {
    auto& save = saveP.second;

    Model* model = Pumpkin_GetModel(save.name);
    if (!model) {
      continue;
    }

    CopyPropertyHolder(save.properties, model->properties);
    Model_SetShader(model, Pumpkin_GetShader(save.shader));
    Model_SetMesh(model, Pumpkin_GetMesh(save.mesh));
  }


  for (auto& saveP : cameraSaves) {
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

    for (std::string const& script : save.objectInfo.scripts) {
      Object_AddScript(camera, script);
    }
    camera->transform = save.objectInfo.transform;
  }
  Pumpkin_SetPrimaryCamera(Pumpkin_GetCamera(primaryCamera));



  for (auto& saveP : objectSaves) {
    auto& save = saveP.second;

    Object* object = nullptr;
    if (save.runtime) {
      object = Pumpkin_RegisterObject(save.name);
    } else {
      object = Pumpkin_GetObject(save.name);
      if (object)
        ClearScripts(object);
    }
    if (!object) continue;

    if ((!pObjInt(object)->model && save.model.size() != 0) || (pObjInt(object)->model && pObjInt(object)->model->name != save.model)) {
      Object_SetModel(object, Pumpkin_GetModel(save.model));
    }

    ClearScripts(object);
    for (std::string const& script : save.scripts) {
      Object_AddScript(object, script);
    }
    object->transform = save.transform;
    if (save.runtime) {
      runtimeObjects.insert({_STRING_HASHER(save.name), object});
    }
  }
}




void SaveData::Save(std::string const& name, std::string const& defaultPrimaryCamera) {
  std::string relative = Pumpkin_ToRelativePath(name + _DEV_SAVE_FILE);
  std::ofstream stream(relative, std::ios::binary | std::ios::trunc);

  // Only object can be created in dev mode
  // So only it can contain differences in elements
  // All others will have differences instead in properties
  //for (auto& diffObject : diff.objectSaves) objectSaves.erase(_STRING_HASHER(diffObject.second.name));
  // Maybe not


  uint32_t variableHolder;


  variableHolder = static_cast<uint32_t>(shaderSaves.size());
  stream.write((char*)&variableHolder, 4); // Number of shaders
  for (auto& shaderP : shaderSaves) {
    auto& save = shaderP.second;
    stream.write(save.name.c_str(), save.name.size() + 1); // Name of shader

    variableHolder = static_cast<uint32_t>(save.startInfos.size());
    stream.write((char*)&variableHolder, 4); // Number of linked shaders
    for (ShaderInfo const& info : save.startInfos) {
      variableHolder = static_cast<uint32_t>(info.shaders.size());
      stream.write((char*)&variableHolder, 4); // Number of files in shader
      for (std::string const& shader : info.shaders) {
        stream.write(shader.c_str(), shader.size() + 1); // Shader path location
      }

      variableHolder = static_cast<uint32_t>(info.type);
      stream.write((char*)&variableHolder, 4); // Type of shader
    }

    WriteProperties;
  }


  variableHolder = static_cast<uint32_t>(modelSaves.size());
  stream.write((char*)&variableHolder, 4); // Number of models
  for (auto& modelP : modelSaves) {
    auto& save = modelP.second;
    stream.write(save.name.c_str(), save.name.size() + 1); // Name of model
    stream.write(save.shader.c_str(), save.shader.size() + 1); // Shader name
    stream.write(save.mesh.c_str(), save.mesh.size() + 1); // LoadedModelMesh name

    WriteProperties;
  }


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
    WriteObject;
  }


  variableHolder = static_cast<uint32_t>(objectSaves.size());
  stream.write((char*)&variableHolder, 4); // Number of objects
  for (auto& objectP : objectSaves) {
    auto& object = objectP.second;

    stream.write(object.name.c_str(), object.name.size() + 1); // Name of object
    WriteObject;
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
  ReadStream(&shaderCount, 4); // Number of shaders
  for (uint32_t shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++) {
    ShaderSaveData save;
    save.name = ReadString(stream); // Name of shader
    uint32_t startInfoCount;
    ReadStream(&startInfoCount, 4); // Number of linked shaders
    for (uint32_t startInfoIndex = 0; startInfoIndex < startInfoCount; startInfoIndex++) {
      ShaderInfo startInfo;
      uint32_t fileCount;
      ReadStream(&fileCount, 4); // Number of files in shader
      for (uint32_t fileIndex = 0; fileIndex < fileCount; fileIndex++) {
        startInfo.shaders.push_back(ReadString(stream)); // Shader path location
      }
      ReadStream(&variableHolder, 4); // Type of shader
      startInfo.type = variableHolder;

      save.startInfos.push_back(startInfo);
    }
    ReadProperties;

    shaderSaves.insert({_STRING_HASHER(save.name), save});
  };


  uint32_t modelCount;
  ReadStream(&modelCount, 4); // Number of models
  for (uint32_t modelIndex = 0; modelIndex < modelCount; modelIndex++) {
    ModelSaveData save;
    save.name = ReadString(stream); // Name of model
    save.shader = ReadString(stream); // Shader name
    save.mesh = ReadString(stream); // LoadedModelMesh name
    ReadProperties;

    modelSaves.insert({_STRING_HASHER(save.name), save});
  }


  uint32_t cameraCount;
  ReadStream(&cameraCount, 4); // Number of cameras
  for (uint32_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
    CameraSaveData save;
    save.objectInfo.name = ReadString(stream); // Name of camera

    ReadStream(&save.angleBased, 1);
    ReadStream(&save.fov, 4);
    ReadStream(&save.aspect, 4);
    ReadStream(&save.near, 4);
    ReadStream(&save.far, 4);
    ReadStream(&save.perspective, 1);

    ObjectSaveData& object = save.objectInfo;
    ReadObject;

    cameraSaves.insert({_STRING_HASHER(save.objectInfo.name), save});
  }



  uint32_t objectCount;
  ReadStream(&objectCount, 4); // Number of objects
  for (uint32_t objectIndex = 0; objectIndex < objectCount; objectIndex++) {
    ObjectSaveData object;
    object.name = ReadString(stream); // Name of object
    ReadObject;

    objectSaves.insert({_STRING_HASHER(object.name), object});
  }


  primaryCamera = ReadString(stream); // Primary camera
  stream.close();

  return true;
}





void SaveData::Delete() {
  for (auto& elem : shaderSaves) {
    for (auto& props : elem.second.properties) {
      free(props.second.prop);
    }
  }
  for (auto& elem : modelSaves) {
    for (auto& props : elem.second.properties) {
      free(props.second.prop);
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
    for (std::string const& script : save.second.scripts) {
      if (save.second.scripts.contains(script)) continue;

      WriteLine(std::format("Object_RemoveScript({:}, {:?});", objectName, script));
    }
  }

  for (std::string const& script : save.second.scripts) {
    if (cmpSave == nullptr || !cmpSave->second.scripts.contains(script)) {
      WriteLine(std::format("Object_AddScript({:}, {:?});", objectName, script));
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
std::string ReadString(std::ifstream& stream) {
  std::string ret = std::string();

  int c = stream.get();
  while (c != '\0') {
    if (stream.eof() || !stream.good()) { return ret; }

    ret.append(1, c);
    c = stream.get();
  }

  return ret;
}


void CopyObject(Object* obj, ObjectSaveData& data) {
  assert(obj);

  pObjDefInt(obj, i);
  pObjDefExt(obj, e);

  data.transform = obj->transform;
  data.name = i->name;
  data.model = i->model ? i->model->name : "";

  for (auto& scriptP : e->scripts) {
    auto& script = scriptP.second;

    data.scripts.insert(script.name);
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

    to.properties.insert({_STRING_HASHER(prop.name), Property(prop.name, data, prop.type)});
  }
}


}; // namespace

#endif