#include <filestove/config.hpp>

#include <gbBase/Exception.hpp>
#include <gbBase/Log.hpp>

#include <algorithm>
#include <fstream>
#include <limits>
#include <string>

namespace {
char const* g_configFilename = "filestove.cfg";
char const g_magicNumber[] = { 'F', 'S', 'T', 'V', '1' };
char const g_versionTag_0100[] = { 0, 1, 0, 0 };
}

namespace filestove {

inline namespace v0100 {

Config::Config()
    :doLogging(false), directories{}, scanInterval(std::chrono::seconds(5)),
    readIdleThreshold(10 << 20), readBufferSize(4096), fileCollectChunkSize(1024)
{}

}

Config readConfig()
{
    return readConfig(g_configFilename);
}

Config readConfig(std::filesystem::path const& filename)
{
    if (!std::filesystem::exists(filename)) {
        return {};
    }
    std::ifstream fin(filename, std::ios_base::binary);
    if (!fin) {
        GHULBUS_LOG(Error, "Unable to open config file " << filename << " for reading.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Unable to open config file for reading.");
    }

    char magic[5];
    auto check_magic_number = [](char const (&magic)[5]) -> bool {
        return std::equal(std::begin(g_magicNumber), std::end(g_magicNumber), std::begin(magic));
    };
    fin.read(magic, 5);
    if (!fin || check_magic_number(magic)) {
        GHULBUS_LOG(Error, "Invalid magic number sequence.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error reading config file.");
    }

    char version[4] = {};
    auto const compare_version = [](char const (&ref_version)[4], char const (&version)[4]) -> bool {
        return std::equal(std::begin(ref_version), std::end(ref_version), std::begin(version));
    };
    fin.read(version, 4);
    if (!fin || !compare_version(g_versionTag_0100, version)) {
        GHULBUS_LOG(Error, "Unknown config file version.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error reading config file.");
    }

    Config ret;

    char do_logging;
    fin.read(&do_logging, 1);
    if (!fin || ((do_logging != 0) && (do_logging != 1))) {
        GHULBUS_LOG(Error, "Invalid entry for doLogging.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error reading config file.");
    }
    ret.doLogging = (do_logging == 1) ? true : false;

    std::uint32_t n_directories;
    fin.read(reinterpret_cast<char*>(&n_directories), 4);
    if (!fin) {
        GHULBUS_LOG(Error, "Invalid path index length.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error reading config file.");
    }
    for (std::uint32_t i = 0; i < n_directories; ++i) {
        std::uint32_t slen;
        fin.read(reinterpret_cast<char*>(&slen), 4);
        if (!fin) {
            GHULBUS_LOG(Error, "Invalid path entry #" << i << ".");
            GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error reading config file.");
        }
        std::vector<char> buffer;
        buffer.resize(slen);
        fin.read(buffer.data(), buffer.size());
        if (!fin || !buffer.back() == '\0') {
            GHULBUS_LOG(Error, "Invalid path entry.");
            GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error reading config file.");
        }
        ret.directories.emplace_back(buffer.data());
    }

    std::uint32_t scan_interval;
    fin.read(reinterpret_cast<char*>(&scan_interval), 4);
    if (!fin) {
        GHULBUS_LOG(Error, "Invalid scan interval.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error reading config file.");
    }
    ret.scanInterval = std::chrono::milliseconds{scan_interval};

    std::uint64_t read_idle_threshold;
    fin.read(reinterpret_cast<char*>(&read_idle_threshold), 8);
    if (!fin) {
        GHULBUS_LOG(Error, "Invalid read idle threshold.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error reading config file.");
    }
    ret.readIdleThreshold = read_idle_threshold;

    std::uint64_t read_buffer_size;
    fin.read(reinterpret_cast<char*>(&read_buffer_size), 8);
    if (!fin) {
        GHULBUS_LOG(Error, "Invalid read buffer size.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error reading config file.");
    }
    ret.readBufferSize = read_buffer_size;

    std::uint64_t file_collect_chunk_size;
    fin.read(reinterpret_cast<char*>(&file_collect_chunk_size), 8);
    if (!fin) {
        GHULBUS_LOG(Error, "Invalid file collect chunk size.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error reading config file.");
    }
    ret.fileCollectChunkSize = file_collect_chunk_size;

    return ret;
}

void writeConfig(Config const& config)
{
    auto const out_file = std::filesystem::path(g_configFilename).concat(".~new");
    writeConfig(config, out_file);

    std::error_code ec;
    std::filesystem::rename(out_file, g_configFilename, ec);
    if (ec) {
        GHULBUS_LOG(Error, "Cannot establish config file: " << ec.message() <<
                           "; File has been saved to " << out_file);
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error writing config file.");
    }
}


void writeConfig(Config const& config, std::filesystem::path const& filename)
{
    std::ofstream fout(filename, std::ios_base::binary);
    if (!fout) {
        GHULBUS_LOG(Error, "Unable to open output config file " << filename << " for writing.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Unable to open config file for writing.");
    }

    fout.write(g_magicNumber, sizeof(g_magicNumber));
    fout.write(g_versionTag_0100, sizeof(g_versionTag_0100));

    char const do_logging = (config.doLogging ? 1 : 0);
    fout.write(&do_logging, 1);

    if (config.directories.size() > std::numeric_limits<std::uint32_t>::max()) {
        GHULBUS_LOG(Error, "Maximum number of directories exceeded.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error writing config file.");
    }
    std::uint32_t const n_directories = static_cast<std::uint32_t>(config.directories.size());
    fout.write(reinterpret_cast<char const*>(&n_directories), 4);
    for (auto const& d : config.directories) {
        std::string const str = d.generic_string();
        if (str.length() + 1 > std::numeric_limits<std::uint32_t>::max()) {
            GHULBUS_LOG(Error, "Invalid directory entry.");
            GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error writing config file.");
        }
        std::uint32_t const slen = static_cast<std::uint32_t>(str.length() + 1);
        fout.write(reinterpret_cast<char const*>(&slen), 4);
        fout.write(str.data(), slen);
    }

    if (config.scanInterval.count() > std::numeric_limits<std::uint32_t>::max()) {
        GHULBUS_LOG(Error, "Invalid scan interval.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error writing config file.");
    }
    std::uint32_t const scan_interval = static_cast<std::uint32_t>(config.scanInterval.count());
    fout.write(reinterpret_cast<char const*>(&scan_interval), 4);

    fout.write(reinterpret_cast<char const*>(&config.readIdleThreshold), 8);
    fout.write(reinterpret_cast<char const*>(&config.readBufferSize), 8);
    fout.write(reinterpret_cast<char const*>(&config.fileCollectChunkSize), 8);
    if (!fout) {
        GHULBUS_LOG(Error, "Config file write error.");
        GHULBUS_THROW(Ghulbus::Exceptions::IOError{}, "Error writing config file.");
    }
}

}
