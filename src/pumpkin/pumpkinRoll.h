#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_PUMPKIN_ROLL_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_PUMPKIN_ROLL_H

#ifndef _WIN32 
#error Only available on windows
#endif


#ifdef PUMPKIN_ROLL_PROD
#include "private/functions.h"
#define DLL __declspec(dllexport)
#else
#include "pumpkin/pumpkinLoadingFunctions.h"
#define DLL __declspec(dllimport)
#endif
#include "pumpkin/constants.h"
#include "pumpkin/types.h"


namespace pumpkin {

bool Load(int argv, char** argc);
void End();


#define RegisterScript(scriptAllocate, script) Pumpkin_RegisterScriptRaw(scriptAllocate, typeid(script).name(), sizeof(script))
#define ScriptAllocateFunction(script) Script* Allocate ## script () { return new script ## (); }

#define ScriptName(script) typeid(script).name()



#ifndef PUMPKIN_ROLL_VERBOSE_PRINT

#define pDebug(msg) ::pumpkin::Pumpkin_PrintError(::pumpkin::PrintLevel::DEBUG, "", msg)
#define pWarn(msg) ::pumpkin::Pumpkin_PrintError(::pumpkin::PrintLevel::WARNING, "[WARNING] ", msg)
#define pError(msg) ::pumpkin::Pumpkin_PrintError(::pumpkin::PrintLevel::ERROR, "[ERROR] ", msg)

#else

#define STRINGYFY(x) # x
#define PASTE_AS_STRING(s) STRINGYFY(s)

#define pDebug(msg) ::pumpkin::Pumpkin_PrintError(::pumpkin::PrintLevel::DEBUG, "", msg)
#define pWarn(msg) ::pumpkin::Pumpkin_PrintError(::pumpkin::PrintLevel::WARNING, "[WARNING] " __FILE__ " -- " PASTE_AS_STRING(__LINE__), msg)
#define pError(msg) ::pumpkin::Pumpkin_PrintError(::pumpkin::PrintLevel::ERROR, "[ERROR] " __FILE__ " -- " PASTE_AS_STRING(__LINE__),  msg)

#endif

#ifdef PUMPKIN_ROLL_NO_CHECK

#define pNullCheck (void)0;
#define pCheckIf (void)0;
#define pCheckIfNot (void)0;

#else

#define pNullCheck(obj, ...)  \
if (obj == nullptr) {         \
  pError(#obj " is nullptr"); \
  return __VA_ARGS__;         \
}

#define pCheckIf(obj, val, ...)  \
if (obj == val) {                \
  pError(#obj " is invalid");    \
  return __VA_ARGS__;            \
}

#define pCheckIfNot(obj, val, ...)  \
if (obj != val) {                   \
  pError(#obj " is invalid");       \
  return __VA_ARGS__;               \
}

#endif

}; // namespace pumpkin

#endif