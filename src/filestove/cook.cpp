#include <filestove/cook.hpp>

#include <filestove/stove.hpp>

#include <gbBase/Log.hpp>

namespace filestove {

void cook(std::filesystem::path const& path)
{
    std::vector<std::filesystem::path> files;
    auto const iterOpts = std::filesystem::directory_options::follow_directory_symlink;
    for (auto& e : std::filesystem::recursive_directory_iterator(path, iterOpts)) {\
        if (e.is_regular_file()) {
            files.emplace_back(e);
        }
    }
    GHULBUS_LOG(Info, "Collected " << files.size() << " files for cooking.");
    filestove::Stove stove(std::move(files), 4096);
    while (stove.cook()) {};

    GHULBUS_LOG(Info, "Read " << stove.readCount() << " bytes overall.");
}

}
