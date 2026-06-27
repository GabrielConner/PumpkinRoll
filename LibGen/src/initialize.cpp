#include "glfw/glfw3.h"


namespace pumpkin_private {
__declspec(dllexport) bool InitLibGen() {
  return glfwInit();
}

__declspec(dllexport) void EndLibGen() {
  glfwTerminate();
}
}; // namespace pumpkin_private