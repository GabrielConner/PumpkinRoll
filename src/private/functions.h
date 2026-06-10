#ifndef PUMPKIN_ROLL_SRC_PRIVATE_FUNCTIONS_H
#define PUMPKIN_ROLL_SRC_PRIVATE_FUNCTIONS_H

#include "pumpkin/types.h"

namespace pumpkin {


StartReturn Init(StartSettings const& start);
void Update();
void End();

RuntimeSettings* GetRuntime();

void PrintError(PrintLevel level, char const* file, int line, char const* msg);


// Object
// --------------------------------------------------
// --------------------------------------------------

Object* RegisterObject(std::string const& name);
Object* GetObject(std::string const& name);
bool DeleteObject(std::string const& name);

char const* Object_GetName(Object const* object);
bool Object_SetModel(Object* object, Model* model);

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

bool Model_AddVariable(Model* model, std::string const& name, VariableType var);
bool Model_SetShader(Model* model, Shader* shader);
bool Model_SetMesh(Model* model, Mesh* mesh);

// --------------------------------------------------
// --------------------------------------------------
// Model



// Shader
// --------------------------------------------------
// --------------------------------------------------

Shader* RegisterShader(std::string const& name, GLuint shader);
Shader* GetShader(std::string const& name);

bool Shader_AddVariable(Shader* shader, std::string const& name, VariableType var);

// --------------------------------------------------
// --------------------------------------------------
// Shader

}; // namespace pumpkin


#endif