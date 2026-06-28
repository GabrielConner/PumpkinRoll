#ifndef PPM_INCLUDE_PPACK_SHADER_HANDLING_H
#define PPM_INCLUDE_PPACK_SHADER_HANDLING_H

#include <string>

#include "pPack/vector.h"
#include "pPack/fileInterface.h"

#ifdef PUMPKIN_ROLL_PROD
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif


namespace pPack {
struct ShaderCreateInfo {
  const char** shaders;
  unsigned int shaderCount;
  unsigned int type;

  ShaderCreateInfo() : shaders(0), shaderCount(0), type(0) {}
  ShaderCreateInfo(const char** s, unsigned int sC, unsigned int t) : shaders(s), shaderCount(sC), type(t) {}
};

class ShaderHandler {
  unsigned int internal;

  static unsigned int currentShader;

public:


  API void Active();

  operator int() {
    return internal;
  }

  ShaderHandler() {
    internal = 0;
  }
  ShaderHandler(unsigned int s) {
    internal = s;
  }



  static unsigned int GetCurrentShader() {
    return currentShader;
  }
  API static void SetCurrentShader(unsigned int s);

  API static unsigned int CreateShader(ShaderCreateInfo* shaderInfo, size_t shaderCount, OpenReadFileFunc open, ReadFileFunc read, CloseFileFunc close);

  API static void SetBool(std::string name, bool value);
  API static void SetUInt(std::string name, unsigned int value);
  API static void SetInt(std::string name, int value);
  API static void SetFloat(std::string name, float value);
  API static void SetVector2(std::string name, float x, float y);
  API static void SetVector2(std::string name, Vector2 vec);
  API static void SetVector3(std::string name, float x, float y, float z);
  API static void SetVector3(std::string name, Vector3 vec);
  API static void SetVector4(std::string name, float x, float y, float z, float w);
  API static void SetVector4(std::string name, Vector4 vec);
  API static void SetMat4(std::string name, const float* Mat);


  API static void SetBool(const char* name, bool value);
  API static void SetUInt(const char* name, unsigned int value);
  API static void SetInt(const char* name, int value);
  API static void SetFloat(const char* name, float value);
  API static void SetVector2(const char* name, float x, float y);
  API static void SetVector2(const char* name, Vector2 vec);
  API static void SetVector3(const char* name, float x, float y, float z);
  API static void SetVector3(const char* name, Vector3 vec);
  API static void SetVector4(const char* name, float x, float y, float z, float w);
  API static void SetVector4(const char* name, Vector4 vec);
  API static void SetMat4(const char* name, const float* Mat);
};
} //namespace pPack

#endif