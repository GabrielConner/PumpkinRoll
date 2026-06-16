#ifdef PUMPKIN_ROLL_DEV
#include "private/pumpkinRollDev.h"

#include "pumpkin/types.h"
#include "private/types.h"
#include "private/mesh.h"
#include "private/model.h"
#include "private/shader.h"
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

bool StringSort(std::string const& a, std::string const& b);

void ListObjects();
void ListMeshes();
void ListModels();
void ListShaders();
inline bool ToNumberFromAscii(int& i) { i -= 48; return i < 0 || i > 9; }
void AddError(std::string const& msg);


void ModelForward();
void ModelBack();

void MeshForward();
void MeshBack();

void ShaderForward();
void ShaderBack();

void ObjectForward();
void ObjectBack();

void PropertyForward();
void PropertyBack();


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


struct HoldShader : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
};


struct SelectObject : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
  void Set() override;
};


struct SelectModel : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
  void Set() override;
};


struct SelectMesh : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
  void Set() override;
};


struct SelectShader : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
  void Set() override;
};



struct HoldProperty : Ask {
  bool changing = false;
  int maxPos = 0;
  int currentPos = 0;
  bool intType = false;
  std::string builtString = "";

  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;

  void Set() override {
    changing = false;
    maxPos = 0;
    currentPos = 0;
    intType = false;
    builtString = "";
  }
};


struct SelectProperty : Ask {
  void Prompt(int i, std::string const& line) override;
  void Question(std::string const& line) override;
  void Set() override;
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
  Shader* holdingShader = nullptr;
  Object* selectedObject = nullptr;
  Model* selectedModel = nullptr;
  Shader* selectedShader = nullptr;
  Mesh* selectedMesh = nullptr;

  Property* holdingProperty = nullptr;
  PropertyHolder* holdingPropertyHolder = nullptr;
};



// Data container
struct Data {
  MainMenu mainMenu;
  CreateObject createObject;
  HoldObject holdObject;
  HoldModel holdModel;
  HoldShader holdShader;
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
  Shader* holdingShader = nullptr;
  Object* selectedObject = nullptr;
  Model* selectedModel = nullptr;
  Shader* selectedShader = nullptr;
  Mesh* selectedMesh = nullptr;

  Property* holdingProperty = nullptr;
  PropertyHolder* holdingPropertyHolder = nullptr;


  std::unordered_map<size_t, Model*>::iterator currentModel;
  std::unordered_map<size_t, Mesh*>::iterator currentMesh;
  std::unordered_map<size_t, Shader*>::iterator currentShader;
  std::unordered_map<size_t, Object*>::iterator currentObject;
  std::unordered_map<size_t, Property>::iterator currentProperty;



  Pumpkin* pumpkin = nullptr;
  Ask* ask = nullptr;

  void (*forward)() = nullptr;
  void (*back)() = nullptr;

  std::vector<std::string> messages;
  DataHistorySlice history[_MAX_HISTORY] = {0};


  bool cycle = false;
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
    } else if (selectedShader && !holdingShader) {
      holdingShader = selectedShader;
      selectedShader = nullptr;
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
    holdingShader = slice.holdingShader;
    selectedObject = slice.selectedObject;
    selectedModel = slice.selectedModel;
    selectedShader = slice.selectedShader;
    selectedMesh = slice.selectedMesh;


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
      slice.holdingObject = holdingObject;
      slice.holdingModel = holdingModel;
      slice.holdingShader = holdingShader;
      slice.selectedObject = selectedObject;
      slice.selectedModel = selectedModel;
      slice.selectedShader = selectedShader;
      slice.selectedMesh = selectedMesh;

      slice.holdingProperty = holdingProperty;
      slice.holdingPropertyHolder = holdingPropertyHolder;

    }

    forward = nullptr;
    back = nullptr;

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
    } else if (holdingShader) {
      SetAsk(&holdShader);
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
    std::cout << "[ESCAPE to display extended (" << std::boolalpha << data->display << std::boolalpha << ")]\n";
    std::cout << "[UP ARROW to clear messages]\n";
    std::cout << "[RIGHT ARROW to go back]\n";
    std::cout << "[LEFT ARROW to main menu]\n";
    std::cout << "[HOME to use cycling (" << std::boolalpha << data->cycle << std::boolalpha << ")]\n";
    std::cout << "[PAGE UP and DOWN to cycle options when applicable]\n\n";

    if (data->holdingObject) {
      std::cout << "[OBJECT : " << pObjInt(data->holdingObject)->name << "]\n";
    }
    if (data->holdingModel) {
      std::cout << "[MODEL : " << data->holdingModel->name << "]\n";
    }
    if (data->holdingShader) {
      std::cout << "[SHADER : " << data->holdingShader->name << "]\n";
    }
    if (data->holdingProperty) {
      std::cout << "[PROPERTY : " << data->holdingProperty->name << "]\n";
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
        case _PAGE_UP:
          if (data->forward) data->forward();
          data->updateAsk = true;
          break;
        case _PAGE_DOWN:
          if (data->back) data->back();
          data->updateAsk = true;
          break;
        case _HOME:
          data->cycle = !data->cycle;
          data->updateAsk = true;
          break;
      }

      return;
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
    case 0: // Run //prtodo
      break;
    case 1: // Create object
      data->SetAsk(&data->createObject);
      break;
    case 2: // Select object //prtodo
      data->SetAsk(&data->selectObject);
      break;
    case 3: // Select model
      data->SetAsk(&data->selectModel);
      break;
    case 4: // Select shader //prtodo
      break;
    case 5: // Select camera //prtodo
      break;
    case 6: // Primary camera view //prtodo
      break;
    case 7: // Build //prtodo
      break;
    case 8: // Save //prtodo
      break;
    case 9: // Exit //prtodo
      break;
  }
}


