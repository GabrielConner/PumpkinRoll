#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_PUMPKIN_ROLL_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_PUMPKIN_ROLL_H

#ifndef _WIN32 
#error Only available on windows
#endif


#ifdef PUMPKIN_ROLL_PROD
#include "private/functions.h"
#else
#include "pumpkin/functions.h"
#endif
#include "pumpkin/constants.h"

namespace pumpkin {


#ifndef PUMPKIN_ROLL_VERBOSE_PRINT

#define pDebug(msg) ::pumpkin::PrintError(::pumpkin::PrintLevel::DEBUG, "", "", msg)
#define pWarn(msg) ::pumpkin::PrintError(::pumpkin::PrintLevel::WARNING, "[WARNING] " "", "", msg)
#define pError(msg) ::pumpkin::PrintError(::pumpkin::PrintLevel::ERROR, "[ERROR] " "", "", msg)

#else

#define pDebug(msg) ::pumpkin::PrintError(::pumpkin::PrintLevel::DEBUG, "", "", msg)
#define pWarn(msg) ::pumpkin::PrintError(::pumpkin::PrintLevel::WARNING, "[WARNING] " __FILE__, __LINE__, msg)
#define pError(msg) ::pumpkin::PrintError(::pumpkin::PrintLevel::ERROR, "[ERROR] " __FILE__, __LINE__, msg)

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