#include "pumpkin/pumpkinRoll.h"

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
float BounceScript::acc = -9.8f;
ScriptAllocateFunction(BounceScript);




int SceneBuild();
int main(int argv, char** argc) {
  
  if (!Load(argv, argc)) return 1; // Load DLL

  StartReturn ret = Pumpkin_Init(StartSettings(), argv, argc, nullptr); // Starts up and loads stuff
  if (ret == StartReturn::FAILURE) { // Failure means to not call Pumpkin_End, End still needs to be called
    End(); // Close DLL
    return 1;
  }
  if (ret != StartReturn::ERROR) { // Error means failed to fully start but Pumpkin_End does need to be called
    RegisterScript(AllocateBounceScript, BounceScript, sizeof(BounceScript));

    // Uncomment after dev scene is built
    //SceneBuild();
    Pumpkin_Update();
  }

  Pumpkin_End(); // Free memory

  End(); // Close DLL

  return 0;
}