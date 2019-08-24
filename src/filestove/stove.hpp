#ifndef FILESTOVE_INCLUDE_GUARD_STOVE_HPP_
#define FILESTOVE_INCLUDE_GUARD_STOVE_HPP_

#include <filesystem>
#include <memory>
#include <vector>

namespace filestove {

class Stove {
    struct Pimpl;
    std::unique_ptr<Pimpl> m_pimpl;
public:
    Stove(std::vector<std::filesystem::path> files_to_cook);
    ~Stove();

    Stove(Stove const&) = delete;
    Stove& operator=(Stove const&) = delete;

    bool cook();

    void resetReadCount() noexcept;
    std::size_t readCount() const noexcept;
};

}

#endif
