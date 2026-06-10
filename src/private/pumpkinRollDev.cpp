#ifdef PUMPKIN_ROLL_DEV
#include "private/pumpkinRollDev.h"

#include "pumpkin/types.h"
#include "private/types.h"
#include "private/model.h"
#include "private/pumpkinRoll.h"

#include "pPack/windowManager.h"

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <assert.h>

#include <conio.h>

using namespace ::pPack;
using namespace ::pumpkin;
using namespace ::pumpkin_private;


namespace {

// Abstract Ask struct
struct Ask {
  virtual void Prompt(int i, std::string const& line) = 0;
  virtual void Question(std::string const& line) = 0;
  virtual void Set() {}
};


// Helper functions
void ListObjects();
void ListModels();
inline bool ToNumberFromAscii(int& i) { i -= 48; return i < 0 || i > 9; }


/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                          A S K   T Y P E S                            */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/

struct MainMenu : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
} mainMenu;


struct CreateObject : Ask {

  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
} createObject;


struct SelectObject : Ask {
  bool display = false;

  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
} selectObject;


struct HoldObject : Ask {

  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
} holdObject;



// Data container
struct Data {
  std::string line = "";

  Object* selectedObject = nullptr;
  Pumpkin* pumpkin = nullptr;
  std::vector<std::string> messages;
  Ask* ask = nullptr;

  bool updateAsk = true;

  void SetAsk(Ask* Ask) {
    ask = Ask;
    ask->Set();
    line = "";
    updateAsk = true;
  }

  void ResetAsk() {
    if (selectedObject) {
      SetAsk(&holdObject);
    } else {
      SetAsk(&mainMenu);
    }
  }

}*data = nullptr;


}; // namespace









namespace pumpkin_private {

// Assumes that the proper order is being followed for Starting/Ending since kept private

void StartDevelopment() {
  assert(data == nullptr);

  data = new Data();
  data->pumpkin = GetPumpkin();
  data->SetAsk(&mainMenu);
}


void UpdateDevelopment() {
  assert(data != nullptr);
  assert(data->ask != nullptr);

  if (data->updateAsk) {

  // Works but still keeps the scrollbar
/*    CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(hStdout, &csbi);
    SMALL_RECT scrollRect;
    scrollRect.Left = 0;
    scrollRect.Top = 0;
    scrollRect.Bottom = csbi.dwSize.Y;
    scrollRect.Right = csbi.dwSize.X;

    COORD dest;
    dest.X = 0;
    dest.Y = -csbi.dwSize.Y;

    CHAR_INFO cInfo;
    cInfo.Char.UnicodeChar = ' ';
    cInfo.Attributes = csbi.wAttributes;
    ScrollConsoleScreenBuffer(hStdout, &scrollRect, NULL, dest, &cInfo);


    dest.X = 0;
    dest.Y = csbi.dwSize.Y;
    std::cout << std::string(10, '\n');
    SetConsoleCursorPosition(hStdout, dest);

    dest.X = 0;
    dest.Y = 0;
    
    SetConsoleCursorPosition(hStdout, dest);*/


    // idc if it's slow and unsafe
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif

    data->ask->Question(data->line);

    data->updateAsk = false;
  }

  while (_kbhit()) {
    int ret = _getch_nolock();
    if (ret == 0xE0 || ret == 0x00) {
      ret = _getch_nolock();
      continue;
    }

    if (std::isgraph(ret)) data->line += (char)ret;

    if (ret == _BACKSPACE && data->line.size() > 0) data->line.erase(data->line.size() - 1);

    data->ask->Prompt(ret, data->line);

    if (ret == '\r' || ret == '\n') {
      data->line = "";
    }
  }
}


void EndDevelopment() {
  assert(data != nullptr);
  delete(data);
}

}; // namespace pumpkin_private




