/*

  This file has been modified specially for pumpkin roll

*/


#ifndef PPM_INCLUDE_PPACK_SHADER_HANDLING_H
#define PPM_INCLUDE_PPACK_SHADER_HANDLING_H

#include <string>

#include "glad/glad.h"

#include "pPack/vector.h"


namespace pPack {
struct ShaderCreateInfo {
  const char** shaders;
  GLuint shaderCount;
  GLuint type;

  ShaderCreateInfo(const char** s, unsigned int sC, unsigned int t) : shaders(s), shaderCount(sC), type(t) {}
};

class ShaderHandler {
  GLuint internal;

  static GLuint currentShader;

public:


  void Active();

  operator int() {
    return internal;
  }

  ShaderHandler() {
    internal = 0;
  }
  ShaderHandler(unsigned int s) {
    internal = s;
  }



  static bool FileExists(std::string file);

  static GLuint GetCurrentShader() {
    return currentShader;
  }
  static void SetCurrentShader(GLuint s);

  static GLuint CreateShader(ShaderCreateInfo* shaderInfo, size_t shaderCount);

  static void SetBool(std::string name, bool value);
  static void SetUInt(std::string name, unsigned int value);
  static void SetInt(std::string name, int value);
  static void SetFloat(std::string name, float value);
  static void SetVector2(std::string name, float x, float y);
  static void SetVector2(std::string name, Vector2 vec);
  static void SetVector3(std::string name, float x, float y, float z);
  static void SetVector3(std::string name, Vector3 vec);
  static void SetVector4(std::string name, float x, float y, float z, float w);
  static void SetVector4(std::string name, Vector4 vec);
  static void SetMat4(std::string name, const float* Mat);


  static void SetBool(const char* name, bool value);
  static void SetUInt(const char* name, unsigned int value);
  static void SetInt(const char* name, int value);
  static void SetFloat(const char* name, float value);
  static void SetVector2(const char* name, float x, float y);
  static void SetVector2(const char* name, Vector2 vec);
  static void SetVector3(const char* name, float x, float y, float z);
  static void SetVector3(const char* name, Vector3 vec);
  static void SetVector4(const char* name, float x, float y, float z, float w);
  static void SetVector4(const char* name, Vector4 vec);
  static void SetMat4(const char* name, const float* Mat);
};
} //namespace pPack

#endif