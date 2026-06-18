#include "pumpkin/pumpkinRoll.h"
#include "pumpkin/constants.h"
#include "private/pumpkinRoll.h"
#include "private/fileManager.h"
#include "private/shader.h"
#include "private/camera.h"
#include "pumpkin/fileManager.h"
#include "private/mesh.h"
#include "private/model.h"
#include "private/propertyHolder.h"
#include "private/types.h"
#include "pumpkin/types.h"
#include "private/pumpkinRollDev.h"

#include "pPack/windowManager.h"
#include "pPack/shaderHandling.h"
#include "pPack/timer.h"

#define GLFW_INCLUDE_NONE
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/ext.hpp"
#include "glm/common.hpp"

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>
#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef near
#undef far
#undef GetObject
#undef ERROR

using namespace ::pumpkin;
using namespace ::pumpkin_private;
using namespace ::pPack;


namespace {

Pumpkin* pumpkinData = nullptr;

#ifdef PUMPKIN_ROLL_NO_CHECK

#define CHECK(...) (void)0;

#else

#define pPumpkinCheck(...) if (pumpkinData == nullptr) { pError("PumpkinRoll has not been started"); return __VA_ARGS__; }

#endif

#ifdef PUMPKIN_ROLL_DEV
_CrtMemState crtMemState;
#endif

}; // namespace




namespace pumpkin {

struct TempScript : Script {
  double totalTime = 0;

  void Update(Object* obj, double deltaTime) override {
    totalTime += deltaTime;
    obj->transform.position.y = sin(totalTime);
  }
};
ScriptAllocateFunction(TempScript);



StartReturn Init(StartSettings const& start) {

#ifdef PUMPKIN_ROLL_DEV
  _CrtMemCheckpoint(&crtMemState);
#endif


  // START OF FAILURE ZONE
  // where calling End would cause more errors


  // Can be start/stopped
  // Can't be start then started
  if (pumpkinData != nullptr) {
    pError("PumpkinRoll already initialized");
    return StartReturn::FAILURE;
  }


  // Create program data
  pumpkinData = new Pumpkin();

#ifdef PUMPKIN_ROLL_DEV
  StartDevelopment();
#endif


  // GLFW
  if (!glfwInit()) {
    pError("Failed to initialize GLFW");
    delete(pumpkinData);
    return StartReturn::FAILURE;
  }


  // Open main window
  WindowCreateHint hints[] = {
        WindowCreateHint(GLFW_RESIZABLE, start.resizable),
        WindowCreateHint(GLFW_CONTEXT_VERSION_MAJOR, 4),
        WindowCreateHint(GLFW_CONTEXT_VERSION_MINOR, 6),
  };
  Window* mainWindow = new Window();
  mainWindow->Open(start.width, start.height, start.title, hints, sizeof(hints) / sizeof(WindowCreateHint));
  if (!mainWindow->IsOpen()) {
    pError("Failed to open primary window");
    delete(pumpkinData);
    delete(mainWindow);
    return StartReturn::FAILURE;
  }
  mainWindow->SetAsContext();
  mainWindow->SetSwapInterval(1);

  // Register window
  if (!pumpkin_private::RegisterWindow("pumpkin_roll__primary_window", mainWindow)) {
    pError("Failed to register primary window");
    delete(pumpkinData);
    delete(mainWindow);
    return StartReturn::FAILURE;
  }
  pumpkinData->primaryWindow = mainWindow;

  // Start OpenGL
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    pError("Failed to load OpenGL");
    delete(pumpkinData);
    delete(mainWindow);
    return StartReturn::FAILURE;
  }

  // Get path of exe
  CHAR fileName[512];
  memset(fileName, 0, sizeof(fileName));
  DWORD ret = GetModuleFileNameA(NULL, fileName, 512);
  if (!ret || GetLastError()) {
    pWarn("Failed to retrieve executable path");
  } else {
    pumpkinData->exePath = std::filesystem::path(fileName).parent_path().concat("\\").lexically_normal().string();
  }


