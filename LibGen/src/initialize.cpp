#define GLFW_INCLUDE_NONE
#include "glfw/glfw3.h"
#include "glad/glad.h"

namespace pumpkin_private {
__declspec(dllexport) bool LibGenInitGLFW() {
  return glfwInit();
}


__declspec(dllexport) bool LibGenInitGLAD() {
  return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}


__declspec(dllexport) void EndLibGen() {
  glfwTerminate();
}
}; // namespace pumpkin_private