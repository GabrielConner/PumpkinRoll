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
void AddError(std::string const& msg);


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


struct HoldModel : Ask {
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


struct SelectMesh : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
};


struct SelectShader : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
};



struct HoldProperty : Ask {
  bool changing = false;
  int maxPos = 0;
  int currentPos = 0;
  bool typed = false;
  bool intType = false;
  std::string builtString = "";

  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;

  void Set() override {
    changing = false;
    maxPos = 0;
    currentPos = 0;
    typed = false;
    intType = false;
    builtString = "";
  }
};


struct SelectProperty : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
};


struct CreateProperty : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
};


struct PropertyChanger : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
};




// Return data to go backs
struct DataHistorySlice {
  Ask* ask = nullptr;
  Object* holdingObject = nullptr;
  Model* holdingModel = nullptr;
  Object* selectedObject = nullptr;
  Model* selectedModel = nullptr;

  Property* holdingProperty = nullptr;
  PropertyHolder* holdingPropertyHolder = nullptr;
};



// Data container
struct Data {
  MainMenu mainMenu;
  CreateObject createObject;
  HoldObject holdObject;
  HoldModel holdModel;
  SelectObject selectObject;
  SelectModel selectModel;
  SelectMesh selectMesh;
  SelectShader selectShader;
  HoldProperty holdProperty;
  SelectProperty selectProperty;
  CreateProperty createProperty;
  PropertyChanger propertyChanger;

  std::string line = "";
  std::string newPropertyName = "";

  Object* holdingObject = nullptr;
  Model* holdingModel = nullptr;
  Object* selectedObject = nullptr;
  Model* selectedModel = nullptr;

  Property* holdingProperty = nullptr;
  PropertyHolder* holdingPropertyHolder = nullptr;


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
    } else if (selectedModel && !holdingModel) {
      holdingModel = selectedModel;
      selectedModel = nullptr;
    }
  }


  // For when an ask deletes the data it is working on, prevents from going back
  void DeleteBack() {
    if (historyLength == 0) {
      return;
    }
    historyLength--;
    currentInHistory = (currentInHistory - 1) % _MAX_HISTORY;
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
    holdingModel = slice.holdingModel;
    selectedObject = slice.selectedObject;
    selectedModel = slice.selectedModel;
    holdingProperty = slice.holdingProperty;
    holdingPropertyHolder = slice.holdingPropertyHolder;

    SetAsk(slice.ask, false);
  }


  // Sets the current ask and by default saves to history
  void SetAsk(Ask* Ask, bool trigger=true) {
    if (trigger) {
      historyLength = std::min(_MAX_HISTORY, historyLength + 1);
      currentInHistory = (currentInHistory + 1) % _MAX_HISTORY;
      DataHistorySlice& slice = history[currentInHistory];

      slice.ask = ask;
      slice.holdingModel = holdingModel;
      slice.selectedObject = selectedObject;
      slice.selectedModel = selectedModel;
      slice.holdingProperty = holdingProperty;
      slice.holdingPropertyHolder = holdingPropertyHolder;
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
      SetAsk(&holdObject);
    } else if (holdingModel) {
      SetAsk(&holdModel);
    } else {
      SetAsk(&mainMenu);
    }
  }


  void ResetHold() {
    holdingObject = nullptr;
    holdingModel = nullptr;
    selectedObject = nullptr;
    selectedModel = nullptr;
    holdingProperty = nullptr;
    holdingPropertyHolder = nullptr;
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
  std::cerr << msg << '\n';
  return;

  assert(data);

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
    case 0: // Run
      break;
    case 1: // Primary camera view
      break;
    case 2: // Create object
      data->SetAsk(&data->createObject);
      break;
    case 3: // Create camera
      break;
    case 4: // Select object
      data->SetAsk(&data->selectObject);
      break;
    case 5: // Select model
      data->SetAsk(&data->selectModel);
      break;
    case 6: // Select Shader
      break;
    case 7: // Select camera
      break;
  }
}