  if (!FileManagerStart()) {
    pError("Failed to start file manager");
    delete(pumpkinData);
    delete(mainWindow);
    return StartReturn::FAILURE;
  }

  // END OF FAILURE ZONE





  // Register and create defaults

  // Camera
  Camera* camera = RegisterCamera("PumpkinRoll__DefaultCamera");
  if (!camera) {
    pError("Failed to create default camera");
    return StartReturn::ERROR;
  }
  if (!SetPrimaryCamera(camera)) {
    pError("Failed to set primary camera");
    return StartReturn::ERROR;
  }
  camera->transform.position.z = 5;


  // Shader
  char const* vertLocs = "shaders/test.vert";
  char const* fragLocs = "shaders/test.frag";

  ShaderInfo shaderCreateInfos[] = {ShaderInfo({vertLocs}, 1, GL_VERTEX_SHADER), ShaderInfo({fragLocs}, 1, GL_FRAGMENT_SHADER)};

  Shader* shader = RegisterShader("PumpkinRoll__DefaultShader", shaderCreateInfos, sizeof(shaderCreateInfos) / sizeof(ShaderInfo));
  if (!shader) {
    pError("Failed to create default shader");
    return StartReturn::ERROR;
  }


  // Format
  FormatStartInfo formatCreateInfos[] = {
    FormatStartInfo{},
    FormatStartInfo{.size = 4},
  };

  GLuint format = RegisterFormat("PumpkinRoll__DefaultFormat", formatCreateInfos, 2, true);
  if (!format) {
    pError("Failed to create default format");
    return StartReturn::ERROR;
  }


  // Mesh
  float planeVerts[] = {
    // Position               // Color
    -1.0f, 0.0f, 1.0f,        1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,         0.0f, 1.0f, 0.0f, 1.0f,
    -1.0f, 0.0f, -1.0f,       0.0f, 0.0f, 1.0f, 1.0f,

    -1.0f, 0.0f, -1.0f,       0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f,         0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, -1.0f,        1.0f, 0.0f, 1.0f, 1.0f
  };

  Mesh* mesh = RegisterMesh("PumpkinRoll__PlaneMesh", planeVerts, 7 * sizeof(float), 6, false, format);
  if (!mesh) {
    pError("Failed to create default mesh");
    return StartReturn::ERROR;
  }



  // Triangle
  float triVerts[] = {
    // Position               // Color
    -1.0f, 0.0f, 1.0f,        1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,         0.0f, 1.0f, 0.0f, 1.0f,
    0.f, 0.0f, -1.0f,         0.0f, 0.0f, 1.0f, 1.0f,
  };

  if (!RegisterMesh("PumpkinRoll__TriangleMesh", triVerts, 7 * sizeof(float), 3, false, format)) {
    pError("Failed to create triangle mesh");
    return StartReturn::ERROR;
  }





  // Model
  Model* model = RegisterModel("PumpkinRoll_PlaneModel");
  if (!model) {
    pError("Failed to create default model");
    return StartReturn::ERROR;
  }
  if (!Model_SetShader(model, shader)) {
    pError("Failed to set model shader");
    return StartReturn::ERROR;
  }
  if (!Model_SetMesh(model, mesh)) {
    pError("Failed to set model mesh");
    return StartReturn::ERROR;
  }
  PropertyHolder_AddProperty(Model_GetProperties(model), "color", nullptr, VariableType::VECTOR4);

  MatrixWrapper identity = MatrixWrapper();
  PropertyHolder_AddProperty(Shader_GetProperties(shader), "adjust", &identity, VariableType::MAT4);



  // Object
  Object* render = RegisterObject("TestObj");
  if (!render) {
    pError("Failed to create TestObj");
    return StartReturn::ERROR;
  }
  Object_SetModel(render, model);
  render->transform.rotation.x = 45;


  ApplyStaticBuffer();



  RegisterScript(AllocateTempScript, TempScript);

  return StartReturn::SUCCESS;
}





