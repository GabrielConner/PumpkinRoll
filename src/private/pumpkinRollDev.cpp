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

inline constexpr int _MAX_HISTORY = 16;


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
};


struct CreateObject : Ask {

  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
};


struct HoldObject : Ask {

  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
};


struct SelectObject : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
};


struct SelectModel : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
};




// Return data to go backs
struct DataHistorySlice {
  Ask* ask = nullptr;
  Object* holdingObject = nullptr;
  Object* selectedObject = nullptr;
  Model* selectedModel = nullptr;
};



// Data container
struct Data {
  MainMenu mainMenu;
  CreateObject createObject;
  HoldObject holdObject;
  SelectObject selectObject;
  SelectModel selectModel;

  std::string line = "";

  Object* holdingObject = nullptr;
  Object* selectedObject = nullptr;
  Model* selectedModel = nullptr;


  Pumpkin* pumpkin = nullptr;
  Ask* ask = nullptr;


  std::vector<std::string> messages;
  DataHistorySlice history[_MAX_HISTORY] = {0};


  bool display = false;
  bool updateAsk = true;
  int currentInHistory = 0;
  int historyLength = 0;



  // Handles implicit actions based on what is selected
  // Such as if there is a selectedObject and no holdingObject: set holdingObject to selectedObject
  void HandleImplicit() {
    if (holdingObject && selectedModel) {
      Object_SetModel(holdingObject, selectedModel);
      selectedModel = nullptr;
    } else if (selectedObject && !holdingObject) {
      holdingObject = selectedObject;
      selectedObject = nullptr;
    }
  }


  // Goes back in history of commands
  void GoBack() {
    if (historyLength == 0) {
      ResetAsk();
      return;
    }

    historyLength--;
    DataHistorySlice& slice = history[currentInHistory];
    currentInHistory = (currentInHistory - 1) % _MAX_HISTORY;

    ask = slice.ask;
    holdingObject = slice.holdingObject;
    selectedObject = slice.selectedObject;
    selectedModel = slice.selectedModel;

    SetAsk(slice.ask, false);
  }


  // Sets the current ask and by default saves to history
  void SetAsk(Ask* Ask, bool trigger=true) {
    if (trigger) {
      historyLength = std::min(_MAX_HISTORY, historyLength + 1);
      currentInHistory = (currentInHistory + 1) % _MAX_HISTORY;
      DataHistorySlice& slice = history[currentInHistory];

      slice.ask = ask;
      slice.holdingObject = holdingObject;
      slice.selectedObject = selectedObject;
      slice.selectedModel = selectedModel;
    }

    ask = Ask;
    ask->Set();
    line = "";

    updateAsk = true;
  }


  // Does an implicit SetAsk based on the after effects of HandleImplicit
  void ResetAsk() {
    HandleImplicit();

    if (holdingObject) {
      SetAsk(&holdObject, true);
    } else {
      SetAsk(&mainMenu, true);
    }
  }


  void ResetHold() {
    holdingObject = nullptr;
    selectedObject = nullptr;
    selectedModel = nullptr;
  }

}*data = nullptr;


}; // namespace









namespace pumpkin_private {

// Assumes that the proper order is being followed for Starting/Ending since kept private

void StartDevelopment() {
  assert(data == nullptr);

  // Create data
  data = new Data();
  data->pumpkin = GetPumpkin();
  data->ask = &data->mainMenu;
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


    // Default commands
    std::cout << "[ESCAPE to display extended]\n";
    std::cout << "[UP ARROW to clear messages]\n";
    std::cout << "[RIGHT ARROW to go back]\n";
    std::cout << "[LEFT ARROW to main menu]\n\n";

    if (data->holdingObject) {
      std::cout << "[OBJECT : " << pObjInt(data->holdingObject)->name << "]\n";
    }
    if (data->selectedModel) {
      std::cout << "[MODEL : " << data->selectedModel->name << "]\n";
    }


    // Output redirected print messages
    for (auto& msg : data->messages) {
      std::cout << msg;
    }
    std::cout << "\n\n";


    data->ask->Question(data->line);
    data->updateAsk = false;
  }


