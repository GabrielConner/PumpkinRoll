#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_CONSTANTS_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_CONSTANTS_H

#include <string>

namespace pumpkin {

inline constexpr std::hash<std::string> _STRING_HASHER = std::hash<std::string>();


inline constexpr char _BACKSPACE = 0x08;
inline constexpr char _ESCAPE = 0x1B;
inline constexpr char _HOME = 0x47;
inline constexpr char _UP_ARROW = 0x48;
inline constexpr char _PAGE_UP = 0x49;
inline constexpr char _LEFT_ARROW = 0x4B;
inline constexpr char _RIGHT_ARROW = 0x4D;
inline constexpr char _PAGE_DOWN = 0x51;


}; // namespace pumpkin

#endif