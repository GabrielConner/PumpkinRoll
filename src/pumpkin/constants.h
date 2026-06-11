#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_CONSTANTS_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_CONSTANTS_H

#include <string>

namespace pumpkin {

inline constexpr std::hash<std::string> _STRING_HASHER = std::hash<std::string>();


namespace var_type {
inline constexpr uint32_t INT = 1;
inline constexpr uint32_t FLOAT = 2;
inline constexpr uint32_t MAT4 = 4;
inline constexpr uint32_t VECTOR2 = 8;
inline constexpr uint32_t VECTOR3 = 16;
inline constexpr uint32_t VECTOR4 = 32;
inline constexpr uint32_t CONTROL = 1 << 31;
}; // namespace var_type


inline constexpr char _BACKSPACE = 0x08;
inline constexpr char _ESCAPE = 0x1B;
inline constexpr char _UP_ARROW = 0x48;
inline constexpr char _LEFT_ARROW = 0x4B;
inline constexpr char _RIGHT_ARROW = 0x4D;


}; // namespace pumpkin

#endif