  // For all buttons that were pressed
  while (_kbhit()) {
    int ret = _getch_nolock();

    // Special key was pressed
    if (ret == 0xE0 || ret == 0x00) {

      // Get special key
      ret = _getch_nolock();
      switch (ret) {
        case _UP_ARROW:
          data->messages.clear();
          data->updateAsk = true;
          break;
        case _RIGHT_ARROW:
          data->GoBack();
          break;
        case _LEFT_ARROW:
          data->SetAsk(&data->mainMenu);
          data->ResetHold();
          break;
      }

      continue;
    }
    // escape isn't a special key 
    if (ret == _ESCAPE) {
      data->display = !data->display;
      data->updateAsk = true;
      continue;
    }

    // Is graphical key, ask prompt decides to reset with new key input or not
    if (std::isgraph(ret)) data->line += (char)ret;

    // Implicit capture
    if (ret == _BACKSPACE) {
      if (data->line.size() != 0) {
        data->line.erase(data->line.size() - 1);
      }
    }


    data->ask->Prompt(ret, data->line);
  }
}


void EndDevelopment() {
  assert(data != nullptr);
  delete(data);
}

}; // namespace pumpkin_private




namespace pumpkin {

// Redirect errors to list so they can display correctly
void PrintError(PrintLevel level, char const* file, char const* msg) {
  if (data->pumpkin != nullptr && (level == PrintLevel::NOPRINT || data->pumpkin->runtime.printLevel > level)) return;
  data->messages.push_back(std::string(msg) + '\n' + file + "\n\n");
}

}; // namespace pumpkin




namespace {



// MainMenu
// **************************************************
// **************************************************

void MainMenu::Prompt(int i, std::string const& line) {
  assert(data);

  if (ToNumberFromAscii(i)) return;

  switch (i) {
    case 0: // Create object
      data->SetAsk(&data->createObject);
      break;
    case 1: // Select object
      data->SetAsk(&data->selectObject);
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
    if (line.size() == 0) { // Go back
      data->SetAsk(&data->mainMenu);
      return;
    }
    

    Object* render = RegisterObject(line);
    if (!render) {
      pWarn((std::string("Failed to create ") + line).c_str());
      data->updateAsk = true;
      return;
    }

    // PRTODO remove this at some point
    render->transform.rotation.x = 90;

    // Isn't part of the selecting cycle stuff so can be direct
    data->SetAsk(&data->mainMenu);
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





// HoldObject
// **************************************************
// **************************************************

void HoldObject::Prompt(int i, std::string const& line) {
  assert(data);
  if (ToNumberFromAscii(i)) return;

  switch (i) {
    case 0: // Delete object
      ::pumpkin::DeleteObject(pObjInt(data->holdingObject)->name);
      data->holdingObject = nullptr;
      data->SetAsk(&data->mainMenu);
      break;
    case 1: // Set object model
      data->selectedModel = pObjInt(data->holdingObject)->model;
      data->SetAsk(&data->selectModel);
      break;
  }
}


void HoldObject::Question(std::string const& line) {
  std::cout << "0. Delete object\n1. Set object model";
}

// **************************************************
// **************************************************
// HoldObject





// SelectObject
// **************************************************
// **************************************************

void SelectObject::Prompt(int i, std::string const& line) {
  assert(data);

  if (i == '\r' || i == '\n') {
    if (line.size() == 0) { // Go back
      data->ResetAsk();
      return;
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

  if (data->display) {
    ListObjects();
  }

  std::cout << "Enter object name to select or empty to go back\n>>" << line;
}

// **************************************************
// **************************************************
// SelectObject





// SelectModel
// **************************************************
// **************************************************

void SelectModel::Prompt(int i, std::string const& line) {
  assert(data);

  if (i == '\r' || i == '\n') {
    if (line.size() == 0) { // Go back
      data->selectedModel = nullptr;
      data->ResetAsk();
      return;
    }

    data->selectedModel = GetModel(line);
    if (data->selectedModel == nullptr) {
      pWarn("No model selected");
    }
    data->ResetAsk();
    return;
  }

  data->updateAsk = true;
}


void SelectModel::Question(std::string const& line) {
  assert(data);

  if (data->display) {
    ListModels();
  }

  std::cout << "Enter model name to select or empty to go back\n>>" << line;
}

// **************************************************
// **************************************************
// SelectModel










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
  std::cout << '\n';
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
  std::cout << '\n';
}

}; // namespace


#endif