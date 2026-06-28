# Pumpkin Pie

## Introduction

Pumpkin Pie is a basic framework for running a 3D single-scene project.  It can be switched into development which turns it into a console app for creating the scene, and managing basic settings.

While in the development mode the scene can be saved to a `*.pmpknrl` file for reloading into development.  The scene can be built into a `*.pmpknrl.cpp` file for compiling directly into the project, for creating the release version.

After creating the build file a function with the signature `int SceneBuild()` should be called right before `Pumpkin_Update()` is called.  This should be commented out and the `*.pmpknrl` development file should be used if anything needs to be updated then rebuilt. 

 While in debug mode the built file will contain new lines to help format, these are removed while in release.

#### The default controls for the development camera and the `PumpkinRoll__CameraFreeMovement` script are

|Control | Description |
|--------|------------|
|WASD|Basic forward/left type stff|
|E| Move up|
|Q| Move down|
|Right-click| Activate movement (Hold in dev, toggle in script) **Can be changed for the script**|

## System requirements

Pumpkin Pie is available Windows 7 and above [needs testing].  Can be ported to other systems as the main Windows requirement is the CRT memory checking and getting the location of the executable.

## Compiling

To setup the source the prod zipped files in [release](https://github.com/GabrielConner/PumpkinRoll/releases) can be used for all the visual studio and ppm files.  There is a bunch of settings that would make creating from new slow, and unless bored I won't be making a (C)MAKE file(s).

Compiled on Visual C++ (2026) but should be available for earlier.  Clang can also be used but does have some problems with the `ScriptAllocateFunction` macro.  Other compilers are untested.

Highly recommened to not self build but use prebuilt binaries found in [release](https://github.com/GabrielConner/PumpkinRoll/releases)

## Creating a project

The basic structure of a project should follow 

```c++
#include "pumpkin/pumpkinRoll.h"

using namespace ::pPack;
using namespace ::pumpkin;

int SceneBuild();
int main(int argc, char** argv) {
  if (!Load(argc, argv)) return 1;

  StartReturn ret = Pumpkin_Init(StartSettings(), argc, argv, nullptr);
  if (ret == StartReturn::FAILURE) {
    End();
    return 1;
  }

  if (ret != StartReturn::ERROR) {
    Pumpkin_RegisterCamera("primary camera"); 

    SceneBuild();
    Pumpkin_Update();
  }

  Pumpkin_End();
  End();
}
```


`Load` and `End` are for the pumpkin pie library dll, holds some common things that all the dll's use.

A camera should always be created as they cannot be created within the development console, though they can be set as the primary camera.

All available functions, past `Load`/`End`, are found in `pumpkinFunctions.h` and some virtual overridable functions in `pumpkin/types.h`

`pumpkin/pumpkinRoll.h` contains macros for registering scripts, a quick script allocate function, and error printing.
