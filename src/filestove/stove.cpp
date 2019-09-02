#include <filestove/stove.hpp>

#include <gbBase/Assert.hpp>
#include <gbBase/Log.hpp>

#include <Windows.h>

#include <algorithm>

namespace filestove {

class FileHandle {
private:
    HANDLE m_handle;
public:
    FileHandle() noexcept
        :m_handle(INVALID_HANDLE_VALUE)
    {}

    FileHandle(HANDLE h) noexcept
        :m_handle(h)
    {}

    ~FileHandle()
    {
        if (m_handle != INVALID_HANDLE_VALUE) { CloseHandle(m_handle); }
    }

    FileHandle(FileHandle const&) = delete;
    FileHandle& operator=(FileHandle const&) = delete;

    FileHandle(FileHandle&& rhs) noexcept
        :FileHandle()
    {
        std::swap(m_handle, rhs.m_handle);
    }

    FileHandle& operator=(FileHandle&& rhs)
    {
        std::swap(m_handle, rhs.m_handle);
        rhs.reset();
        return *this;
    }

    void reset()
    {
        if (m_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
        }
    }

    void reset(HANDLE h)
    {
        if (m_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_handle);
        }
        m_handle = h;
    }

    HANDLE get() const noexcept
    {
        return m_handle;
    }

    operator bool() const {
        return m_handle != INVALID_HANDLE_VALUE;
    }
};

struct Stove::Pimpl {
    std::vector<std::filesystem::path> m_files;
    struct CurrentFileInfo {
        std::vector<std::filesystem::path>::const_iterator it;
        FileHandle hfile;
        std::size_t size;
        std::size_t bytes_read;

        CurrentFileInfo()
            :it{}, size(0), bytes_read(0)
        {}
    } m_currentFile;
    std::size_t m_readCount = 0;
    DWORD m_bufferSize;
    std::vector<std::byte> m_buffer;
    bool m_isDone;

    bool cook();
    void openFile();
};

Stove::Stove(std::vector<std::filesystem::path> files_to_cook, std::int32_t buffer_size)
    :m_pimpl(std::make_unique<Pimpl>())
{
    m_pimpl->m_files = std::move(files_to_cook);
    m_pimpl->m_currentFile.it = begin(m_pimpl->m_files);
    m_pimpl->m_bufferSize = buffer_size;
    m_pimpl->m_buffer.resize(buffer_size);
    m_pimpl->m_isDone = false;
}

Stove::~Stove() = default;
Stove::Stove(Stove&&) = default;
Stove& Stove::operator=(Stove&&) = default;


bool Stove::cook()
{
    return m_pimpl->cook();
}

bool Stove::Pimpl::cook()
{
    if (!m_currentFile.hfile) {
        if (m_currentFile.it == end(m_files)) { m_isDone = true; return false; }
        openFile();
        if (!m_currentFile.hfile) { m_isDone = true; return false; }
    }
    GHULBUS_ASSERT(m_bufferSize == m_buffer.size());

    DWORD bytes_read;
    DWORD const to_read = static_cast<DWORD>(
        std::min(static_cast<size_t>(m_bufferSize), m_currentFile.size - m_currentFile.bytes_read));
    if (ReadFile(m_currentFile.hfile.get(), m_buffer.data(), to_read, &bytes_read, nullptr) != TRUE) {
        GHULBUS_LOG(Warning, "Unable to read file " << m_currentFile.it->generic_string() <<
                             "; Error was: " << GetLastError());
        m_currentFile.hfile.reset();
    } else {
        m_currentFile.bytes_read += bytes_read;
        m_readCount += bytes_read;
        if (m_currentFile.bytes_read == m_currentFile.size) {
            m_currentFile.hfile.reset();
            ++m_currentFile.it;
        }
    }

    return true;
}

void Stove::Pimpl::openFile()
{
    for (; m_currentFile.it != end(m_files); ++m_currentFile.it) {
        m_currentFile.hfile.reset(
            CreateFileW(m_currentFile.it->native().c_str(),
                GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
        if (!m_currentFile.hfile) {
            GHULBUS_LOG(Warning, "Unable to open file " << m_currentFile.it->generic_string());
        } else {
            m_currentFile.bytes_read = 0;
            LARGE_INTEGER s;
            if (GetFileSizeEx(m_currentFile.hfile.get(), &s) == 0) {
                GHULBUS_LOG(Warning, "Unable to determine size for file " <<
                                     m_currentFile.it->generic_string() <<
                                     "; Error was: " << GetLastError());
            } else {
                m_currentFile.size = s.QuadPart;
                GHULBUS_LOG(Info, "Opened " << m_currentFile.it->generic_string() <<
                                  " for reading (" << m_currentFile.size <<  " bytes).");
                return;
            }
        }
    }
    m_currentFile.hfile.reset();
}

void Stove::resetReadCount() noexcept
{
    m_pimpl->m_readCount = 0;
}

std::size_t Stove::readCount() const noexcept
{
    return m_pimpl->m_readCount;
}

bool Stove::isDone() const noexcept
{
    return m_pimpl->m_isDone;
}

}
