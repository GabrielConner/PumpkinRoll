#ifndef PUMPKIN_ROLL_SRC_PRIVATE_FUNCTIONS_H
#define PUMPKIN_ROLL_SRC_PRIVATE_FUNCTIONS_H

#include "pumpkin/types.h"

namespace pumpkin {


StartReturn Init(StartSettings const& start);
void Update();
void End();

RuntimeSettings* GetRuntime();

void PrintError(PrintLevel level, char const* file, char const* msg);

double DeltaTime();


// Object
// --------------------------------------------------
// --------------------------------------------------

Object* RegisterObject(std::string const& name);
Object* GetObject(std::string const& name);
bool DeleteObject(std::string const& name);

char const* Object_GetName(Object const* object);
bool Object_SetModel(Object* object, Model* model);
bool Object_AddScript(Object* object, std::string const& name);
Script* Object_GetScript(Object* object, std::string const& name);
bool Object_RemoveScript(Object* object, std::string const& name);

void Object_AddDeleteCallback(Object* object, ObjectDeleteCallback ptrFunc, int id);
void Object_RemoveDeleteCallback(Object* object, ObjectDeleteCallback ptrFunc, int id);

// --------------------------------------------------
// --------------------------------------------------
// Object



// Transform
// --------------------------------------------------
// --------------------------------------------------

void Transform_GenerateModel(Transform transform, MatrixWrapper& store);

// --------------------------------------------------
// --------------------------------------------------
// Transform



// Camera
// --------------------------------------------------
// --------------------------------------------------

Camera* RegisterCamera(std::string const& name);
Camera* GetCamera(std::string const& name);

bool SetPrimaryCamera(Camera* camera);
Camera* GetPrimaryCamera();

void Camera_GenerateView(Camera* camera);
void Camera_GenerateProjection(Camera* camera);

::pPack::Vector3* Camera_Forward(Camera* camera);
::pPack::Vector3* Camera_Right(Camera* camera);

bool Camera_GetAngleBased(Camera* camera);
void Camera_AngleBased(Camera* camera, bool b);

void Camera_LookAtTarget(Camera* camera, ::pPack::Vector3* target);
::pPack::Vector3* Camera_GetLookAtTarget(Camera* camera);

// --------------------------------------------------
// --------------------------------------------------
// Camera




// Mesh
// --------------------------------------------------
// --------------------------------------------------

Mesh* RegisterMesh(std::string const& name, void* vertices, size_t size, size_t count, bool dynamic, GLuint format);
Mesh* GetMesh(std::string const& name);

GLuint RegisterFormat(std::string const& name, FormatStartInfo const* const formatStartInfo, GLuint count, bool autoOffset);
GLuint GetFormat(std::string const& name);

void ApplyStaticBuffer();

// --------------------------------------------------
// --------------------------------------------------
// Mesh



// Model
// --------------------------------------------------
// --------------------------------------------------

Model* RegisterModel(std::string const& name);
Model* GetModel(std::string const& name);

bool Model_SetShader(Model* model, Shader* shader);
bool Model_SetMesh(Model* model, Mesh* mesh);

PropertyHolder* Model_GetProperties(Model* shader);

// --------------------------------------------------
// --------------------------------------------------
// Model



// Shader
// --------------------------------------------------
// --------------------------------------------------

Shader* RegisterShader(std::string const& name, ShaderInfo* startInfos, int count);
Shader* GetShader(std::string const& name);

PropertyHolder* Shader_GetProperties(Shader* shader);

// --------------------------------------------------
// --------------------------------------------------
// Shader



// PropertyHolder
// --------------------------------------------------
// --------------------------------------------------

bool PropertyHolder_AddProperty(PropertyHolder* holder, std::string const& name, void* value, VariableType type);
bool PropertyHolder_SetProperty(PropertyHolder* holder, std::string const& name, void* value);
bool PropertyHolder_SetOrAddProperty(PropertyHolder* holder, std::string const& name, void* value, VariableType type);

void* PropertyHolder_GetProperty(PropertyHolder* holder, std::string const& name);

// --------------------------------------------------
// --------------------------------------------------
// PropertyHolder



// Script
// --------------------------------------------------
// --------------------------------------------------

bool RegisterScriptRaw(ScriptAllocateFunction scriptAllocate, std::string const& name, size_t size);
Script* CreateScript(std::string const& name);
char const* GetScriptName(Script* script);

#define RegisterScript(scriptAllocate, script) RegisterScriptRaw(scriptAllocate, typeid(script).name(), sizeof(script))
#define ScriptAllocateFunction(script) Script* Allocate ## script () { return new script ## (); }

#define ScriptName(script) typeid(script).name()

// --------------------------------------------------
// --------------------------------------------------
// Script

}; // namespace pumpkin


#endif