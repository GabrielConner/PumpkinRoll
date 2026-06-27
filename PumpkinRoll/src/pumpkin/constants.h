#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_CONSTANTS_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_CONSTANTS_H

#include <string>
#include "pPack/vector.h"

namespace pumpkin {

inline constexpr std::hash<std::string> _STRING_HASHER = std::hash<std::string>();

inline constexpr double _PI = 3.1415926535897932384626433832795;
inline constexpr double _DEG_TO_RAD = _PI / 180.0;
inline constexpr double _RAD_TO_DEG = 180.0 / _PI;

inline constexpr ::pPack::Vector3 _UP = {0, 1, 0};

inline constexpr char _BACKSPACE = 0x08;
inline constexpr char _ESCAPE = 0x1B;
inline constexpr char _HOME = 0x47;
inline constexpr char _UP_ARROW = 0x48;
inline constexpr char _PAGE_UP = 0x49;
inline constexpr char _LEFT_ARROW = 0x4B;
inline constexpr char _RIGHT_ARROW = 0x4D;
inline constexpr char _PAGE_DOWN = 0x51;


inline constexpr char const* _DEV_SAVE_FILE = ".pmpknrl";


}; // namespace pumpkin

#endif