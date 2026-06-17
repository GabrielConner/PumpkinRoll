#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_TYPES_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_TYPES_H

#include "pPack/vector.h"
#include "glad/glad.h"

#include <unordered_map>
#include <set>

namespace pumpkin {


/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                      E N U M E R A T I O N S                          */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/



enum struct PrintLevel { DEBUG = 0, WARNING, ERROR, NOPRINT };
enum struct StartReturn { FAILURE, ERROR, SUCCESS };
enum struct AttributeType { FLOAT, INT, LONG };
enum struct VariableType { UNKNOWN = -1, INT, FLOAT, MAT4, VECTOR2, VECTOR3, VECTOR4 };





/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                        B A S I C   T Y P E S                          */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/



struct alignas(sizeof(uint8_t) * 4) Line { uint8_t data[4]; };

struct Object;
typedef void (*ObjectDeleteCallback)(Object*, int id);

struct Vertex {
  ::pPack::Vector3 position;
  ::pPack::Vector3 normal;
  ::pPack::Vector2 uv;
  ::pPack::Vector4 color;
  ::pPack::Vector3 tangent;
};



struct MatrixWrapper {
  union {
    ::pPack::Vector4 cols[4];
    struct {
      ::pPack::Vector4 col0;
      ::pPack::Vector4 col1;
      ::pPack::Vector4 col2;
      ::pPack::Vector4 col3;
    };
  };

  MatrixWrapper() : col0(1,0,0,0), col1(0,1,0,0), col2(0,0,1,0), col3(0,0,0,1) {}
};



struct Transform {
  ::pPack::Vector3 position = 0;
  ::pPack::Vector3 scale = 1;
  ::pPack::Vector3 rotation = 0;
};



struct StartSettings {
  char title[256] = "Pumpkin Roll";
  int width = 1280, height = 720;
  bool resizable = true;
};



struct RuntimeSettings {
  ::pPack::Vector4 backgroundColor = {0.2f,0.3f,0.3f,1.0f};
  PrintLevel printLevel = PrintLevel::WARNING;
};








/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                     O B J E C T   C L A S S E S                       */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/


struct Script;
struct Object {
  Line internal[4] = {0};

  std::unordered_map<size_t, ::pumpkin::Script*> scripts = std::unordered_map<size_t, ::pumpkin::Script*>();
  std::set<std::pair<::pumpkin::ObjectDeleteCallback, int>> deleteCallbacks = std::set<std::pair<::pumpkin::ObjectDeleteCallback, int>>();

  Transform transform = Transform();
};



struct Camera : public Object {
  Line camInternal[10] = {0};

  MatrixWrapper view;
  MatrixWrapper proj;

  union {
    float fov = 90.f;
    float width;
  };
  union {
    float aspect = 1.7777f;
    float height;
  };
  float near = 0.01f;
  float far = 100.f;

  bool perspective = true;
};



struct FormatStartInfo {
  GLint size = 3;
  GLenum type = GL_FLOAT;
  GLboolean normalized = GL_FALSE;
  GLuint relativeOffset = 0;
  AttributeType attribType = AttributeType::FLOAT;
};



struct Script {
#ifdef PUMPKIN_ROLL_DEV
  virtual void Prebuild(Object* obj) {}
#endif

  virtual void Start(Object* obj) {}
  virtual void Update(Object* obj, double deltaTime) {}
  virtual void End(Object* obj) {}


  virtual Script* Allocate() = 0;
};




/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                T Y P E   F O R W A R D   D E C L A R E S              */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/

struct Mesh;
struct Model;
struct Shader;
struct PropertyHolder;


}; // namespace pumpkin

#endif