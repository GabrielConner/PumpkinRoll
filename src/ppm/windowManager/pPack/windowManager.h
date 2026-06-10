#ifndef PPM_INCLUDE_PPACK_WINDOW_MANAGER_H
#define PPM_INCLUDE_PPACK_WINDOW_MANAGER_H
#define GLFW_INCLUDE_NONE

#include "GLFW\glfw3.h"
#include <string>
#include <map>



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

  static Window* GetCurrentContext();



  InputInfo GetInput(int Key) {
    return keyMapping[Key];
  }



  void SetSize(int Width, int Height);
  void SetTitle(std::string Title);
  void SetFocus();
  void SetMouseScroll(double X, double Y);
  void SetMousePosition(double X, double Y);
  void SetValidity(bool Valid);

  void SetSwapInterval(int i);
  void SetShouldClose(bool set);
  void SetBlockFunction(BlockReturn (*func)(Window*, int, int, int));
  void SetReadFunction(void (*func)(Window*, unsigned int));
  void SetCursorInputMode(int val);


  void RequestFocus();


  int GetWidth() const;
  int GetHeight() const;

  bool WidthLarger() const;

  double GetMouseScrollX() const;
  double GetMouseScrollY() const;


#ifndef _PPM_VECTOR
  IVector2
#else
  ::pPack::IVector2
#endif 
    GetSize() const;


  double GetAspectRatio() const;

  std::string GetTitle() const;
  bool GetMaximized() const;
  bool GetVisible() const;
  bool GetValidity() const;


#ifndef _PPM_VECTOR
  DVector2
#else
  ::pPack::DVector2
#endif  
GetMousePosition() const;


#ifndef _PPM_VECTOR
  DVector2
#else
  ::pPack::DVector2
#endif  
GetDeltaMousePosition() const;


#ifndef _PPM_VECTOR
  DVector2
#else
  ::pPack::DVector2
#endif  
GetRealMousePosition() const;



  void Open(int Width, int Height, std::string Title, WindowCreateHint* Hints, int HintCount);
  void Close();


  bool IsOpen();
  bool ShouldClose();


  void Show();


  void Maximize();
  void Restore();
  void Minimize();


  void ResetKeys();

  void Swap();

  void SetCursor(GLFWcursor* cursor);

  void SetAsContext();


#ifdef _PPM_GLAD
  void SetViewport();
#endif


  Window();
  ~Window();
};




}; // namespace pPack


#endif