void MainMenu::Question(std::string const& line) {
  std::cout << "0. Run\n1. Create object\n2. Select object\n3. Select model\n4. Select shader\n5. Select camera\n6. Primary camera view\n7. Build\n8. Save\n9. Exit";
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
    case 2: // Add script //prtodo
      break;
    case 3: // Select script //prtodo
      break;
    case 4: // Transform //prtodo
      break;
  }
}


void HoldObject::Question(std::string const& line) {
  std::cout << "0. Delete object\n1. Set object model\n2. Add script\n3. Select script\n4. Transform";
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
    case 0: // Set mesh //prtodo
      break;
    case 1: // Set shader //prtodo
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





// HoldShader
// **************************************************
// **************************************************

void HoldShader::Prompt(int i, std::string const& line) {
  //prtodo
}


void HoldShader::Question(std::string const& line) {
  std::cout << "0. Select property";//prtodo
}

// **************************************************
// **************************************************
// HoldShader





// SelectObject
// **************************************************
// **************************************************

void SelectObject::Prompt(int i, std::string const& line) {
  assert(data);

  if (i == '\r' || i == '\n') {
    std::string str = (data->cycle && data->currentObject != data->pumpkin->registeredObjects.end()) ? pObjInt(data->currentObject->second)->name : line;
    if (str.size() == 0) { // Go back
      data->ResetAsk();
      return;
    }


    data->selectedObject = GetObject(str);
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

  std::string str = (data->cycle && data->currentObject != data->pumpkin->registeredObjects.end()) ? pObjInt(data->currentObject->second)->name : line;
  std::cout << "Enter object name to select or empty to go back\n>>" << str;
}


void SelectObject::Set() {
  assert(data);

  data->currentObject = data->pumpkin->registeredObjects.begin();
  data->forward = ObjectForward;
  data->back = ObjectBack;
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
    std::string str = (data->cycle && data->currentModel != data->pumpkin->registeredModels.end()) ? data->currentModel->second->name : line;
    if (str.size() == 0) { // Go back
      data->selectedModel = nullptr;
      data->ResetAsk();
      return;
    }

    data->selectedModel = GetModel(str);
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

  std::string str = (data->cycle && data->currentModel != data->pumpkin->registeredModels.end()) ? data->currentModel->second->name : line;
  std::cout << "Enter model name to select or empty to go back\n>>" << str;
}


void SelectModel::Set() {
  assert(data);

  data->currentModel = data->pumpkin->registeredModels.begin();
  data->forward = ModelForward;
  data->back = ModelBack;
}

// **************************************************
// **************************************************
// SelectModel





// SelectMesh
// **************************************************
// **************************************************

void SelectMesh::Prompt(int i, std::string const& line) {
  assert(data);
  // prtodo Fix this
  if (i == '\r' || i == '\n') {
    std::string str = (data->cycle && data->currentMesh != data->pumpkin->registeredMeshes.end()) ? data->currentMesh->second->name : line;
    if (str.size() == 0) { // Go back
      data->selectedModel = nullptr;
      data->ResetAsk();
      return;
    }

    data->selectedModel = GetModel(str);
    if (data->selectedModel == nullptr) {
      AddError("No mesh selected");
    }
    data->ResetAsk();
    return;
  }

  data->updateAsk = true;
}


void SelectMesh::Question(std::string const& line) {
  assert(data);

  if (data->display) {
    ListMeshes();
  }

  std::string str = (data->cycle && data->currentMesh != data->pumpkin->registeredMeshes.end()) ? data->currentMesh->second->name : line;
  std::cout << "Enter mesh name to select or empty to go back\n>>" << str;
}


void SelectMesh::Set() {
  assert(data);

  data->currentMesh = data->pumpkin->registeredMeshes.begin();
  data->forward = MeshForward;
  data->back = MeshBack;
}

// **************************************************
// **************************************************
// SelectMesh





// SelectShader
// **************************************************
// **************************************************

void SelectShader::Prompt(int i, std::string const& line) {
  assert(data);
  // prtodo Fix this
  if (i == '\r' || i == '\n') {
    std::string str = (data->cycle && data->currentShader != data->pumpkin->registeredShaders.end()) ? data->currentShader->second->name : line;
    if (str.size() == 0) { // Go back
      data->selectedModel = nullptr;
      data->ResetAsk();
      return;
    }

    data->selectedModel = GetModel(str);
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
    ListShaders();
  }

  std::string str = (data->cycle && data->currentShader != data->pumpkin->registeredShaders.end()) ? data->currentShader->second->name : line;
  std::cout << "Enter model name to select or empty to go back\n>>" << line;
}


void SelectShader::Set() {
  assert(data);

  data->currentShader = data->pumpkin->registeredShaders.begin();
  data->forward = ShaderForward;
  data->back = ShaderBack;
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
      if (builtString.size() != 0) {
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
        if (!builtString.empty()) builtString.pop_back();
        data->updateAsk = true;
        return;
      }

      if (builtString.size() != 0) {
        if (data->holdingProperty->type == VariableType::MAT4) {
          *((float*)data->holdingProperty->prop + (currentPos % 4) * 4 + (currentPos / 4)) = std::strtof(builtString.c_str(), nullptr);
        }
        else if (intType) *((int32_t*)data->holdingProperty->prop + currentPos) = (int32_t)std::strtol(builtString.c_str(), nullptr, 10);
        else *((float*)data->holdingProperty->prop + currentPos) = std::strtof(builtString.c_str(), nullptr);
        builtString.clear();
      }

      if (i == 'w' && currentPos >= 4) currentPos -= 4;
      else if (i == 's' && currentPos < maxPos - 4) currentPos += 4;
      else if (i == 'a' && currentPos > 0) currentPos--;
      else if (i == 'd' && currentPos < maxPos - 1) currentPos++;
      else return;

      data->updateAsk = true;
      return;
    }


    if (intType) {
      if (!std::isdigit(i) && i != '-' && i != '+') return;
    } else if (!std::isdigit(i) && i != 'e' && i != '-' && i != '+' && i != '.' && i != 'f') return;

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

      data->holdingPropertyHolder->DeleteProperty(data->holdingProperty->name);
      //data->DeleteBack();
      data->SetAsk(&data->propertyChanger, false);
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

    if (data->holdingProperty->type == VariableType::MAT4) {
      MatrixWrapper* wrap = (MatrixWrapper*)data->holdingProperty->prop;
      for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
          std::cout << "[ ";
          if (j * 4 + i == currentPos) {
            std::cout << "$ ";
            if (builtString.size() != 0) std::cout << builtString;
            else std::cout << *((float*)&wrap->cols[i] + j);
            std::cout << " $";
          } else {
            std::cout << *((float*)&wrap->cols[i] + j);
          }

          std::cout << " ]";
        }
        if (j != 3) std::cout << "\n";
      }


    } else {
      for (char i = 0; i < maxPos; i++) {
        std::cout << "[ ";

        if (i == currentPos) std::cout << "$ ";

        if (i != currentPos || builtString.size() == 0) {
          if (intType) std::cout << *(in + i);
          else std::cout << *(fl + i);
        } else {
          std::cout << builtString;
        }

        if (i == currentPos) std::cout << " $";

        std::cout << " ]";
      }
    }
    return;
  }

  std::cout << "0. Change\n1. Delete";
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
    std::string str = (data->cycle && data->currentProperty != data->holdingPropertyHolder->properties.end()) ? data->currentProperty->second.name : line;
    if (str.size() == 0) { // Go back
      data->SetAsk(&data->propertyChanger, false);
      return;
    }

    auto find = data->holdingPropertyHolder->properties.find(_STRING_HASHER(str));
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

  std::string str = (data->cycle && data->currentProperty != data->holdingPropertyHolder->properties.end()) ? data->currentProperty->second.name : line;
  std::cout << "Enter name of property to select or empty to go back\n>>" << str;
}


