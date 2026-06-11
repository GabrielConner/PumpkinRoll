#include "pumpkin/pumpkinRoll.h"
#include "pumpkin/constants.h"
#include "private/pumpkinRoll.h"
#include "private/fileManager.h"
#include "private/shader.h"
#include "pumpkin/fileManager.h"
#include "private/mesh.h"
#include "private/model.h"
#include "private/types.h"
#include "private/pumpkinRollDev.h"

#include "pPack/windowManager.h"
#include "pPack/shaderHandling.h"

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
  Camera* camera = RegisterCamera("PumpkinRoll__Default");
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

  ShaderCreateInfo shaderCreateInfos[] = {ShaderCreateInfo(&vertLocs, 1, GL_VERTEX_SHADER), ShaderCreateInfo(&fragLocs, 1, GL_FRAGMENT_SHADER)};

  Shader* shader = RegisterShader("PumpkinRoll__Default", ShaderHandler::CreateShader(shaderCreateInfos, sizeof(shaderCreateInfos) / sizeof(ShaderCreateInfo)));
  if (!shader) {
    pError("Failed to create default shader");
    return StartReturn::ERROR;
  }


  // Format
  FormatStartInfo formatCreateInfos[] = {
    FormatStartInfo{},
    FormatStartInfo{.size = 4},
  };

  GLuint format = RegisterFormat("PumpkinRoll__Default", formatCreateInfos, 2, true);
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

  Mesh* mesh = RegisterMesh("PumpkinRoll__Default", planeVerts, 7 * sizeof(float), 6, false, format);
  if (!mesh) {
    pError("Failed to create default mesh");
    return StartReturn::ERROR;
  }


  // Model
  Model* model = RegisterModel("PumpkinRoll_Default");
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


  // Object
  Object* render = RegisterObject("TestObj");
  if (!render) {
    pError("Failed to create TestObj");
    return StartReturn::ERROR;
  }
  Object_SetModel(render, model);
  render->transform.rotation.x = 45;


  ApplyStaticBuffer();


#ifdef PUMPKIN_ROLL_DEV
  StartDevelopment();
#endif

  return StartReturn::SUCCESS;
}





