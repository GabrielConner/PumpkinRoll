#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_TYPES_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_TYPES_H

#include "pPack/fileInterface.h"
#include "pPack/vector.h"
#include "glad/glad.h"

#include <algorithm>
#include <format>
#include <sstream>
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
struct Script;
typedef Script* (*ScriptAllocateFunction)();
typedef void* (*ProcAddressFunction(const char* name));
typedef bool (*PumpkinRollLoadFunction)(ProcAddressFunction func);

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
  MatrixWrapper(::pPack::Vector4 Col0, ::pPack::Vector4 Col1, ::pPack::Vector4 Col2, ::pPack::Vector4 Col3) : col0(Col0), col1(Col1), col2(Col2), col3(Col3) {}
};



struct Transform {
  ::pPack::Vector3 position = 0;
  ::pPack::Vector3 scale = 1;
  ::pPack::Vector3 rotation = 0;

  Transform() = default;
  Transform(::pPack::Vector3 Position, ::pPack::Vector3 Scale, ::pPack::Vector3 Rotation) : position(Position), scale(Scale), rotation(Rotation) {}
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


struct ShaderInfo {
  std::vector<std::string> shaders = std::vector<std::string>();
  unsigned int shaderCount = 0;
  unsigned int type = 0;
};



struct Script;
struct Object {
  Line internal[6] = {0};

  Transform transform = Transform();
  bool developmentObject = false;

  virtual ~Object() {}
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
  virtual void Start(Object* obj) {}
  virtual void Update(Object* obj, double deltaTime, double totalTime) {}
  virtual void End(Object* obj) {}
};



struct FileData {
  void* data = 0;
  size_t size = 0;
  bool shouldDelete = false;

  void Delete() { free(data); data = nullptr; size = 0; }
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




/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                        F O R M A T T E R S                            */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/


template<>
struct std::formatter<::pumpkin::MatrixWrapper, char> {
  bool construct = false;

  template<class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& ctx) {
    auto it = ctx.begin();
    if (it == ctx.end())
      return it;

    if (*it == 'c') {
      construct = true;
      ++it;
    }


    // Required by specifications of BasicFormatter
    if (it != ctx.end() && *it != '}')
      throw std::format_error("Invalid format args for MatrixWrapper.");

    return it;
  }

  template<class FmtContext>
  FmtContext::iterator format(::pumpkin::MatrixWrapper s, FmtContext& ctx) const {
    std::ostringstream out;
    if (construct) {
      out << "::pumpkin::MatrixWrapper(" << "::pPack::Vector4(" << s.cols[0] << ")," << "::pPack::Vector4(" << s.cols[1] << ")," << "::pPack::Vector4(" << s.cols[2] << ")," << "::pPack::Vector4(" << s.cols[3] << "))";
    } else {
      out << "{" << s.cols[0] << ',' << s.cols[1] << ',' << s.cols[2] << ',' << s.cols[3] << "}";
    }

    return std::ranges::copy(std::move(out).str(), ctx.out()).out;
  }
};



template<>
struct std::formatter<::pumpkin::Transform, char> {
  bool construct = false;

  template<class ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& ctx) {
    auto it = ctx.begin();
    if (it == ctx.end())
      return it;

    if (*it == 'c') {
      construct = true;
      ++it;
    }


    // Required by specifications of BasicFormatter
    if (it != ctx.end() && *it != '}')
      throw std::format_error("Invalid format args for Transform.");

    return it;
  }

  template<class FmtContext>
  FmtContext::iterator format(::pumpkin::Transform s, FmtContext& ctx) const {
    std::ostringstream out;
    if (construct) {
      out << "::pumpkin::Transform(" << "::pPack::Vector3(" << s.position << ")," << "::pPack::Vector3(" << s.scale << ")," << "::pPack::Vector3(" << s.rotation << "))";
    } else {
      out << "{" << s.position << ',' << s.scale << ',' << s.rotation << "}";
    }

    return std::ranges::copy(std::move(out).str(), ctx.out()).out;
  }
};


#endif