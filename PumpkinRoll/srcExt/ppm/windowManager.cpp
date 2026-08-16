#include "pPack/windowManager.h"
#include <map>

#ifdef _PPM_VECTOR

#include "pPack/vector.h"

#endif


#ifdef _PPM_GLAD

#include "glad/glad.h"

#endif

using namespace ::pPack;

namespace {

std::map<GLFWwindow*, Window*> windowToWindowMap = std::map<GLFWwindow*, Window*>();

Window* currentContext = nullptr;

}; // namespace




namespace pPack {

void _windowResizedCallback(GLFWwindow* glWindow, int Width, int Height) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];

  primaryWindow->width = Width;
  primaryWindow->height = Height;
  primaryWindow->aspectRatio = (double)Width / Height;
  primaryWindow->valid = false;
}


void _mouseScrollCallback(GLFWwindow* glWindow, double xoffset, double yoffset) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];
  primaryWindow->mouseScrollX = xoffset;
  primaryWindow->mouseScrollY = yoffset;
}


void _charCallback(GLFWwindow* glWindow, unsigned int codepoint) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];
  if (primaryWindow->readFunction != nullptr) {
    primaryWindow->readFunction(primaryWindow, codepoint);
  }
}


void _keyCallback(GLFWwindow* glWindow, int Key, int Scancode, int Action, int Mods) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];

  int res = 0;
  if (primaryWindow->blockUntil == nullptr || (res = primaryWindow->blockUntil(primaryWindow, Key, Action, Mods)) != 0) {
    if (res == UNBLOCK) {
      primaryWindow->blockUntil = nullptr;
    }


    InputInfo& info = primaryWindow->keyMapping[Key];
    info.mods = Mods;

    if (Action == GLFW_PRESS) {
      info.pressed = true;
      info.held = true;
      info.released = false;
    } else if (Action == GLFW_RELEASE) {
      info.pressed = false;
      info.held = false;
      info.released = true;
    }

  }
}


void _closeCallback(GLFWwindow* glWindow) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];

  primaryWindow->shouldClose = true;
}


void _mouseCallback(GLFWwindow* glWindow, int Button, int Action, int Mods) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];

  int res = 0;
  if (primaryWindow->blockUntil == nullptr || (res = primaryWindow->blockUntil(primaryWindow, Button, Action, Mods)) != 0) {
    if (res == UNBLOCK) {
      primaryWindow->blockUntil = nullptr;
    }

    InputInfo& info = primaryWindow->keyMapping[Button];
    info.mods = Mods;

    if (Action == GLFW_PRESS) {
      info.pressed = true;
      info.held = true;
      info.released = false;
    } else if (Action == GLFW_RELEASE) {
      info.pressed = false;
      info.held = false;
      info.released = true;
    }
  }
}


void _mouseMoveCallback(GLFWwindow* glWindow, double xPos, double yPos) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];

  if (!primaryWindow->mouseMovedSinceReset) {
    primaryWindow->oldMouseX = primaryWindow->mouseX;
    primaryWindow->oldMouseY = primaryWindow->mouseY;
    primaryWindow->oldRealMouseX = primaryWindow->realMouseX;
    primaryWindow->oldRealMouseY = primaryWindow->realMouseY;
    primaryWindow->mouseMovedSinceReset = true;
  }

  primaryWindow->realMouseX = xPos;
  primaryWindow->realMouseY = yPos;

  primaryWindow->mouseX = (2 * xPos / primaryWindow->width) - 1;
  primaryWindow->mouseY = -((2 * yPos / primaryWindow->height) - 1);
}


void _mouseEnterCallback(GLFWwindow* glWindow, int Entered) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];

  if (Entered == GLFW_TRUE) {
    primaryWindow->mouseOver = true;
  } else {
    primaryWindow->mouseOver = false;
  }
}


void _windowFocusCallback(GLFWwindow* glWindow, int Focused) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];

  if (Focused == GLFW_TRUE) {
    primaryWindow->hasFocus = true;
  } else {
    primaryWindow->hasFocus = false;
  }
}


void _windowMaximizeCallback(GLFWwindow* glWindow, int Maximized) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];

  if (Maximized == GLFW_TRUE) {
    primaryWindow->maximized = true;
  } else {
    primaryWindow->maximized = false;
  }
}