void MainMenu::Question(std::string const& line) {
  std::cout << "0. Run\n1. Primary camera view\n2. Create object\n3. Create camera\n4. Select object\n5. Select Model\n6. Select Shader\n7. Select camera";
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
      AddError((std::string("Failed to create ") + line).c_str());
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
  std::cout << "0. Delete object\n1. Set object model\n3. Add script\n4. Select script";
}

// **************************************************
// **************************************************
// HoldObject





// HoldModel
// **************************************************
// **************************************************

void HoldModel::Prompt(int i, std::string const& line) {
  assert(data);
  assert(data->holdingModel);

  if (ToNumberFromAscii(i)) return;

  switch (i) {
    case 0: // Set mesh
      break;
    case 1: // Set shader
      break;
    case 2: // Select property
      data->holdingPropertyHolder = &data->holdingModel->properties;
      data->SetAsk(&data->propertyChanger);
      break;
  }
}


void HoldModel::Question(std::string const& line) {
  std::cout << "0. Set mesh\n1. Set shader\n2. Select property";
}

// **************************************************
// **************************************************
// HoldModel





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
      AddError("No object selected");
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
      AddError("No model selected");
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





// SelectMesh
// **************************************************
// **************************************************

void SelectMesh::Prompt(int i, std::string const& line) {
  assert(data);

  if (i == '\r' || i == '\n') {
    if (line.size() == 0) { // Go back
      data->selectedModel = nullptr;
      data->ResetAsk();
      return;
    }

    data->selectedModel = GetModel(line);
    if (data->selectedModel == nullptr) {
      AddError("No model selected");
    }
    data->ResetAsk();
    return;
  }

  data->updateAsk = true;
}


void SelectMesh::Question(std::string const& line) {
  assert(data);

  if (data->display) {
    ListModels();
  }

  std::cout << "Enter model name to select or empty to go back\n>>" << line;
}

// **************************************************
// **************************************************
// SelectMesh





// SelectShader
// **************************************************
// **************************************************

void SelectShader::Prompt(int i, std::string const& line) {
  assert(data);

  if (i == '\r' || i == '\n') {
    if (line.size() == 0) { // Go back
      data->selectedModel = nullptr;
      data->ResetAsk();
      return;
    }

    data->selectedModel = GetModel(line);
    if (data->selectedModel == nullptr) {
      AddError("No model selected");
    }
    data->ResetAsk();
    return;
  }

  data->updateAsk = true;
}


void SelectShader::Question(std::string const& line) {
  assert(data);

  if (data->display) {
    ListModels();
  }

  std::cout << "Enter model name to select or empty to go back\n>>" << line;
}

// **************************************************
// **************************************************
// SelectShader





// HoldProperty
// **************************************************
// **************************************************

void HoldProperty::Prompt(int i, std::string const& line) {
  assert(data);
  assert(data->holdingProperty);

  if (changing) {
    if (i == '\n' || i == '\r') {
      if (typed) {
        if (intType) *((int32_t*)data->holdingProperty->prop + currentPos) = std::strtol(builtString.c_str(), nullptr, 10);
        else *((float*)data->holdingProperty->prop + currentPos) = std::strtof(builtString.c_str(), nullptr);
      }

      changing = false;
      data->updateAsk = true;
      Set();

      return;
    }

    i = std::tolower(i);
    
    if (i == 'w' || i == 's' || i == 'a' || i == 'd' || i == _BACKSPACE) {
      if (i == _BACKSPACE) {
          builtString.pop_back();
      }

      if (typed) {
        if (intType) *((int32_t*)data->holdingProperty->prop + currentPos) = (int32_t)std::strtol(builtString.c_str(), nullptr, 10);
        else *((float*)data->holdingProperty->prop + currentPos) = std::strtof(builtString.c_str(), nullptr);
        builtString.clear();
      }

      if (i == 'w' && currentPos >= 4) currentPos -= 4;
      else if (i == 's' && currentPos < maxPos - 4) currentPos += 4;
      else if (i == 'a' && currentPos > 0) currentPos--;
      else if (i == 'd' && currentPos < maxPos - 1) currentPos++;
      else return;

      typed = false;
      data->updateAsk = true;
      return;
    }


    if (intType) {
      if (!std::isdigit(i) && i != '-' && i != '+') return;
    } else if (!std::isdigit(i) && i != 'e' && i != '-' && i != '+' && i != '.' && i != 'f') return;

    typed = true;
    builtString += (char)i;
    data->updateAsk = true;
    
    return;
  }


  if (ToNumberFromAscii(i)) return;

  switch (i) {
    case 0: // Change

      changing = true;
      maxPos = data->holdingProperty->typeSize / sizeof(float);
      intType = data->holdingProperty->type == VariableType::INT;
      data->updateAsk = true;
      

      break;
    case 1: // Delete

      data->holdingPropertyHolder->properties.erase(_STRING_HASHER(data->holdingProperty->name));
      data->DeleteBack();
      data->SetAsk(&data->propertyChanger);
      data->holdingProperty = nullptr;

      break;
  }
}


