#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_FUNCTIONS_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_FUNCTIONS_H

#include "pumpkin/types.h"

namespace pumpkin {
extern "C" {

#ifdef PUMPKIN_ROLL_PROD
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

API StartReturn Pumpkin_Init(StartSettings const& start, int argv, char** argc, void (*devLoad)());
API void Pumpkin_Update();
API void Pumpkin_End();

API RuntimeSettings* Pumpkin_GetRuntime();

API std::string Pumpkin_ExecutableLocation();

API void Pumpkin_PrintError(PrintLevel level, char const* file, char const* msg);

API double Pumpkin_DeltaTime();
API double Pumpkin_TotalTime();


// Object
// --------------------------------------------------
// --------------------------------------------------

API Object* Pumpkin_RegisterObject(std::string const& name);
API Object* Pumpkin_GetObject(std::string const& name);
API bool Pumpkin_DeleteObject(std::string const& name);

API char const* Object_GetName(Object const* object);
API bool Object_SetModel(Object* object, Model* model);
API bool Object_AddScript(Object* object, std::string const& name);
API Script* Object_GetScript(Object* object, std::string const& name);
API bool Object_RemoveScript(Object* object, std::string const& name);

API void Object_AddDeleteCallback(Object* object, ObjectDeleteCallback ptrFunc, int id);
API void Object_RemoveDeleteCallback(Object* object, ObjectDeleteCallback ptrFunc, int id);

// --------------------------------------------------
// --------------------------------------------------
// Object



// Transform
// --------------------------------------------------
// --------------------------------------------------

API void Transform_GenerateModel(Transform transform, MatrixWrapper& store);

// --------------------------------------------------
// --------------------------------------------------
// Transform



// Camera
// --------------------------------------------------
// --------------------------------------------------

API Camera* Pumpkin_RegisterCamera(std::string const& name);
API Camera* Pumpkin_GetCamera(std::string const& name);

API bool Pumpkin_SetPrimaryCamera(Camera* camera);
API Camera* Pumpkin_GetPrimaryCamera();

API void Camera_GenerateView(Camera* camera);
API void Camera_GenerateProjection(Camera* camera);

API ::pPack::Vector3* Camera_Forward(Camera* camera);
API ::pPack::Vector3* Camera_Right(Camera* camera);

API bool Camera_GetAngleBased(Camera* camera);
API void Camera_AngleBased(Camera* camera, bool b);

API void Camera_LookAtTarget(Camera* camera, ::pPack::Vector3* target);
API ::pPack::Vector3* Camera_GetLookAtTarget(Camera* camera);

// --------------------------------------------------
// --------------------------------------------------
// Camera




// LoadedModelMesh
// --------------------------------------------------
// --------------------------------------------------

API Mesh* Pumpkin_RegisterMesh(std::string const& name, void* vertices, size_t size, size_t count, bool dynamic, GLuint format);
API Mesh* Pumpkin_GetMesh(std::string const& name);

API GLuint Pumpkin_RegisterFormat(std::string const& name, FormatStartInfo const* const formatStartInfo, GLuint count, bool autoOffset);
API GLuint Pumpkin_GetFormat(std::string const& name);

API void Pumpkin_ApplyStaticBuffer();

// --------------------------------------------------
// --------------------------------------------------
// LoadedModelMesh



// Model
// --------------------------------------------------
// --------------------------------------------------

API Model* Pumpkin_RegisterModel(std::string const& name);
API Model* Pumpkin_GetModel(std::string const& name);

API bool Model_SetShader(Model* model, Shader* shader);
API bool Model_SetMesh(Model* model, Mesh* mesh);

API PropertyHolder* Model_GetProperties(Model* shader);

// --------------------------------------------------
// --------------------------------------------------
// Model



// Shader
// --------------------------------------------------
// --------------------------------------------------

API Shader* Pumpkin_RegisterShader(std::string const& name, ShaderInfo* startInfos, int count);
API Shader* Pumpkin_GetShader(std::string const& name);

API PropertyHolder* Shader_GetProperties(Shader* shader);

// --------------------------------------------------
// --------------------------------------------------
// Shader



// PropertyHolder
// --------------------------------------------------
// --------------------------------------------------

API bool PropertyHolder_AddProperty(PropertyHolder* holder, std::string const& name, void* value, VariableType type);
API bool PropertyHolder_SetProperty(PropertyHolder* holder, std::string const& name, void* value);
API bool PropertyHolder_SetOrAddProperty(PropertyHolder* holder, std::string const& name, void* value, VariableType type);
API void PropertyHolder_DeleteProperty(PropertyHolder* holder, std::string const& name);

API void* PropertyHolder_GetProperty(PropertyHolder* holder, std::string const& name);

// --------------------------------------------------
// --------------------------------------------------
// PropertyHolder



// Script
// --------------------------------------------------
// --------------------------------------------------

API bool Pumpkin_RegisterScriptRaw(ScriptAllocateFunction scriptAllocate, std::string const& name, size_t size);
API Script* Pumpkin_CreateScript(std::string const& name);
API char const* Pumpkin_GetScriptName(Script* script);

// --------------------------------------------------
// --------------------------------------------------
// Script




// FileManager
// --------------------------------------------------
// --------------------------------------------------

API int Pumpkin_LoadFile(std::string filePath, bool pack, bool cache, bool relative, bool binary);
API void Pumpkin_ForgetFile(std::string const& path);
API FileData Pumpkin_ReadFile(std::string const& path, bool binary);

API std::string Pumpkin_ToRelativePath(std::string const& path);

API bool Pumpkin_OpenFileFunc(std::string const& location, bool relative, bool binary, ::pPack::FileHandle& handle);
API void Pumpkin_CloseFileFunc(::pPack::FileHandle& handle);

// --------------------------------------------------
// --------------------------------------------------
// FileManager

}
}; // namespace pumpkin


#endif