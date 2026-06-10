#include "pumpkin/pumpkinRoll.h"

#include <iostream>
#include <Windows.h>
#undef ERROR

using namespace ::pumpkin;


int main() {
  StartReturn ret = Init(StartSettings{});



  if (ret == StartReturn::SUCCESS) {
    Update();
    auto runtime = GetRuntime();
    runtime->backgroundColor = {0.2f,0.3f,0.3f,0.1f};
  }



  if (ret != StartReturn::FAILURE) End();

  return 0;
}