#include <filestove/file_collector.hpp>

#include <gbBase/Log.hpp>

#include <numeric>

namespace filestove {

FileCollector::FileCollector(std::vector<std::filesystem::path> paths)
    : m_paths(std::move(paths)), m_pathIt(begin(m_paths))
{}

bool FileCollector::collect(std::int32_t collect_limit)
{
    std::int32_t collect_count = 0;
    for (; m_pathIt != end(m_paths); ++m_pathIt) {
        if (collect_count >= collect_limit) { return true; }
        if (m_fileIt == std::filesystem::recursive_directory_iterator{}) {
            if (std::filesystem::is_directory(*m_pathIt)) {
                auto const iterOpts = std::filesystem::directory_options::follow_directory_symlink;
                m_fileIt = std::filesystem::recursive_directory_iterator(*m_pathIt, iterOpts);
            } else if (std::filesystem::is_regular_file(*m_pathIt)) {
                auto const fsize = std::filesystem::file_size(*m_pathIt);
                m_files.emplace_back(*m_pathIt, fsize);
                ++collect_count;
                continue;
            } else {
                GHULBUS_LOG(Warning, "Unrecognized file type " << m_pathIt->generic_string());
                continue;
            }
        }
        for (; m_fileIt != std::filesystem::recursive_directory_iterator{}; ++m_fileIt) {
            if (collect_count >= collect_limit) { return true; }
            if (m_fileIt->is_regular_file()) {
                auto const fsize = std::filesystem::file_size(*m_fileIt);
                m_files.emplace_back(*m_fileIt, fsize);
                ++collect_count;
            }
        }
    }
    GHULBUS_LOG(Info, "Collected " << m_files.size() << " files for cooking.");
    return false;
}

std::vector<FileInfo> FileCollector::extractCollectedFiles()
{
    auto ret = std::move(m_files);
    m_files.clear();
    return std::move(ret);
}

std::uintmax_t FileCollector::currentFileCount() const
{
    return m_files.size();
}

std::uintmax_t FileCollector::currentSizeCount() const
{
    return std::accumulate(cbegin(m_files), cend(m_files), std::uintmax_t(0),
                           [](std::uintmax_t acc, FileInfo const& i) { return acc + i.size; });
}

}