namespace {



// MainMenu
// **************************************************
// **************************************************

void MainMenu::Prompt(int i, std::string const& line) {
  assert(data);

  if (ToNumberFromAscii(i)) return;

  switch (i) {
    case 0:
      data->SetAsk(&createObject);
      break;
    case 1:
      data->SetAsk(&selectObject);
      break;
  }
}


void MainMenu::Question(std::string const& line) {
  std::cout << "0. Create object\n1. Select object\n";
}


// **************************************************
// **************************************************
// MainMenu





// CreateObject
// **************************************************
// **************************************************

void CreateObject::Prompt(int i, std::string const& line) {
  assert(data);

  if (i == '\r' || i == '\n') {
    if (line.size() == 0) {
      data->SetAsk(&mainMenu);
      return;
    }
    
    Object* render = RegisterObject(line);
    if (!render) {
      pWarn((std::string("Failed to create ") + line).c_str());
      return;
    }

    Object_SetModel(render, GetModel("PumpkinRoll_Default"));
    render->transform.rotation.x = 90;

    data->SetAsk(&mainMenu);
    return;
  }

  data->updateAsk = true;
}


void CreateObject::Question(std::string const& line) {
  std::cout << "Enter object name or empty to return\n>>" << line;
}

// **************************************************
// **************************************************
// CreateObject





// SelectObject
// **************************************************
// **************************************************

void SelectObject::Prompt(int i, std::string const& line) {
  assert(data);

  if (i == _ESCAPE) {
    display = !display;
    data->updateAsk = true;
    return;
  }

  if (i == '\r' || i == '\n') {
    if (line.size() == 0) {
      data->SetAsk(&mainMenu);
    }


    data->selectedObject = GetObject(line);
    if (data->selectedObject == nullptr) {
      pWarn("No object selected");
    }
    data->ResetAsk();
    return;
  }

  data->updateAsk = true;
}


void SelectObject::Question(std::string const& line) {
  assert(data);

  if (display) {
    std::vector<std::string> objList;
    for (auto objP : data->pumpkin->registeredObjects) {
      objList.push_back(Object_GetName(objP.second));
    }
    std::sort(objList.begin(), objList.end(), [](std::string const& a, std::string const& b) {
      int sumA = std::accumulate(a.begin(), a.end(), 0);
      int sumB = std::accumulate(b.begin(), b.end(), 0);
      return sumA < sumB;
    });
    for (auto obj : objList) std::cout << obj << '\n';
    std::cout << "\n\n";
  }

  std::cout << "Enter object name to select or empty to go back\n[ESCAPE] Toggle object list\n>>" << line;
}

// **************************************************
// **************************************************
// SelectObject





// HoldObject
// **************************************************
// **************************************************

void HoldObject::Prompt(int i, std::string const& line) {
  assert(data);
  if (ToNumberFromAscii(i)) return;

  switch (i) {
    case 0:
      ::pumpkin::DeleteObject(pObjInt(data->selectedObject)->name);
      data->selectedObject = nullptr;
      data->SetAsk(&mainMenu);
    break;
  }
}


void HoldObject::Question(std::string const& line) {
  std::cout << "[OBJECT : " << pObjInt(data->selectedObject)->name << "]\n\n" << "0. Delete object\n1. Set object model\n2. Set object property";
}

// **************************************************
// **************************************************
// HoldObject





/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                       L I S T I N G   T H I N G S                     */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/

void ListObjects() {
  std::vector<std::string> objList;
  for (auto objP : data->pumpkin->registeredObjects) {
    objList.push_back(Object_GetName(objP.second));
  }
  std::sort(objList.begin(), objList.end(), [](std::string const& a, std::string const& b) {
    int sumA = std::accumulate(a.begin(), a.end(), 0);
    int sumB = std::accumulate(b.begin(), b.end(), 0);
    return sumA < sumB;
  });
  for (auto obj : objList) std::cout << obj << '\n';
}


void ListModels() {
  std::vector<std::string> list;
  for (auto t : data->pumpkin->registeredModels) {
    list.push_back(t.second->name);
  }
  std::sort(list.begin(), list.end(), [](std::string const& a, std::string const& b) {
    int sumA = std::accumulate(a.begin(), a.end(), 0);
    int sumB = std::accumulate(b.begin(), b.end(), 0);
    return sumA < sumB;
  });
  for (auto t : list) std::cout << t << '\n';
}

}; // namespace


#endif