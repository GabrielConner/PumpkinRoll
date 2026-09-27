#ifndef PUMPKIN_ROLL_SRC_PUMPKIN_CONSTANTS_H
#define PUMPKIN_ROLL_SRC_PUMPKIN_CONSTANTS_H

#include <string>
#include "pPack/vector.h"
#include "pumpkin/types.h"

namespace pumpkin {

inline constexpr std::hash<std::string> _PR_STRING_HASHER = std::hash<std::string>();

inline constexpr double _PR_PI = 3.1415926535897932384626433832795;
inline constexpr double _PR_DEG_TO_RAD = _PR_PI / 180.0;
inline constexpr double _PR_RAD_TO_DEG = 180.0 / _PR_PI;

inline constexpr double _PR_DELTA = 1e-8;

inline constexpr ::pPack::Vector3 _PR_UP = {0, 1, 0};
inline constexpr ::pPack::DVector3 _PR_DUP = {0, 1, 0};

inline constexpr char _PR_BACKSPACE = 0x08;
inline constexpr char _PR_ESCAPE = 0x1B;
inline constexpr char _PR_SPACE = 0x20;
inline constexpr char _PR_HOME = 0x47;
inline constexpr char _PR_UP_ARROW = 0x48;
inline constexpr char _PR_PAGE_UP = 0x49;
inline constexpr char _PR_LEFT_ARROW = 0x4B;
inline constexpr char _PR_RIGHT_ARROW = 0x4D;
inline constexpr char _PR_PAGE_DOWN = 0x51;

inline constexpr double _PR_INFINITY = std::numeric_limits<double>::infinity();

inline constexpr char const* _DEV_SAVE_FILE = ".pmpknrl";

inline constexpr ::pumpkin::Interval _PR_INTERVAL_EMPTY = ::pumpkin::Interval(+_PR_INFINITY, -_PR_INFINITY);
inline constexpr ::pumpkin::Interval _PR_INTERVAL_UNIVERSE = ::pumpkin::Interval(-_PR_INFINITY, +_PR_INFINITY);

inline constexpr ::pumpkin::AABB _PR_AABB_EMPTY = ::pumpkin::AABB(_PR_INTERVAL_EMPTY, _PR_INTERVAL_EMPTY, _PR_INTERVAL_EMPTY);
inline constexpr ::pumpkin::AABB _PR_AABB_UNIVERSE = ::pumpkin::AABB(_PR_INTERVAL_UNIVERSE, _PR_INTERVAL_UNIVERSE, _PR_INTERVAL_UNIVERSE);

}; // namespace pumpkin

#endif