void SelectProperty::Set() {
  assert(data);

  data->currentProperty = data->holdingPropertyHolder->properties.begin();
  data->forward = PropertyForward;
  data->back = PropertyBack;
}

// **************************************************
// **************************************************
// SelectProperty





// CreateProperty
// **************************************************
// **************************************************

void CreateProperty::Prompt(int i, std::string const& line) {
  //prtodo
}


void CreateProperty::Question(std::string const& line) {
  //prtodo
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

  std::cout << "0. Select property\n1. Create property";
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

bool StringSort(std::string const& a, std::string const& b) {
  int sumA = 0;
  int sumB = 0;
  int low = a.size() < b.size() ? a.size() : b.size();
  bool alphabet = false;
  for (int i = 0; i < low; i++) {
    if (std::isalpha(a[i]) || std::isalpha(b[i])) alphabet = true;

    if (alphabet) {
      if (a[i] > b[i]) {
        sumA++;
        break;
      }

      if (b[i] > a[i]) {
        sumB++;
        break;
      }
    } else {
      sumA += a[i];
      sumB += b[i];
    }
  }

  return sumB > sumA;
}



void ListObjects() {
  std::vector<std::string> list;
  for (auto objP : data->pumpkin->registeredObjects) {
    list.push_back(Object_GetName(objP.second));
  }
  std::sort(list.begin(), list.end(), StringSort);
  for (auto& elem : list) std::cout << elem << "\n\n";
}


void ListMeshes() {
  std::vector<std::string> list;
  for (auto t : data->pumpkin->registeredMeshes) {
    list.push_back(t.second->name);
  }
  std::sort(list.begin(), list.end(), StringSort);
  for (auto& elem : list) std::cout << elem << "\n\n";
}


void ListModels() {
  std::vector<std::string> list;
  for (auto t : data->pumpkin->registeredModels) {
    list.push_back(t.second->name);
  }
  std::sort(list.begin(), list.end(), StringSort);
  for (auto& elem : list) std::cout << elem << "\n\n";
}


void ListShaders() {
  std::vector<std::string> list;
  for (auto t : data->pumpkin->registeredShaders) {
    list.push_back(t.second->name);
  }
  std::sort(list.begin(), list.end(), StringSort);
  for (auto& elem : list) std::cout << elem << "\n\n";
}



void AddError(std::string const& msg) {
  assert(data);
  data->messages.push_back(msg + "\n\n");
}





/*************************************************************************/
/*************************************************************************/
/*                                                                       */
/*                    S E L E C T I N G   T H I N G S                    */
/*                                                                       */
/*************************************************************************/
/*************************************************************************/



void ModelForward() {
  assert(data);

  if (data->currentModel != --data->pumpkin->registeredModels.end()) data->currentModel++;
  else data->currentModel = data->pumpkin->registeredModels.begin();
}

void ModelBack() {
  assert(data);

  if (data->currentModel != data->pumpkin->registeredModels.begin()) data->currentModel--;
  else data->currentModel = --data->pumpkin->registeredModels.end();
}



void MeshForward() {
  assert(data);

  if (data->currentMesh != --data->pumpkin->registeredMeshes.end()) data->currentMesh++;
  else data->currentMesh = data->pumpkin->registeredMeshes.begin();
}

void MeshBack() {
  assert(data);

  if (data->currentMesh != data->pumpkin->registeredMeshes.begin()) data->currentMesh--;
  else data->currentMesh = --data->pumpkin->registeredMeshes.end();
}



void ShaderForward() {
  assert(data);

  if (data->currentShader != --data->pumpkin->registeredShaders.end()) data->currentShader++;
  else data->currentShader = data->pumpkin->registeredShaders.begin();
}

void ShaderBack() {
  assert(data);

  if (data->currentShader != data->pumpkin->registeredShaders.begin()) data->currentShader--;
  else data->currentShader = --data->pumpkin->registeredShaders.end();
}



void ObjectForward() {
  assert(data);

  if (data->currentObject != --data->pumpkin->registeredObjects.end()) data->currentObject++;
  else data->currentObject = data->pumpkin->registeredObjects.begin();
}

void ObjectBack() {
  assert(data);

  if (data->currentObject != data->pumpkin->registeredObjects.begin()) data->currentObject--;
  else data->currentObject = --data->pumpkin->registeredObjects.end();
}



void PropertyForward() {
  assert(data);

  if (data->currentProperty != --data->holdingPropertyHolder->properties.end()) data->currentProperty++;
  else data->currentProperty = data->holdingPropertyHolder->properties.begin();
}

void PropertyBack() {
  assert(data);

  if (data->currentProperty != data->holdingPropertyHolder->properties.begin()) data->currentProperty--;
  else data->currentProperty = --data->holdingPropertyHolder->properties.end();
}



}; // namespace


#endif