void Update() {
  pPumpkinCheck();

  auto& runtime = pumpkinData->runtime;

  Timer& singleton = Timer::GetSingleton();

  // Loop until the main window is closed
  while (!pumpkinData->primaryWindow->ShouldClose()) {
    pumpkinData->deltaTime = singleton.GetDeltaTime();

    glfwPollEvents();

    glClearColor(runtime.backgroundColor.x, runtime.backgroundColor.y, runtime.backgroundColor.z, runtime.backgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    glFinish();

    for (auto& cam : pumpkinData->registeredCameras) {
      UpdateCamera(cam.second);
    }

    for (auto& obj : pumpkinData->registeredObjects) {
      for (auto& script : pObjInt(obj.second)->external->scripts) {
        script.second.script->Update(obj.second, pumpkinData->deltaTime);
      }
    }

    if (pumpkinData->primaryCamera) {
      Camera_GenerateProjection(pumpkinData->primaryCamera);
      Camera_GenerateView(pumpkinData->primaryCamera);

      for (auto& shader : pumpkinData->registeredShaders) {
        shader.second->RenderAll();
      }
    }

#ifdef PUMPKIN_ROLL_DEV
  UpdateDevelopment();
#endif

    pumpkinData->primaryWindow->Swap();
    pumpkinData->primaryWindow->ResetKeys();

    singleton.Advance();
  }
}





void End() {
  pPumpkinCheck();

  // Delete all registered items
  for (auto& window : pumpkinData->registeredWindows) {
    window.second->Close();
    delete(window.second);
  }
  for (auto& obj : pumpkinData->registeredObjects) {
    ::pumpkin_private::DeleteObject(obj.second);
  }
  for (auto& model : pumpkinData->registeredModels) {
    model.second->Delete();
    delete(model.second);
  }
  for (auto& shader : pumpkinData->registeredShaders) {
    shader.second->Delete();
    delete(shader.second);
  }
  for (auto& mesh : pumpkinData->registeredMeshes) {
    mesh.second->Delete();
    delete(mesh.second);
  }
  for (auto& format : pumpkinData->registeredFormats) {
    glDeleteVertexArrays(1, &format.second);
  }

  glDeleteBuffers(1, &pumpkinData->globalVBO);

#ifdef PUMPKIN_ROLL_DEV
  EndDevelopment();
#endif

  FileManagerEnd();

  glfwTerminate();

  // Delete pumpkinData
  delete(pumpkinData);

  // Reset so can be started up again
  pumpkinData = nullptr;



#ifdef PUMPKIN_ROLL_DEV
  _CrtMemState crtEndState;
  _CrtMemState crtDiffState;
  _CrtMemCheckpoint(&crtEndState);
  if (_CrtMemDifference(&crtDiffState, &crtMemState, &crtEndState)) {
    std::cerr << "\n\n\n****MEMORY LEAK****\n";
  }
#endif
}



RuntimeSettings* GetRuntime() {
  pPumpkinCheck(nullptr);
  return &pumpkinData->runtime;
}



#ifndef PUMPKIN_ROLL_DEV
void PrintError(PrintLevel level, char const* file, char const* msg) {
  if (pumpkinData != nullptr && (level == PrintLevel::NOPRINT || pumpkinData->runtime.printLevel > level)) return;
  std::cerr << msg << '\n' << file << "\n\n";
}
#endif


double DeltaTime() {
  pPumpkinCheck(0);
  return pumpkinData->deltaTime;
}





// Object
// --------------------------------------------------
// --------------------------------------------------

Object* RegisterObject(std::string const& name) {
  pPumpkinCheck(nullptr);

  if (name.size() == 0) {
    pWarn("Invalid object name");
    return nullptr;
  }

  auto ret = pumpkinData->registeredObjects.insert({_STRING_HASHER(name), nullptr});
  if (!ret.second) {
    pWarn("Object already exists with same name");
    return nullptr;
  }
  ret.first->second = new Object();

  pObjDefInt(ret.first->second, i);

  i->external = new ObjectExternal();

  i->model = nullptr;

  i->name = new char[name.size() + 1];
  i->name[name.size()] = 0;
  std::memcpy(i->name, name.c_str(), name.size());

  return ret.first->second;
}



Object* GetObject(std::string const& name) {
  pPumpkinCheck(nullptr);

  auto ret = pumpkinData->registeredObjects.find(_STRING_HASHER(name));
  return ret == pumpkinData->registeredObjects.end() ? nullptr : ret->second;
}



bool DeleteObject(std::string const& name) {
  pPumpkinCheck(false);

  auto ret = pumpkinData->registeredObjects.find(_STRING_HASHER(name));
  if (ret == pumpkinData->registeredObjects.end()) {
    pWarn("Object does not exist with name");
    return false;
  }

  for (auto& callbacks : pObjExt(ret->second)->deleteCallbacks) {
    callbacks.first(ret->second, callbacks.second);
  }

  ::pumpkin_private::DeleteObject(ret->second);
  pumpkinData->registeredObjects.erase(ret);
  return true;
}



char const* Object_GetName(Object const* object) {
  pNullCheck(object, nullptr);
  return pObjInt(object)->name;
}



bool Object_SetModel(Object* object, Model* model) {
  pNullCheck(object, false);
  pNullCheck(model, false);

  pObjDefInt(object, i);

  if (!model->AddObject(object)) {
    pWarn("Failed to set object model");
    return false;
  }

  if (i->model != nullptr) i->model->RemoveObject(object);

  i->model = model;

  return true;
}



bool Object_AddScript(Object* object, std::string const& name) {
  pNullCheck(object, false);

  Script* script = CreateScript(name);
  if (script == nullptr) {
    pWarn("Failed to add script with name to object");
    return false;
  }

  auto ret = pObjExt(object)->scripts.insert({_STRING_HASHER(name), ScriptAddPair(script, name)});
  if (!ret.second) {
    delete(script);
    pWarn("Script already exists on object");
    return false;
  }

  return true;
}



Script* Object_GetScript(Object* object, std::string const& name) {
  pNullCheck(object, nullptr);

  pObjDefExt(object, ext);
  
  auto find = ext->scripts.find(_STRING_HASHER(name));
  if (find != ext->scripts.end()) { 
    pWarn("Script does not exist with name on object");
    return nullptr;
  }

  return find->second.script;
}



bool Object_RemoveScript(Object* object, std::string const& name) {
  pNullCheck(object, false);

  pObjDefExt(object, ext);

  auto find = ext->scripts.find(_STRING_HASHER(name));
  if (find == ext->scripts.end()) {
    pWarn("Script does not exist with name on object");
    return false;
  }
  delete(find->second.script);
  ext->scripts.erase(find);

  return true;
}



void Object_AddDeleteCallback(Object* object, ObjectDeleteCallback ptrFunc, int id) {
  pNullCheck(object);
  pObjExt(object)->deleteCallbacks.insert({ptrFunc, id});
}



void Object_RemoveDeleteCallback(Object* object, ObjectDeleteCallback ptrFunc, int id) {
  pNullCheck(object);
  pObjExt(object)->deleteCallbacks.erase({ptrFunc, id});
}

// --------------------------------------------------
// --------------------------------------------------
// Object





// Transform
// --------------------------------------------------
// --------------------------------------------------

void Transform_GenerateModel(Transform transform, MatrixWrapper& store) {
  store = MatrixWrapper();
  glm::highp_mat4* data = (glm::highp_mat4*)&store;
  *data = glm::translate(*data, glm::vec3(transform.position.x, transform.position.y, transform.position.z));
  *data = glm::scale(*data, glm::vec3(transform.scale.x, transform.scale.y, transform.scale.z));
  *data = glm::rotate(*data, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
  *data = glm::rotate(*data, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
  *data = glm::rotate(*data, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
}

// --------------------------------------------------
// --------------------------------------------------
// Transform





// Camera
// --------------------------------------------------
// --------------------------------------------------

Camera* RegisterCamera(std::string const& name) {
  pPumpkinCheck(nullptr);

  if (name.size() == 0) {
    pWarn("Invalid camera name");
    return nullptr;
  }

  auto ret = pumpkinData->registeredCameras.insert({_STRING_HASHER(name), nullptr});
  if (!ret.second) {
    pWarn("Camera already exists with same name");
    return nullptr;
  };
  auto objRet = pumpkinData->registeredObjects.insert({_STRING_HASHER(name), nullptr});
  if (!objRet.second) {
    pWarn("Object already exists with same name");
    return nullptr;
  };

  Camera* cam = new Camera();
  ret.first->second = cam;
  objRet.first->second = cam;

  StartCamera(cam);

  pObjDefInt(ret.first->second, i);

  i->external = new ObjectExternal();

  i->name = new char[name.size() + 1];
  i->name[name.size()] = 0;
  std::memcpy(i->name, name.c_str(), name.size());

  return cam;
}



Camera* GetCamera(std::string const& name) {
  pPumpkinCheck(nullptr);

  auto ret = pumpkinData->registeredCameras.find(_STRING_HASHER(name));
  return ret == pumpkinData->registeredCameras.end() ? nullptr : ret->second;
}



bool SetPrimaryCamera(Camera* camera) {
  pPumpkinCheck(false);
  pNullCheck(camera, false);

  pumpkinData->primaryCamera = camera;
  return true;
}



Camera* GetPrimaryCamera() {
  pPumpkinCheck(nullptr);

  return pumpkinData->primaryCamera;
}



void Camera_GenerateView(Camera* camera) {
  pNullCheck(camera);

  pCamDefInt(camera, i);
  Vector3 lookAt = i->lookAt ? *i->lookAt : camera->transform.position + i->forward;

  camera->view = MatrixWrapper();
  glm::highp_mat4* data = (glm::highp_mat4*)&camera->view;


  // Easier than trying to fix 'rotate'
  *data = glm::lookAt(glm::vec3(camera->transform.position.x, camera->transform.position.y, camera->transform.position.z), glm::vec3(lookAt.x, lookAt.y, lookAt.z), glm::vec3(_UP.x, _UP.y, _UP.z));
/*
  *data = glm::rotate(*data, glm::radians(-camera->transform.rotation.z), glm::vec3(0, 0, 1));
  *data = glm::rotate(*data, glm::radians(-camera->transform.rotation.y), glm::vec3(0, 1, 0));
  *data = glm::rotate(*data, glm::radians(-camera->transform.rotation.x), glm::vec3(1, 0, 0));
  *data = glm::translate(*data, glm::vec3(-camera->transform.position.x, -camera->transform.position.y, -camera->transform.position.z));*/
}



void Camera_GenerateProjection(Camera* camera) {
  pNullCheck(camera);

  camera->proj = MatrixWrapper();
  glm::highp_mat4* data = (glm::highp_mat4*)&camera->proj;

  *data = glm::perspective(camera->fov, camera->aspect, camera->near, camera->far);
}



Vector3* Camera_Forward(Camera* camera) {
  pNullCheck(camera, nullptr);
  return &pCamInt(camera)->forward;
}



Vector3* Camera_Right(Camera* camera) {
  pNullCheck(camera, nullptr);
  return &pCamInt(camera)->right;
}


bool Camera_GetAngleBased(Camera* camera) {
  pNullCheck(camera, false);
  return pCamInt(camera)->angleBased;
}



void Camera_AngleBased(Camera* camera, bool b) {
  pNullCheck(camera);
  pCamInt(camera)->angleBased = b;
}



void Camera_LookAtTarget(Camera* camera, ::pPack::Vector3* target) {
  pNullCheck(camera);
  pCamInt(camera)->lookAt = target;
}



::pPack::Vector3* Camera_GetLookAtTarget(Camera* camera) {
  pNullCheck(camera, nullptr);
  return pCamInt(camera)->lookAt;
}

// --------------------------------------------------
// --------------------------------------------------
// Camera




// Mesh
// --------------------------------------------------
// --------------------------------------------------

void Mesh::Delete() {
  if (dynamic) {
    glDeleteBuffers(1, &VBO);
  }
  free(vertices);

  format = 0;
  VBO = 0;
  vertices = 0;
  vertexCount = 0;
}



void Mesh::Setup() {
  glBindVertexArray(format);
  glBindVertexBuffer(0, GetVBO(), offset, vertexSize);
}



Mesh* RegisterMesh(std::string const& name, void* vertices, size_t size, size_t count, bool dynamic, GLuint format) {
  pPumpkinCheck(nullptr);

  if (name.size() == 0) {
    pWarn("Invalid mesh name");
    return nullptr;
  }

  if (!vertices) {
    pWarn("vertices is nullptr");
    return nullptr;
  }
  if (size == 0) {
    pWarn("size is invalid");
    return nullptr;
  }
  if (count == 0) {
    pWarn("count is invalid");
    return nullptr;
  }

  size_t totalSize = size * count;
  if (totalSize / size != count) { // overflow detection
    pWarn("Data too large");
    return nullptr;
  }
  if (format == 0) {
    pWarn("format is invalid");
    return nullptr;
  }


  auto ret = pumpkinData->registeredMeshes.insert({_STRING_HASHER(name), nullptr});
  if (!ret.second) {
    pWarn("Mesh already exists with same name");
    return nullptr;
  }
  Mesh* mesh = new Mesh();
  ret.first->second = mesh;

  mesh->name = name;

  // Set stuff
  mesh->format = format;
  mesh->vertices = malloc(totalSize);
  if (mesh->vertices == nullptr) {
    pWarn("Failed to allocate vertex buffer");
    delete(mesh);
    return nullptr;
  }
  memcpy(mesh->vertices, vertices, totalSize);
  mesh->vertexCount = count;
  mesh->vertexSize = size;
  mesh->bufferSize = totalSize;
  mesh->dynamic = dynamic;

  // Uses shared vbo if static
  if (!dynamic) {
    mesh->pVBO = &pumpkinData->globalVBO;
    return mesh;
  }

  // Create buffers and add data to buffer
  glCreateBuffers(1, &mesh->VBO);
  glNamedBufferData(mesh->VBO, totalSize, vertices, GL_DYNAMIC_DRAW);

  return mesh;
}



Mesh* GetMesh(std::string const& name) {
  pPumpkinCheck(nullptr);

  auto ret = pumpkinData->registeredMeshes.find(_STRING_HASHER(name));
  return ret == pumpkinData->registeredMeshes.end() ? nullptr : ret->second;
}



GLuint RegisterFormat(std::string const& name, FormatStartInfo const* const formatStartInfo, GLuint count, bool autoOffset) {
  pPumpkinCheck(0);
  pNullCheck(formatStartInfo, 0, 0);
  pCheckIf(count, 0, 0);

  auto ret = pumpkinData->registeredFormats.insert({_STRING_HASHER(name), 0});
  if (!ret.second) {
    pWarn("Format already exists with same name");
    return 0;
  }
  GLuint& vao = ret.first->second;

  // Create format
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glVertexBindingDivisor(0, 0);

  // Auto offset keeping counter
  GLuint rollingOffset = 0;
  for (GLuint i = 0; i < count; i++) {
    glEnableVertexAttribArray(i);
    glVertexAttribBinding(i, 0);

    // Set format based on attribute type
    if (formatStartInfo[i].attribType == AttributeType::FLOAT)
      glVertexAttribFormat
      (
      i,
      formatStartInfo[i].size,
      formatStartInfo[i].type,
      formatStartInfo[i].normalized,
      autoOffset ? rollingOffset : formatStartInfo[i].relativeOffset
      );
    else if (formatStartInfo[i].attribType == AttributeType::INT)
      glVertexAttribIFormat
      (
      i,
      formatStartInfo[i].size,
      formatStartInfo[i].type,
      autoOffset ? rollingOffset : formatStartInfo[i].relativeOffset
      );
    else if (formatStartInfo[i].attribType == AttributeType::LONG)
      glVertexAttribLFormat
      (
      i,
      formatStartInfo[i].size,
      formatStartInfo[i].type,
      autoOffset ? rollingOffset : formatStartInfo[i].relativeOffset
      );

    rollingOffset += formatStartInfo[i].size * sizeof(GLfloat);
  }

  glBindVertexArray(0);
  return vao;
}



GLuint GetFormat(std::string const& name) {
  pPumpkinCheck(0);

  auto ret = pumpkinData->registeredFormats.find(_STRING_HASHER(name));
  return ret == pumpkinData->registeredFormats.end() ? 0 : ret->second;

}



void ApplyStaticBuffer() {
  pPumpkinCheck();

  std::vector<Mesh*> staticMeshes = std::vector<Mesh*>();
  for (auto& p : pumpkinData->registeredMeshes)
    if (!p.second->dynamic) staticMeshes.push_back(p.second);
  if (staticMeshes.size() == 0) return;


  // Get total size of all meshes
  size_t totalSize = 0;
  size_t temp = 0;
  for (Mesh* m : staticMeshes) {
    temp = totalSize + m->bufferSize;
    if (temp < totalSize) { // My own janky overflow detection.
      pWarn("Size overflowed, too large or too many static meshes");
      return;
    }

    totalSize = temp;
  }

  // Delete previous vbo is exists
  if (pumpkinData->globalVBO) glDeleteBuffers(1, &pumpkinData->globalVBO);

  // Create new one with preinitialized size
  glCreateBuffers(1, &pumpkinData->globalVBO);
  glNamedBufferData(pumpkinData->globalVBO, totalSize, nullptr, GL_STATIC_DRAW);

  // Copy all meshes into buffer
  size_t currentOffset = 0;
  for (Mesh* m : staticMeshes) {
    glNamedBufferSubData(pumpkinData->globalVBO, currentOffset, m->bufferSize, m->vertices);
    m->offset = currentOffset;
    currentOffset += m->bufferSize;
  }
}


// --------------------------------------------------
// --------------------------------------------------
// Mesh





// Model
// --------------------------------------------------
// --------------------------------------------------

Model* RegisterModel(std::string const& name) {
  pPumpkinCheck(nullptr);

  if (name.size() == 0) {
    pWarn("Invalid model name");
    return nullptr;
  }

  auto ret = pumpkinData->registeredModels.insert({_STRING_HASHER(name), 0});
  if (!ret.second) {
    pWarn("Model already exists with same name");
    return nullptr;
  };
  Model* model = new Model();

  ret.first->second = model;
  model->name = name;

  return model;
}



Model* GetModel(std::string const& name) {
  pPumpkinCheck(nullptr);

  auto ret = pumpkinData->registeredModels.find(_STRING_HASHER(name));
  return ret == pumpkinData->registeredModels.end() ? nullptr : ret->second;
}


bool Model_SetShader(Model* model, Shader* shader) {
  pNullCheck(model, false);
  pNullCheck(shader, false);

  if (model->shader) {
    model->shader->RemoveModel(model);
    model->shader = nullptr;
  }

  if (shader->AddModel(model))
    model->shader = shader;
  else return false;
  return true;
}


bool Model_SetMesh(Model* model, Mesh* mesh) {
  pNullCheck(model, false);
  pNullCheck(mesh, false);


  model->mesh = mesh;
  return true;
}


PropertyHolder* Model_GetProperties(Model* model) {
  pNullCheck(model, nullptr);

  return &model->properties;
}

// --------------------------------------------------
// --------------------------------------------------
// Model





// Shader
// --------------------------------------------------
// --------------------------------------------------

Shader* RegisterShader(std::string const& name, ShaderInfo* startInfos, int count) {
  pPumpkinCheck(nullptr);
  pNullCheck(startInfos, nullptr);
  if (count <= 0) {
    pWarn("Count must be positive number");
    return nullptr;
  }

  if (name.size() == 0) {
    pWarn("Invalid shader name");
    return nullptr;
  }


  // Attempt insertion
  auto ret = pumpkinData->registeredShaders.insert({_STRING_HASHER(name), nullptr});
  if (!ret.second) {
    pWarn("Shader already exists with same name");
    return nullptr;
  } 

  Shader* tShader = new Shader();
  for (int i = 0; i < count; i++) {
    tShader->startInfos.push_back(startInfos[i]);
  }
  ret.first->second = tShader;
  tShader->name = name;
  tShader->Reload();

  return tShader;
}



Shader* GetShader(std::string const& name) {
  pPumpkinCheck(nullptr);

  auto ret = pumpkinData->registeredShaders.find(_STRING_HASHER(name));
  return ret == pumpkinData->registeredShaders.end() ? nullptr : ret->second;
}



PropertyHolder* Shader_GetProperties(Shader* shader) {
  pNullCheck(shader, nullptr);

  return &shader->properties;
}

// --------------------------------------------------
// --------------------------------------------------
// Shader





// Script
// --------------------------------------------------
// --------------------------------------------------

bool RegisterScriptRaw(ScriptAllocateFunction scriptAllocate, std::string const& name, size_t size) {
  pPumpkinCheck(false);
  pNullCheck(scriptAllocate, false);
  if (size == 0) {
    pWarn("Invalid size");
    return false;
  }

  if (name.size() == 0) {
    pWarn("Invalid script name");
    return false;
  }

  return pumpkinData->registeredScripts.insert({_STRING_HASHER(name), ScriptInfoPair(scriptAllocate, name, size)}).second;
}



Script* CreateScript(std::string const& name) {
  pPumpkinCheck(nullptr);
  
  auto find = pumpkinData->registeredScripts.find(_STRING_HASHER(name));
  if (find == pumpkinData->registeredScripts.end()) {
    pWarn("Script with name not found");
    return nullptr;
  }

  return find->second.allocate();
}



char const* GetScriptName(Script* script) {
  pNullCheck(script, nullptr);
  return typeid(*script).name();
}

// --------------------------------------------------
// --------------------------------------------------
// Script



}; // namespace pumpkin






namespace pumpkin_private {


void DeleteObjects() {
  pPumpkinCheck();

  // Delete all objects
  // Since can be created by dev console

  for (auto& obj : pumpkinData->registeredObjects) {
    ::pumpkin_private::DeleteObject(obj.second);
  }

  pumpkinData->registeredObjects.clear();
  pumpkinData->registeredCameras.clear();
}



bool RegisterWindow(std::string const& name, ::pPack::Window* data) {
  assert(pumpkinData != nullptr);

  if (data == nullptr) return false;
  return pumpkinData->registeredWindows.insert(std::pair<size_t, Window*>(_STRING_HASHER(name), data)).second;
}


::pPack::Window* GetWindow(std::string const& name) {
  assert(pumpkinData != nullptr);

  auto ret = pumpkinData->registeredWindows.find(_STRING_HASHER(name));
  return ret == pumpkinData->registeredWindows.end() ? nullptr : ret->second;
}


Pumpkin* GetPumpkin() {
  return pumpkinData;
}



void ApplyShaderVariable(Property const& var) {
  if (var.prop == nullptr) return;
  switch (var.type) {
    case VariableType::INT:
      ShaderHandler::SetInt(var.name, *((int*)var.prop));
      break;
    case VariableType::FLOAT:
      ShaderHandler::SetFloat(var.name, *((float*)var.prop));
      break;
    case VariableType::MAT4:
      ShaderHandler::SetMat4(var.name, (float*)var.prop);
      break;
    case VariableType::VECTOR2:
      ShaderHandler::SetVector2(var.name, *((Vector2*)var.prop));
      break;
    case VariableType::VECTOR3:
      ShaderHandler::SetVector3(var.name, *((Vector3*)var.prop));
      break;
    case VariableType::VECTOR4:
      ShaderHandler::SetVector4(var.name, *((Vector4*)var.prop));
      break;
  }
}



void DeleteObject(Object* obj) {
  assert(obj != nullptr);
  pObjDefInt(obj, i);
  delete(i->name);
  if (i->model) i->model->RemoveObject(obj);

  auto ext = pObjInt(obj)->external;

  for (auto& script : ext->scripts) {
    script.second.script->End(obj);
    delete(script.second.script);
  }
  delete(ext);
  delete(obj);
}


}; // namespace pumpkin_private