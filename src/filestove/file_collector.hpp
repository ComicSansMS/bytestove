#ifndef FILESTOVE_INCLUDE_GUARD_FILE_COLLECTOR_HPP_
#define FILESTOVE_INCLUDE_GUARD_FILE_COLLECTOR_HPP_

#include <filesystem>
#include <vector>

namespace filestove {

class FileCollector {
private:
    std::vector<std::filesystem::path> m_paths;
    std::vector<std::filesystem::path> m_files;
    std::vector<std::filesystem::path>::iterator m_pathIt;
    std::filesystem::recursive_directory_iterator m_fileIt;
public:
    FileCollector(std::vector<std::filesystem::path> paths);

    bool collect(int collect_limit);

    std::vector<std::filesystem::path> extractCollectedFiles();
};

}

#endif