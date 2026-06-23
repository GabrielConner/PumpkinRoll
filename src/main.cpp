#include "pumpkin/pumpkinRoll.h"

#include <iostream>
#include <Windows.h>
#undef ERROR

using namespace ::pumpkin;


struct BounceScript : Script {
  static float acc;
  
  float vel = 0;

  virtual void Update(Object* obj, double deltaTime, double totalTime) {
    vel += acc * deltaTime;
    if (obj->transform.position.y < -3.f) {
      obj->transform.position.y = -3;
      vel = -vel;
    }
    obj->transform.position.y += vel * deltaTime;
  }
};
ScriptAllocateFunction(BounceScript);

float BounceScript::acc = -9.8f;


int SceneBuild();

int main(int argv, char** argc) {
  StartReturn ret = Init(StartSettings{}, argv, argc, nullptr);

  if (ret == StartReturn::SUCCESS) {
    RegisterScript(AllocateBounceScript, BounceScript);

    SceneBuild();

    Update();
    auto runtime = GetRuntime();
    runtime->backgroundColor = {0.2f,0.3f,0.3f,0.1f};
  }


  if (ret != StartReturn::FAILURE) End();

  return 0;
}