void Update() {
  pPumpkinCheck();

  auto& runtime = pumpkinData->runtime;

  // Loop until the main window is closed
  while (!pumpkinData->primaryWindow->ShouldClose()) {
    glfwPollEvents();

    glClearColor(runtime.backgroundColor.x, runtime.backgroundColor.y, runtime.backgroundColor.z, runtime.backgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

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
  for (auto& cam : pumpkinData->registeredCameras) {
    delete(cam.second);
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
    pError("\n\n\n****MEMORY LEAK****\n");
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





// Object
// --------------------------------------------------
// --------------------------------------------------

Object* RegisterObject(std::string const& name) {
  pPumpkinCheck(nullptr);

  auto ret = pumpkinData->registeredObjects.insert({_STRING_HASHER(name), nullptr});
  if (!ret.second) {
    pWarn("Object already exists with same name");
    return nullptr;
  }
  ret.first->second = new Object();

  pObjDefInt(ret.first->second, i);

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
  *data = glm::rotate(*data, transform.rotation.z, glm::vec3(0, 0, 1));
  *data = glm::rotate(*data, transform.rotation.y, glm::vec3(0, 1, 0));
  *data = glm::rotate(*data, transform.rotation.x, glm::vec3(1, 0, 0));
}

// --------------------------------------------------
// --------------------------------------------------
// Transform





// Camera
// --------------------------------------------------
// --------------------------------------------------


Camera* RegisterCamera(std::string const& name) {
  pPumpkinCheck(nullptr);

  Camera* cam = new Camera();
  if (!pumpkinData->registeredCameras.insert({_STRING_HASHER(name), cam}).second) {
    pWarn("Camera already exists with same name");
    delete(cam);
    return nullptr;
  } return cam;
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
  pNullCheck(camera)

    camera->view = MatrixWrapper();
  glm::highp_mat4* data = (glm::highp_mat4*)&camera->view;

  *data = glm::rotate(*data, -camera->transform.rotation.z, glm::vec3(0, 0, 1));
  *data = glm::rotate(*data, -camera->transform.rotation.y, glm::vec3(0, 1, 0));
  *data = glm::rotate(*data, -camera->transform.rotation.x, glm::vec3(1, 0, 0));
  *data = glm::translate(*data, glm::vec3(-camera->transform.position.x, -camera->transform.position.y, -camera->transform.position.z));
}



void Camera_GenerateProjection(Camera* camera) {
  pNullCheck(camera);

  camera->proj = MatrixWrapper();
  glm::highp_mat4* data = (glm::highp_mat4*)&camera->proj;

  *data = glm::perspective(camera->fov, camera->aspect, camera->near, camera->far);
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
  pPumpkinCheck(nullptr)

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


  Mesh* mesh = new Mesh();
  if (!pumpkinData->registeredMeshes.insert({_STRING_HASHER(name), mesh}).second) {
    pWarn("Mesh already exists with same name");
    delete(mesh);
    return nullptr;
  }

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
    if (temp < totalSize) {
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

  Model* model = new Model();
  model->name = name;
  if (!pumpkinData->registeredModels.insert({_STRING_HASHER(name), model}).second) {
    pWarn("Model already exists with same name");
    delete(model);
    return nullptr;
  } return model;
}



Model* GetModel(std::string const& name) {
  pPumpkinCheck(nullptr);

  auto ret = pumpkinData->registeredModels.find(_STRING_HASHER(name));
  return ret == pumpkinData->registeredModels.end() ? nullptr : ret->second;
}



bool Model_AddVariable(Model* model, std::string const& name, VariableType var) {
  pNullCheck(model, false);

  return model->vars.insert({name, var}).second;
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

// --------------------------------------------------
// --------------------------------------------------
// Model






// Shader
// --------------------------------------------------
// --------------------------------------------------

Shader* RegisterShader(std::string const& name, GLuint shader) {
  pPumpkinCheck(nullptr);
  if (!shader) {
    pWarn("Invalid shader");
    return nullptr;
  }

  // Create shader data
  Shader* tShader = new Shader(shader);

  // Attempt insertion
  if (!pumpkinData->registeredShaders.insert({_STRING_HASHER(name), tShader}).second) {
    pWarn("Shader already exists with same name");
    delete(tShader);
    return nullptr;
  } return tShader;
}



Shader* GetShader(std::string const& name) {
  if (pumpkinData == nullptr) {
    pWarn("PumpkinRoll has not been started");
    return nullptr;
  }

  auto ret = pumpkinData->registeredShaders.find(_STRING_HASHER(name));
  return ret == pumpkinData->registeredShaders.end() ? nullptr : ret->second;
}



bool Shader_AddVariable(Shader* shader, std::string const& name, VariableType var) {
  pNullCheck(shader, false);

  return shader->vars.insert({name, var}).second;
}

// --------------------------------------------------
// --------------------------------------------------
// Shader



}; // namespace pumpkin






namespace pumpkin_private {


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



void ApplyShaderVariable(std::string name, VariableType const& var) {
  if (var.variable == nullptr) return;
  int32_t type = var.type & ~var_type::CONTROL;
  switch (type) {
    case var_type::INT:
      ShaderHandler::SetInt(name, *((int*)var.variable));
      break;
    case var_type::FLOAT:
      ShaderHandler::SetFloat(name, *((float*)var.variable));
      break;
    case var_type::MAT4:
      ShaderHandler::SetMat4(name, (float*)var.variable);
      break;
    case var_type::VECTOR2:
      ShaderHandler::SetVector2(name, *((Vector2*)var.variable));
      break;
    case var_type::VECTOR3:
      ShaderHandler::SetVector3(name, *((Vector3*)var.variable));
      break;
    case var_type::VECTOR4:
      ShaderHandler::SetVector4(name, *((Vector4*)var.variable));
      break;
  }
}



void DeleteObject(Object* obj) {
  assert(obj != nullptr);
  pObjDefInt(obj, i);
  delete(i->name);
  if (i->model) i->model->RemoveObject(obj);
  delete(obj);
}


}; // namespace pumpkin_private