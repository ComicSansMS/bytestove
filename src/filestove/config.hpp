#ifndef FILESTOVE_INCLUDE_GUARD_CONFIG_HPP_
#define FILESTOVE_INCLUDE_GUARD_CONFIG_HPP_

#include <chrono>
#include <filesystem>
#include <vector>

namespace filestove {

inline namespace v0100 {
struct Config {
    bool doLogging;
    std::vector<std::filesystem::path> directories;
    std::chrono::milliseconds scanInterval;
    std::size_t readIdleThreshold;
    std::size_t readBufferSize;
    std::size_t fileCollectChunkSize;

    Config();
};
}

Config readConfig();
Config readConfig(std::filesystem::path const& filename);

void writeConfig(Config const& config);
void writeConfig(Config const& config, std::filesystem::path const& filename);

}

#endif