void _windowIconifyCallback(GLFWwindow* glWindow, int Iconified) {
  if (!windowToWindowMap.contains(glWindow)) {
    return;
  }

  Window* primaryWindow = windowToWindowMap[glWindow];

  if (Iconified == GLFW_TRUE) {
    primaryWindow->visible = false;
  } else {
    primaryWindow->visible = true;
  }
}




// STATIC
//----------------------------------------
Window* Window::GetCurrentContext() {
  return currentContext;
}
//----------------------------------------
// STATIC





//--------------------
void Window::SetSize(int Width, int Height) {
  if (windowRef == nullptr) {
    return;
  }

  glfwSetWindowSize(windowRef, Width, Height);
}



//--------------------
void Window::SetTitle(std::string Title) {
  if (windowRef == nullptr) {
    return;
  }

  title = Title;
  glfwSetWindowTitle(windowRef, Title.c_str());
}



//--------------------
void Window::SetFocus() {
  if (windowRef == nullptr) {
    return;
  }

  glfwFocusWindow(windowRef);
}



//--------------------
void Window::SetMouseScroll(double X, double Y) {
  mouseScrollX = X;
  mouseScrollY = Y;
}



//--------------------
void Window::SetMousePosition(double X, double Y) {
  if (windowRef == nullptr) {
    return;
  }

  double realX = (X + aspectRatio) / 2 * height;
  double realY = (Y + 1) / 2 * height;

  glfwSetCursorPos(windowRef, realX, realY);
}



//--------------------
void Window::SetValidity(bool Valid) {
  valid = Valid;
}



//--------------------
void Window::SetSwapInterval(int i) {
  glfwSwapInterval(i);
}



//--------------------
void Window::SetShouldClose(bool b) {
  shouldClose = b;
}



//--------------------
void Window::RequestFocus() {
  glfwRequestWindowAttention(windowRef);
}



//--------------------
void Window::SetBlockFunction(BlockReturn (*func)(Window*, int, int, int)) {
  blockUntil = func;
}



//--------------------
void Window::SetReadFunction(void (*func)(Window*, unsigned int)) {
  readFunction = func;
}



//--------------------
void Window::SetCursorInputMode(int val) {
  glfwSetInputMode(windowRef, GLFW_CURSOR, val);
}


//--------------------
int Window::GetWidth() const { return width; }
int Window::GetHeight() const { return height; }
bool Window::WidthLarger() const { return width > height; }



//--------------------
double Window::GetMouseScrollX() const { return mouseScrollX; }
double Window::GetMouseScrollY() const { return mouseScrollY; }


#ifndef _PPM_VECTOR
IVector2
#else
::pPack::IVector2
#endif
Window::GetSize() const { return { width, height }; }

double Window::GetAspectRatio() const { return aspectRatio; }
std::string Window::GetTitle() const { return title; }
bool Window::GetMaximized() const { return maximized; }
bool Window::GetVisible() const { return visible; }
bool Window::GetValidity() const { return valid; }

#ifndef _PPM_VECTOR
DVector2
#else
::pPack::DVector2
#endif
Window::GetMousePosition() const { return { mouseX, mouseY }; }


#ifndef _PPM_VECTOR
DVector2
#else
::pPack::DVector2
#endif
Window::GetDeltaMousePosition() const { return {mouseX - oldMouseX, mouseY - oldMouseY}; }


#ifndef _PPM_VECTOR
DVector2
#else
::pPack::DVector2
#endif
Window::GetRealMousePosition() const { return { realMouseX, realMouseY }; }


#ifndef _PPM_VECTOR
DVector2
#else
::pPack::DVector2
#endif
Window::GetDeltaRealMousePosition() const { return {realMouseY - oldRealMouseY, realMouseX - oldRealMouseX}; }



