#ifndef FILESTOVE_INCLUDE_GUARD_VERSION_HPP_
#define FILESTOVE_INCLUDE_GUARD_VERSION_HPP_

#include <cstdint>

namespace filestove {

struct Version {
    std::uint8_t major;
    std::uint8_t minor;
    std::uint8_t patch;
};

inline constexpr Version version() {
    return Version{ 0, 9, 0 };
}

}

#endif
