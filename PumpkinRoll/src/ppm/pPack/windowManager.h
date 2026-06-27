#ifndef PPM_INCLUDE_PPACK_WINDOW_MANAGER_H
#define PPM_INCLUDE_PPACK_WINDOW_MANAGER_H
#define GLFW_INCLUDE_NONE

#include "GLFW\glfw3.h"
#include <string>
#include <map>


#ifdef PUMPKIN_ROLL_LIB
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif


// Intellisense is happy with this
#if _PPM_VECTOR == 1

#include "pPack/vector.h"

#else

struct IVector2 {
  int x;
  int y;
};

struct DVector2 {
  double x;
  double y;
};

#endif


namespace pPack {


enum BlockReturn {BLOCK = 0, ALLOW, UNBLOCK};


struct InputInfo {
  bool pressed;
  bool held;
  bool released;
  int mods;

  InputInfo() : pressed(false), held(false), released(false), mods(0) {}
  InputInfo(bool Pressed, bool Held, bool Released, int Mods) : pressed(Pressed), held(Held), released(Released), mods(Mods) {}
};

struct WindowCreateHint {
  int hint;
  int value;
};



class Window {
  friend void _windowResizedCallback(GLFWwindow* Window, int Width, int Height);
  friend void _mouseScrollCallback(GLFWwindow* glWindow, double xoffset, double yoffset);
  friend void _charCallback(GLFWwindow* glWindow, unsigned int codepoint);
  friend void _keyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods);
  friend void _closeCallback(GLFWwindow* Window);
  friend void _mouseCallback(GLFWwindow* glWindow, int Button, int Action, int Mods);
  friend void _mouseMoveCallback(GLFWwindow* glWindow, double xPos, double yPos);
  friend void _mouseEnterCallback(GLFWwindow* glWindow, int Entered);
  friend void _windowFocusCallback(GLFWwindow* glWindow, int Focused);
  friend void _windowMaximizeCallback(GLFWwindow* glWindow, int Maximized);
  friend void _windowIconifyCallback(GLFWwindow* glWindow, int Maximized);



  int width;
  int height;
  double aspectRatio;
  std::string title;

  std::map<int, InputInfo> keyMapping;
  std::map<int, InputInfo> mouseMapping;

  double realMouseX;
  double realMouseY;

  double oldMouseX;
  double oldMouseY;

  double mouseX;
  double mouseY;

  double mouseScrollX;
  double mouseScrollY;

  bool mouseOver;
  bool hasFocus;
  bool maximized;
  bool visible;

  bool valid;
  bool shouldClose;
  bool mouseMovedSinceReset;

  BlockReturn (*blockUntil)(Window*, int, int, int);
  void (*readFunction)(Window*, unsigned int);

public:
  GLFWwindow* windowRef;

  API static Window* GetCurrentContext();



  InputInfo GetInput(int Key) {
    return keyMapping[Key];
  }



  API void SetSize(int Width, int Height);
  API void SetTitle(std::string Title);
  API void SetFocus();
  API void SetMouseScroll(double X, double Y);
  API void SetMousePosition(double X, double Y);
  API void SetValidity(bool Valid);

  API void SetSwapInterval(int i);
  API void SetShouldClose(bool set);
  API void SetBlockFunction(BlockReturn (*func)(Window*, int, int, int));
  API void SetReadFunction(void (*func)(Window*, unsigned int));
  API void SetCursorInputMode(int val);


  API void RequestFocus();


  API int GetWidth() const;
  API int GetHeight() const;

  API bool WidthLarger() const;

  API double GetMouseScrollX() const;
  API double GetMouseScrollY() const;


#ifndef _PPM_VECTOR
  IVector2
#else
  ::pPack::IVector2
#endif 
    API GetSize() const;


  API double GetAspectRatio() const;

  API std::string GetTitle() const;
  API bool GetMaximized() const;
  API bool GetVisible() const;
  API bool GetValidity() const;


#ifndef _PPM_VECTOR
  DVector2
#else
  ::pPack::DVector2
#endif  
    API GetMousePosition() const;


#ifndef _PPM_VECTOR
  DVector2
#else
  ::pPack::DVector2
#endif  
    API GetDeltaMousePosition() const;


#ifndef _PPM_VECTOR
  DVector2
#else
  ::pPack::DVector2
#endif  
    API GetRealMousePosition() const;



  API void Open(int Width, int Height, std::string Title, WindowCreateHint* Hints, int HintCount);
  API void Close();


  API bool IsOpen();
  API bool ShouldClose();


  API void Show();


  API void Maximize();
  API void Restore();
  API void Minimize();


  API void ResetKeys();

  API void Swap();

  API void SetCursor(GLFWcursor* cursor);

  API void SetAsContext();


#ifdef _PPM_GLAD
  API void SetViewport();
#endif


  API Window();
  API ~Window();
};


API void GLFWPollEvents();
API GLFWglproc(*GetGLFWProcAddress())(char const*);

}; // namespace pPack


#endif