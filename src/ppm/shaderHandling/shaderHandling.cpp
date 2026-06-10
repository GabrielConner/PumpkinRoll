/*

  This file has been modified specially for pumpkin roll

*/

#include "pumpkin/fileManager.h"
#include "pumpkin/pumpkinRoll.h"



#include "pPack/shaderHandling.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>

#include "pPack/vector.h"
#include "glad/glad.h"


using namespace ::pPack;
using namespace ::pumpkin;


unsigned int ShaderHandler::currentShader = 0;


namespace pPack {

bool ShaderHandler::FileExists(std::string file) {
  struct stat buffer;
  if (stat(file.c_str(), &buffer))
    return false;
  return true;
}

void ShaderHandler::Active() {
  glUseProgram(internal);
  currentShader = internal;
}


void ShaderHandler::SetCurrentShader(GLuint s) {
  glUseProgram(s);
  currentShader = s;
}


GLuint ShaderHandler::CreateShader(ShaderCreateInfo* shaderInfo, size_t shaderCount) {
  if (!shaderCount || !shaderInfo)
    return 0;
  for (size_t i = 0; i < shaderCount; i++)
    if (!shaderInfo[i].shaderCount || !shaderInfo[i].shaders)
      return 0;


  GLuint shaderProgram = glCreateProgram();
  GLuint* shaders = new GLuint[shaderCount];
  for (size_t i = 0; i < shaderCount; i++)
    shaders[i] = 0;

  GLchar err[255];
  int success = 0;

  std::string fString;
  FileData data;

  for (size_t shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++) {
    GLchar** shaderStr = new GLchar*[shaderInfo[shaderIndex].shaderCount];

    for (size_t shaderPieceIndex = 0; shaderPieceIndex < shaderInfo[shaderIndex].shaderCount; shaderPieceIndex++) {
      fString = std::string(shaderInfo[shaderIndex].shaders[shaderPieceIndex]);

#ifdef PUMPKIN_ROLL_DEV
      success = LoadFile(fString, true, true, false, false);
#else
      success = LoadFile(fString, true, true, true, false);
#endif
      if (success == 1) {
        glDeleteProgram(shaderProgram);
        pWarn("Shader file couldn't be loaded");
        return 0;
      }
      data = ReadFile(fString, false);
      if (data.data == nullptr) {
        pWarn("Shader couldn't be read");
        return 0;
      }

      shaderStr[shaderPieceIndex] = new char[data.size + 1];
      shaderStr[shaderPieceIndex][data.size] = 0;
      std::memcpy(shaderStr[shaderPieceIndex], (const char*)data.data, data.size);

      if (data.shouldDelete) data.Delete();
    }

    shaders[shaderIndex] = glCreateShader(shaderInfo[shaderIndex].type);
    glShaderSource(shaders[shaderIndex], shaderInfo[shaderIndex].shaderCount, shaderStr, 0);
    glCompileShader(shaders[shaderIndex]);
    glGetShaderiv(shaders[shaderIndex], GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shaders[shaderIndex], 255, 0, err);
      std::cout << err << '\n';
    }

    glAttachShader(shaderProgram, shaders[shaderIndex]);

    for (size_t j = 0; j < shaderInfo[shaderIndex].shaderCount; j++)
      delete[](shaderStr[j]);
    delete[](shaderStr);
  }

  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 255, 0, err);
    std::cout << err << '\n';
  }

  for (size_t i = 0; i < shaderCount; i++)
    glDeleteShader(shaders[i]);
  delete[](shaders);

  return shaderProgram;
}


void ShaderHandler::SetBool(std::string name, bool value) {
  glUniform1i(glGetUniformLocation(currentShader, name.c_str()), value);
}
void ShaderHandler::SetUInt(std::string name, unsigned int value) {
  glUniform1ui(glGetUniformLocation(currentShader, name.c_str()), value);
}void ShaderHandler::SetInt(std::string name, int value) {
  glUniform1i(glGetUniformLocation(currentShader, name.c_str()), value);
}
void ShaderHandler::SetFloat(std::string name, float value) {
  glUniform1f(glGetUniformLocation(currentShader, name.c_str()), value);
}
void ShaderHandler::SetVector2(std::string name, float x, float y) {
  glUniform2f(glGetUniformLocation(currentShader, name.c_str()), x, y);
}
void ShaderHandler::SetVector2(std::string name, Vector2 vec) {
  glUniform2f(glGetUniformLocation(currentShader, name.c_str()), vec.x, vec.y);
}
void ShaderHandler::SetVector3(std::string name, float x, float y, float z) {
  glUniform3f(glGetUniformLocation(currentShader, name.c_str()), x, y, z);
}
void ShaderHandler::SetVector3(std::string name, Vector3 vec) {
  glUniform3f(glGetUniformLocation(currentShader, name.c_str()), vec.x, vec.y, vec.z);
}
void ShaderHandler::SetVector4(std::string name, float x, float y, float z, float w) {
  glUniform4f(glGetUniformLocation(currentShader, name.c_str()), x, y, z, w);
}
void ShaderHandler::SetVector4(std::string name, Vector4 vec) {
  glUniform4f(glGetUniformLocation(currentShader, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}
void ShaderHandler::SetMat4(std::string name, const float* Mat) {
  glUniformMatrix4fv(glGetUniformLocation(currentShader, name.c_str()), 1, false, Mat);
}


void ShaderHandler::SetBool(const char* name, bool value) {
  glUniform1i(glGetUniformLocation(currentShader, name), value);
}
void ShaderHandler::SetUInt(const char* name, unsigned int value) {
  glUniform1ui(glGetUniformLocation(currentShader, name), value);
}
void ShaderHandler::SetInt(const char* name, int value) {
  glUniform1i(glGetUniformLocation(currentShader, name), value);
}
void ShaderHandler::SetFloat(const char* name, float value) {
  glUniform1f(glGetUniformLocation(currentShader, name), value);
}
void ShaderHandler::SetVector2(const char* name, float x, float y) {
  glUniform2f(glGetUniformLocation(currentShader, name), x, y);
}
void ShaderHandler::SetVector2(const char* name, Vector2 vec) {
  glUniform2f(glGetUniformLocation(currentShader, name), vec.x, vec.y);
}
void ShaderHandler::SetVector3(const char* name, float x, float y, float z) {
  glUniform3f(glGetUniformLocation(currentShader, name), x, y, z);
}
void ShaderHandler::SetVector3(const char* name, Vector3 vec) {
  glUniform3f(glGetUniformLocation(currentShader, name), vec.x, vec.y, vec.z);
}
void ShaderHandler::SetVector4(const char* name, float x, float y, float z, float w) {
  glUniform4f(glGetUniformLocation(currentShader, name), x, y, z, w);
}
void ShaderHandler::SetVector4(const char* name, Vector4 vec) {
  glUniform4f(glGetUniformLocation(currentShader, name), vec.x, vec.y, vec.z, vec.w);
}
void ShaderHandler::SetMat4(const char* name, const float* Mat) {
  glUniformMatrix4fv(glGetUniformLocation(currentShader, name), 1, false, Mat);
}


} //namespace pPack