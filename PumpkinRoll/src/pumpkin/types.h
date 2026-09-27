#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_TYPES_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_TYPES_H

#include "pPack/fileInterface.h"
#include "pPack/vector.h"
#include "pPack/windowManager.h"
#include "glad/glad.h"

#include <algorithm>
#include <format>
#include <sstream>
#include <unordered_map>
#include <set>
#include <forward_list>

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
enum struct AttributeName { UNKNOWN = 0, POSITION, SCALE, ROTATION, UV, NORMAL, TANGENT, BITANGENT };




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


struct ShaderInfo {
  std::vector<std::string> shaders = std::vector<std::string>();
  unsigned int type = 0;
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



struct Vertex {
  ::pPack::Vector3 position;
  ::pPack::Vector2 uv;
  ::pPack::Vector3 normal;
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



struct ScriptUpdateInfo {
  double deltaTime = 0;
  double totalTime = 0;
  ::pPack::Window* window = nullptr;
};



struct FormatStartInfo {
  GLint size = 3;
  GLenum type = GL_FLOAT;
  GLboolean normalized = GL_FALSE;
  GLuint relativeOffset = 0;
  AttributeType attributeType = AttributeType::FLOAT;
  AttributeName attributeName = AttributeName::UNKNOWN;
};



struct Format {
  FormatStartInfo* info = 0;
  size_t infoCount = 0;
  size_t vertexStride = 0;
  GLuint id = 0;
};



struct MeshInfo {
  void* vertices = nullptr;
  size_t vertexCount = 0;
  size_t vertexSize = 0;
  size_t bufferSize = 0;
  Format format;
  bool dynamic = false;
};



struct RayHitInfo {
  Object* object; // If null access to other variables is UB
  ::pPack::DVector3 position; // Always defined
  ::pPack::DVector3 normal; // Always defined
  double time; // Used internally
  bool frontFace = true;
};



/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                           C L A S S E S                               */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/


struct Script;
struct Object {
  Line internal[6] = {0};

  Transform transform = Transform();

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



struct ScriptPropertySaveData {
  size_t size = 0;
  void* data = 0;


  void CopyTo(void* dst, size_t expSize) const {
    if (data == nullptr || size == 0 || expSize != size) return;
    memcpy(dst, data, size);
  }


  void Set(void* otherData, size_t nSize) {
    DeleteIfCreated();
    data = otherData;
    size = nSize;
  }


  bool Create(size_t nSize) {
    DeleteIfCreated();
    data = malloc(nSize);
    size = nSize;
    selfCreated = !!data;
    return data;
  }


  void DeleteIfCreated() {
    if (selfCreated) {
      free(data);
      selfCreated = false;
      data = nullptr;
    }
  }

  ScriptPropertySaveData() = default;
  ScriptPropertySaveData(void* Data, size_t Size) : data(Data), size(Size) {}
private:
  bool selfCreated = false;
};



struct Script {
  virtual void Start(Object* obj) {}
  virtual void Update(Object* obj, ScriptUpdateInfo const& info) {}
  virtual void End(Object* obj) {}


  virtual void DevelopmentUpdate(Object* obj, ScriptUpdateInfo const& info) {}

  virtual std::vector<ScriptPropertySaveData> SaveProperties() { return std::vector<ScriptPropertySaveData>(); }
  virtual void LoadProperties(std::vector<ScriptPropertySaveData> const& savedProperties) {}

  virtual void ShaderUpdate(Object* obj) {}
};



struct FileData {
  void* data = 0;
  size_t size = 0;
  bool shouldDelete = false;

  void Delete() { free(data); data = nullptr; size = 0; }
};



struct Ray {
  ::pPack::DVector3 origin = 0;
  ::pPack::DVector3 direction = 0;
};



struct Interval {
  double min = 0;
  double max = 0;

  Interval() = default;
  constexpr Interval(double Min, double Max) : min(Min), max(Max) {}
};



struct AABB {
  Interval x, y, z;

  AABB() = default;
  constexpr AABB(Interval X, Interval Y, Interval Z) : x(X), y(Y), z(Z) {}
};



struct CollisionObject {
  Object* object = 0;

  virtual bool Collide(Ray const& ray, Interval interval, RayHitInfo& hit) const { return false; }
  virtual AABB GenerateAABB() const { return AABB(); }

  virtual void DeleteInternal() {}
};



struct CollisionTemplatePlane : CollisionObject {
  ::pPack::DVector3 origin = 0;
  ::pPack::DVector3 u = 0, v = 0, n = 0, w = 0;
  double D = 0;

  AABB GenerateAABB() const override;

  CollisionTemplatePlane() = default;
};



struct CollisionTriangle : ::pumpkin::CollisionTemplatePlane {
  bool Collide(::pumpkin::Ray const& ray, ::pumpkin::Interval interval, ::pumpkin::RayHitInfo& hit) const override;
};



struct CollisionQuad : ::pumpkin::CollisionTemplatePlane {
  bool Collide(::pumpkin::Ray const& ray, ::pumpkin::Interval interval, ::pumpkin::RayHitInfo& hit) const override;
};



struct CollisionSphere : ::pumpkin::CollisionObject {
  ::pPack::DVector3 center;
  double radius;

  AABB GenerateAABB() const override;
  bool Collide(::pumpkin::Ray const& ray, ::pumpkin::Interval interval, ::pumpkin::RayHitInfo& hit) const override;
};



struct CollisionList : ::pumpkin::CollisionObject {
  std::vector<CollisionObject*> list = std::vector<CollisionObject*>();
  AABB bbox;

  void Add(CollisionObject* obj);
  void RegenerateAABB();

  bool Collide(Ray const& ray, Interval interval, RayHitInfo& hit) const override;
  AABB GenerateAABB() const override;
  void DeleteInternal() override;
};



struct ExplodedObjectList {
  std::vector<CollisionObject*> list = std::vector<CollisionObject*>();

  std::vector<CollisionObject*>::iterator begin() { return list.begin(); }
  std::vector<CollisionObject*>::iterator end() { return list.end(); }

  std::vector<CollisionObject*>::const_iterator begin() const { return list.begin(); }
  std::vector<CollisionObject*>::const_iterator end() const { return list.end(); }

  std::vector<CollisionObject*>::const_iterator cbegin() const { return list.cbegin(); }
  std::vector<CollisionObject*>::const_iterator cend() const { return list.cend(); }

  void push_back(CollisionObject *const& value) { list.push_back(value); }

  template<typename R>
  constexpr void append_range(R&& rg) { list.append_range(rg); }

  ExplodedObjectList() = default;
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




/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                           S C R I P T S                               */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/



struct PumpkinRoll__CameraFreeMovement : Script {
private:
  ::pPack::Vector3 cameraRotateOffset;
  ::pPack::Vector2 cameraRealMouseZero;
  bool movingAround = false;

public:
  int toggleKey = 1;
  float moveSpeed = 3.0f;
  float cameraSpeed = 1.0f / 7.0f;

  void Update(Object* obj, ScriptUpdateInfo const& info) override;
};


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