//--------------------
void Window::Open(int Width, int Height, std::string Title, WindowCreateHint* Hints, int HintCount) {
  if (windowRef != nullptr) {
    Close();
  }

  glfwDefaultWindowHints();
  for (int i = 0; i < HintCount; i++) {
    glfwWindowHint(Hints[i].hint, Hints[i].value);
  }

  windowRef = glfwCreateWindow(Width, Height, Title.c_str(), nullptr, nullptr);
  if (!windowRef) {
    const char* description;
    int code = glfwGetError(&description);

    if (code != GLFW_NO_ERROR) {
      std::cout << description << '\n';
    }
    return;
  }

  width = Width;
  height = Height;
  title = Title;
  aspectRatio = (double)width / height;
  hasFocus = true;

  windowToWindowMap[windowRef] = this;

  glfwSetWindowSizeCallback(windowRef, _windowResizedCallback);
  glfwSetScrollCallback(windowRef, _mouseScrollCallback);
  glfwSetCharCallback(windowRef, _charCallback);
  glfwSetKeyCallback(windowRef, _keyCallback);
  glfwSetWindowCloseCallback(windowRef, _closeCallback);
  glfwSetMouseButtonCallback(windowRef, _mouseCallback);
  glfwSetCursorPosCallback(windowRef, _mouseMoveCallback);
  glfwSetCursorEnterCallback(windowRef, _mouseEnterCallback);
  glfwSetWindowFocusCallback(windowRef, _windowFocusCallback);
  glfwSetWindowMaximizeCallback(windowRef, _windowMaximizeCallback);
  glfwSetWindowIconifyCallback(windowRef, _windowIconifyCallback);

}


//--------------------
void Window::Close() {
  if (windowRef == nullptr) {
    return;
  }
  if (windowToWindowMap.contains(windowRef))
    windowToWindowMap.erase(windowRef);

  glfwDestroyWindow(windowRef);
  windowRef = nullptr;
  shouldClose = false;
}




//--------------------
bool Window::IsOpen() {
  return windowRef != nullptr;
}


bool Window::ShouldClose() {
  return shouldClose;
}




//--------------------
void Window::Show() {
  if (windowRef == nullptr) {
    return;
  }

  glfwShowWindow(windowRef);
}




//--------------------
void Window::Maximize() {
  if (windowRef == nullptr) {
    return;
  }

  glfwMaximizeWindow(windowRef);
}


void Window::Restore() {
  if (windowRef == nullptr) {
    return;
  }

  glfwRestoreWindow(windowRef);
}


//--------------------
void Window::Minimize() {
  if (windowRef == nullptr) {
    return;
  }

  glfwIconifyWindow(windowRef);
}



//--------------------
void Window::ResetKeys() {
  if (windowRef == nullptr) {
    return;
  }

  for (auto& keyI : keyMapping) {
    keyI.second.pressed = false;
    keyI.second.released = false;
  }
  for (auto& mouseI : mouseMapping) {
    mouseI.second.pressed = false;
    mouseI.second.released = false;
  }

  mouseScrollX = 0;
  mouseScrollY = 0;

  oldMouseX = mouseX;
  oldMouseY = mouseY;

  mouseMovedSinceReset = false;
}



//--------------------
void Window::Swap() {
  if (windowRef == nullptr) {
    return;
  }

  glfwSwapBuffers(windowRef);
}


//--------------------
void Window::SetCursor(GLFWcursor* cursor) {
  glfwSetCursor(windowRef, cursor);
}


//--------------------
void Window::SetAsContext() {
  if (windowRef == nullptr) {
    return;
  }
  currentContext = this;

  glfwMakeContextCurrent(windowRef);
}




//--------------------
#ifdef _PPM_GLAD
void Window::SetViewport() {
  glViewport(0, 0, width, height);
}
#endif




//--------------------
Window::Window() : windowRef(nullptr), width(0), height(0), aspectRatio(0), title(),
keyMapping(), mouseMapping(), realMouseX(0), realMouseY(0), mouseX(0), mouseY(0),
mouseOver(false), hasFocus(false), maximized(false), visible(true), valid(false),
shouldClose(false), blockUntil(nullptr), readFunction(nullptr), oldMouseX(0), oldMouseY(0), mouseMovedSinceReset(false) {}


//--------------------
Window::~Window() {
  Close();
}


void GLFWPollEvents() {
  glfwPollEvents();
}

GLFWglproc (*GetGLFWProcAddress())(char const*) {
  return glfwGetProcAddress;
}



}; // namepspace pPack