void HoldProperty::Question(std::string const& line) {
  assert(data);

  if (changing) {
    std::cout << "Press ENTER to stop changing\n\n";

    float* fl = (float*)data->holdingProperty->prop;
    int32_t* in = (int32_t*)data->holdingProperty->prop;

    for (char i = 0; i < maxPos; i++) {
      std::cout << '[';

      if (i == currentPos) std::cout << "*";

      if (i != currentPos || !typed) {
        if (intType) std::cout << *(in + i);
        else std::cout << *(fl + i);
      } else {
        std::cout << builtString;
      }

      if (i == currentPos) std::cout << "*";


      std::cout << ']';

      if ((i + 1) % 4 == 0) {
        std::cout << '\n';
      }
    }
    return;
  }

  std::cout << "0. Change\n1. Delete\n";
}

// **************************************************
// **************************************************
// HoldProperty





// SelectProperty
// **************************************************
// **************************************************

void SelectProperty::Prompt(int i, std::string const& line) {
  assert(data);
  assert(data->holdingPropertyHolder);

  if (i == '\n' || i == '\r') {
    if (line.size() == 0) {
      data->SetAsk(&data->propertyChanger, false);
      return;
    }

    auto find = data->holdingPropertyHolder->properties.find(_STRING_HASHER(line));
    if (find == data->holdingPropertyHolder->properties.end()) {
      AddError("Failed to find property");
      data->SetAsk(&data->selectProperty, false);
      return;
    }

    data->holdingProperty = &find->second;
    data->SetAsk(&data->holdProperty);
  }

  data->updateAsk = true;
}


void SelectProperty::Question(std::string const& line) {
  assert(data);
  assert(data->holdingPropertyHolder);

  if (data->display) {
    data->holdingPropertyHolder->PrintAll();
  }

  std::cout << "Enter name of property to select or empty to go back\n>>" << line;
}

// **************************************************
// **************************************************
// SelectProperty





// CreateProperty
// **************************************************
// **************************************************

void CreateProperty::Prompt(int i, std::string const& line) {

}


void CreateProperty::Question(std::string const& line) {

}

// **************************************************
// **************************************************
// CreateProperty





// PropertyChanger
// **************************************************
// **************************************************

void PropertyChanger::Prompt(int i, std::string const& line) {
  assert(data);

  if (ToNumberFromAscii(i)) return;

  switch (i) {
    case 0: // Select property
      data->SetAsk(&data->selectProperty);
      break;
    case 1: // Create property
      data->SetAsk(&data->createProperty);
      break;
  }
}


void PropertyChanger::Question(std::string const& line) {
  assert(data);

  std::cout << "0. Select property\n1. Create property\n";
}

// **************************************************
// **************************************************
// PropertyChanger








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
  for (auto obj : objList) std::cout << obj << "\n\n";
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
  for (auto t : list) std::cout << t << "\n\n";
}



void AddError(std::string const& msg) {
  assert(data);
  data->messages.push_back(msg);
}

}; // namespace


#endif