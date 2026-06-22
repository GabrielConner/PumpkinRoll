#include "private/devSaveData.h"
#include "private/propertyHolder.h"
#include "private/model.h"
#include "private/shader.h"
#include "private/mesh.h"
#include "pumpkin/fileManager.h"

#include <filesystem>
#include <fstream>
#include <format>
#include <assert.h>


using namespace ::pPack;
using namespace ::pumpkin;
using namespace ::pumpkin_private;

namespace {

void CopyObject(Object* obj, ObjectSaveData& data);
void CopyPropertyHolder(PropertyHolder& from, PropertyHolder& to);

std::string ReadString(std::ifstream& stream);


#define WriteLine(line) stream << line << '\n'


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
  object.scripts.push_back(ReadString(stream)); \
}


}; // namespace



namespace pumpkin_private {

void SaveData::Pull(Pumpkin* pumpkin, std::unordered_map<size_t, Object*> runtimeObjects) {
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
  Camera* pCam = GetPrimaryCamera();
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
    }

    objectSaves.insert({_STRING_HASHER(save.name), save});
  }
}





void SaveData::Push(Pumpkin* pumpkin) {
  assert(pumpkin);

  SetPrimaryCamera(nullptr);

  DeleteObjects();


  for (auto& saveP : shaderSaves) {
    auto& save = saveP.second;

    Shader* shader = GetShader(save.name);
    if (!shader) {
      continue;
    }

    CopyPropertyHolder(save.properties, shader->properties);
    shader->startInfos = save.startInfos;
    shader->Reload();
  }


  for (auto& saveP : modelSaves) {
    auto& save = saveP.second;

    Model* model = GetModel(save.name);
    if (!model) {
      continue;
    }

    CopyPropertyHolder(save.properties, model->properties);
    Model_SetShader(model, GetShader(save.shader));
    Model_SetMesh(model, GetMesh(save.mesh));
  }


  for (auto& saveP : cameraSaves) {
    auto& save = saveP.second;

    auto camera = RegisterCamera(save.objectInfo.name);
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
  SetPrimaryCamera(GetCamera(primaryCamera));



  for (auto& saveP : objectSaves) {
    auto& save = saveP.second;

    Object* object = nullptr;
    if (save.runtime) {
      object = RegisterObject(save.name);
    } else {
      object = GetObject(save.name);
      if (object)
        ClearScripts(object);
    }
    if (!object) continue;

    Object_SetModel(object, GetModel(save.model));

    for (std::string const& script : save.scripts) {
      Object_AddScript(object, script);
    }
    object->transform = save.transform;
  }
}




void SaveData::Save(std::string const& name, std::string const& defaultPrimaryCamera) {
  std::string relative = ToRelativePath(name + _DEV_SAVE_FILE);
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
    stream.write(save.mesh.c_str(), save.mesh.size() + 1); // Mesh name

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
  std::string relative = ToRelativePath(name + _DEV_SAVE_FILE);
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
      startInfo.shaderCount = startInfo.shaders.size();
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
    save.mesh = ReadString(stream); // Mesh name
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
  std::string relative = std::filesystem::path(ToRelativePath(path + _DEV_SAVE_FILE) + ".cpp").lexically_normal().string();
  std::ofstream stream(relative, std::ios::trunc);

  WriteLine("#include \"pumpkin/types.h\"");
#ifdef PUMPKIN_ROLL_PROD
  WriteLine("#include \"private/functions.h\"");
#endif
  WriteLine("#include \"pPack/vector.h\"");

  WriteLine("using namespace ::pPack;");
  WriteLine("using namespace ::pumpkin;");

  WriteLine("int SceneBuild() {");
  WriteLine("int tmpInt;float tmpFloat;MatrixWrapper tmpMat;Vector2 tmpVec2;Vector3 tmpVec3;Vector4 tmpVec4;");

  uint32_t counter = 0;

  for (auto& save : shaderSaves) {
    std::string shaderName = std::format("shader_{:}", save.first);

    auto cmpSave = compare.shaderSaves.find(save.first);
    PropertyHolder const* propertiesCmp = nullptr;
    if (cmpSave != compare.shaderSaves.end()) {
      propertiesCmp = &cmpSave->second.properties;
    }

    WriteLine(std::format("Shader* {:} = GetShader({:?});", shaderName, save.second.name));
    WriteLine(std::format("if ({:}) {{", shaderName));
    WriteLine(std::format("PropertyHolder* prop = Shader_GetProperties({:});", shaderName));

    PropertyHolder& properties = save.second.properties;


    if (propertiesCmp != nullptr) {
      for (auto& cmpPropP : propertiesCmp->properties) {
        if (properties.properties.contains(cmpPropP.first)) continue;
        WriteLine(std::format("PropertyHolder_DeleteProperty(prop, {:?});", cmpPropP.second.name));
      }
    }

    for (auto& propP : properties) {
      auto& p = propP.second;
      if (propertiesCmp != nullptr) {
        auto propCmp = propertiesCmp->properties.find(propP.first);
        if (propCmp != propertiesCmp->properties.end() && memcmp(p.prop, propCmp->second.prop, p.typeSize) == 0) {
          continue;
        }
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

    WriteLine("}");
  }

  WriteLine("return 0;\n}");

  stream.flush();
  stream.close();
}



}; // namespace pumpkin_private



namespace {


// My interpretation of the std::string template for ifstream formatted extract but with 0 as the delimiter
std::string ReadString(std::ifstream& stream) {
  std::string ret = std::string();

  int c = 0;
  do {
    c = stream.get();
    if (c == stream.eof() || !stream.good()) { return ret; }

    ret.append(1, c);
  } while (c != '\0');

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

    data.scripts.push_back(script.name);
  }
}





void CopyPropertyHolder(PropertyHolder& from, PropertyHolder& to) {
  to.DeleteAll();

  for (auto& propP : from.properties) {
    auto& prop = propP.second;

    void* data = malloc(SizeOfType(prop.type));
    if (data == nullptr) {
      throw std::bad_alloc();
    }
    memcpy(data, prop.prop, prop.typeSize);

    to.properties.insert({_STRING_HASHER(prop.name), Property(prop.name, data, prop.type)});
  }
}


}; // namespace