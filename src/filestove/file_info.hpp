#ifndef FILESTOVE_INCLUDE_GUARD_FILE_INFO_HPP_
#define FILESTOVE_INCLUDE_GUARD_FILE_INFO_HPP_

#include <filesystem>

namespace filestove {

struct FileInfo {
    std::filesystem::path path;
    std::uintmax_t size;
    FileInfo(std::filesystem::path p, std::uintmax_t s) : path(std::move(p)), size(s) {}
};

}

#endif
