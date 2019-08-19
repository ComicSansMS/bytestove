#include <filestove/cook.hpp>

#include <gbBase/Assert.hpp>
#include <gbBase/Finally.hpp>
#include <gbBase/Log.hpp>

#include <algorithm>
#include <codecvt>
#include <string>

#include <cstdio>

namespace filestove {

void cook_file(std::filesystem::path const& e)
{
    std::string u8_conv = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.to_bytes(e.native());
    GHULBUS_LOG(Trace, "Cooking " << u8_conv << ".");

    FILE* f = fopen(u8_conv.c_str(), "rb");
    GHULBUS_ASSERT(f);
    auto const guard_file_handle = Ghulbus::finally([f]() { fclose(f); });

    fseek(f, 0, SEEK_END);
    long const fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char buffer[4096];
    long bytes_read = 0;
    while (bytes_read < fsize) {
        long const to_read = std::min(fsize - bytes_read, 4096l);
        fread(buffer, 1, to_read, f);
        bytes_read += to_read;
    }
    GHULBUS_ASSERT(bytes_read == fsize);

}

void cook(std::filesystem::path const& path)
{
    auto const iterOpts = std::filesystem::directory_options::follow_directory_symlink;
    for (auto& e : std::filesystem::recursive_directory_iterator(path, iterOpts)) {\
        if (e.is_regular_file()) {
            cook_file(e.path());
        }
    }
}

}
