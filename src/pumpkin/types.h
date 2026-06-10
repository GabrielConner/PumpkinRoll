#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_TYPES_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_TYPES_H

#include "pPack/vector.h"
#include "glad/glad.h"

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





/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                        B A S I C   T Y P E S                          */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/



struct alignas(sizeof(uint8_t) * 4) Line { uint8_t data[4]; };



struct Vertex {
  ::pPack::Vector3 position;
  ::pPack::Vector3 normal;
  ::pPack::Vector2 uv;
  ::pPack::Vector4 color;
  ::pPack::Vector3 tangent;
};



struct VariableType {
  void* variable = 0;
  uint32_t type = 1;
};



struct MatrixWrapper {
  float data[16] =
  {
  1,0,0,0,
  0,1,0,0,
  0,0,1,0,
  0,0,0,1
  };
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



struct Object {
  Line internal[4] = {0};
  Transform transform = Transform();
};



struct Camera : public Object {
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


}; // namespace pumpkin

#endif