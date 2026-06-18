#include "private/devSaveData.h"
#include "private/propertyHolder.h"
#include "private/model.h"
#include "private/shader.h"
#include "private/mesh.h"

#include <assert.h>


using namespace ::pPack;
using namespace ::pumpkin;
using namespace ::pumpkin_private;

namespace {

void CopyObject(Object* obj, ObjectSaveData& data);
void CopyPropertyHolder(PropertyHolder& from, PropertyHolder& to);

}; // namespace



namespace pumpkin_private {

void SaveData::Pull(Pumpkin* pumpkin) {
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


  for (auto& objectP : pumpkin->registeredObjects) {
    auto& object = objectP.second;

    if (dynamic_cast<Camera*>(object)) continue;

    ObjectSaveData save;
    CopyObject(object, save);
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


  bool firstCamera = true;
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

    if (firstCamera) {
      SetPrimaryCamera(camera);
      firstCamera = false;
    }


    for (std::string const& script : save.objectInfo.scripts) {
      Object_AddScript(camera, script);
    }
    camera->transform = save.objectInfo.transform;
  }


  for (auto& saveP : objectSaves) {
    auto& save = saveP.second;

    Object* object = RegisterObject(save.name);
    if (!object) continue;

    Object_SetModel(object, GetModel(save.model));

    for (std::string const& script : save.scripts) {
      Object_AddScript(object, script);
    }
    object->transform = save.transform;
  }
}





void SaveData::Save(SaveData& diff) {
  
}





void SaveData::Load() {

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
}


}; // namespace pumpkin_private



namespace {

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