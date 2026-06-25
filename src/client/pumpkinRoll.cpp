/* 

NOTE
DON'T INCLUDE IN PROD BUILD

*/

#include "pumpkinFunctions.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace {

void* LoadProcAddress(char const* addr);
HMODULE libraryHandle;

}


namespace pumpkin {

bool Load(int argv, char** argc) {

#if _DEBUG
  if (argv > 1 && strcmp(argc[1], "-d") == 0) {
    libraryHandle = LoadLibraryA("pumpkinRollDev-d.dll");
  } else {
    libraryHandle = LoadLibraryA("pumpkinRoll-d.dll");
  }
#else
  if (argv > 1 && strcmp(argc[1], "-d") == 0) {
    libraryHandle = LoadLibraryA("pumpkinRollDev.dll");
  } else {
    libraryHandle = LoadLibraryA("pumpkinRoll.dll");
  }
#endif

  if (libraryHandle == NULL) {
    return false;
  }

  bool ret = LoadFunctions(LoadProcAddress);

  return ret;
}

void End() {
  FreeLibrary(libraryHandle);
  libraryHandle = NULL;
}

}; // namespace pumpkin



namespace {

void* LoadProcAddress(char const* addr) {
  return GetProcAddress(libraryHandle, addr);
}

}; // namespace