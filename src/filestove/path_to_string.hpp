#ifndef FILESTOVE_INCLUDE_GUARD_PATH_TO_STRING_HPP_
#define FILESTOVE_INCLUDE_GUARD_PATH_TO_STRING_HPP_

#include <filesystem>
#include <string>

namespace filestove {

inline std::string path_to_string(std::filesystem::path const& p) {
    // dirty workaround for the fact that path to std::string conversion throws an exception on msvc
    return std::string{ reinterpret_cast<char const*>(p.generic_u8string().c_str()) };
}

}

#endif
