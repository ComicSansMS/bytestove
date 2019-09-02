#ifndef FILESTOVE_INCLUDE_GUARD_STOVE_HPP_
#define FILESTOVE_INCLUDE_GUARD_STOVE_HPP_

#include <filestove/file_info.hpp>

#include <filesystem>
#include <memory>
#include <vector>

namespace filestove {

class Stove {
    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;
public:
    Stove(std::vector<FileInfo> files_to_cook, std::int32_t buffer_size);
    ~Stove();

    Stove(Stove&&);
    Stove& operator=(Stove&&);

    Stove(Stove const&) = delete;
    Stove& operator=(Stove const&) = delete;

    bool cook();

    void resetReadCount() noexcept;
    std::size_t readCount() const noexcept;

    bool isDone() const noexcept;
};

}

#endif
