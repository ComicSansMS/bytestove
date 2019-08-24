#include <filestove/activity_monitor.hpp>
#include <filestove/exceptions.hpp>

#include <gbBase/Assert.hpp>
#include <gbBase/Finally.hpp>
#include <gbBase/Log.hpp>
#include <gbBase/LogHandlers.hpp>

#include <algorithm>
#include <chrono>
#include <string_view>
#include <vector>

#include <Pdh.h>
#include <PdhMsg.h>

namespace {

std::vector<std::string_view> split(std::vector<char>& v)
{
    std::vector<std::string_view> ret;
    auto it = begin(v);
    while (it != end(v)) {
        auto const new_it = std::find(it, end(v), '\0');
        auto const string_len = std::distance(it, new_it);
        if (string_len == 0) { return ret; }
        ret.emplace_back(&(*it), string_len);
        it = new_it + 1;
    }
    return {};
}

}

namespace filestove {

struct Counter {
    std::string_view name;
    PDH_HCOUNTER handle;
};

struct ActivityMonitor::Pimpl {
    PDH_HQUERY hquery;
    PDH_HCOUNTER hcounter;
    std::vector<char> buffer_expanded_names;
    std::vector<Counter> counters;

    Pimpl();
    ~Pimpl();

    Pimpl(Pimpl const&) = delete;
    Pimpl& operator=(Pimpl const&) = delete;
    Pimpl(Pimpl&&) = delete;
    Pimpl& operator=(Pimpl&&) = delete;
};

ActivityMonitor::Pimpl::Pimpl()
    :hquery(nullptr), hcounter(nullptr)
{
    PDH_STATUS res = PdhOpenQueryA(nullptr, DWORD_PTR{ 0 }, &hquery);
    if (res != ERROR_SUCCESS) {
        GHULBUS_THROW(decorate_exception(Win32Exception{}, Exception_Info::pdh_status(res)),
                      "Error opening pdh query.");
    }

    res = PdhAddEnglishCounter(hquery, "\\FileSystem Disk Activity(default)\\*", DWORD_PTR{ 0 }, &hcounter);
    if (res != ERROR_SUCCESS) {
        GHULBUS_THROW(Win32Exception{} << Exception_Info::pdh_status(res), "Error adding pdh counter.");
    }
    DWORD buffer_size = 0;
    res = PdhGetCounterInfo(hcounter, TRUE, &buffer_size, nullptr);
    std::vector<std::byte> buffer_counter_info;
    buffer_counter_info.resize(buffer_size);
    PDH_COUNTER_INFO_A* counter_info = reinterpret_cast<PPDH_COUNTER_INFO_A>(buffer_counter_info.data());
    res = PdhGetCounterInfo(hcounter, TRUE, &buffer_size, counter_info);
    if (res != ERROR_SUCCESS) {
        GHULBUS_THROW(Win32Exception{} << Exception_Info::pdh_status(res), "Error retrieving pdh counter info.");
    }
    buffer_size = 0;
    res = PdhExpandWildCardPathA(nullptr, counter_info->szFullPath, nullptr, &buffer_size, 0);
    buffer_expanded_names.resize(buffer_size);
    res = PdhExpandWildCardPathA(nullptr, counter_info->szFullPath,
                                 reinterpret_cast<PZZSTR>(buffer_expanded_names.data()), &buffer_size, 0);
    if (res != ERROR_SUCCESS) {
        GHULBUS_THROW(Win32Exception{} << Exception_Info::pdh_status(res), "Error expanding pdh wildcards.");
    }

    auto expanded_strings = split(buffer_expanded_names);
    counters.reserve(expanded_strings.size());
    for (auto const& p : expanded_strings) {
        counters.push_back({ p, nullptr });
        res = PdhAddCounterA(hquery, p.data(), DWORD_PTR{ 0 }, &counters.back().handle);
        if (res != ERROR_SUCCESS) {
            GHULBUS_THROW(Win32Exception{} << Exception_Info::pdh_status(res), "Error adding pdh counter.");
        }
    }

    res = PdhCollectQueryData(hquery);
    if (res != ERROR_SUCCESS) {
        GHULBUS_THROW(Win32Exception{} << Exception_Info::pdh_status(res), "Error collecting pdh query data.");
    }
}

ActivityMonitor::Pimpl::~Pimpl()
{
    PdhCloseQuery(hquery);
}


ActivityMonitor::ActivityMonitor()
    :m_pimpl(std::make_unique<Pimpl>())
{
}

ActivityMonitor::~ActivityMonitor() = default;

std::size_t ActivityMonitor::collect()
{
    std::size_t ret = 0;
    PDH_STATUS res = PdhCollectQueryData(m_pimpl->hquery);
    if (res != ERROR_SUCCESS) {
        GHULBUS_THROW(Win32Exception{} << Exception_Info::pdh_status(res), "Error collecting pdh query data.");
    }
    for (auto const& c : m_pimpl->counters) {
        PDH_FMT_COUNTERVALUE formatted_value;
        res = PdhGetFormattedCounterValue(c.handle, PDH_FMT_LARGE, nullptr, &formatted_value);
        if (res != ERROR_SUCCESS) {
            GHULBUS_THROW(Win32Exception{} << Exception_Info::pdh_status(res), "Error formatting pdh counter data.");
        } else {
            GHULBUS_LOG(Info, c.name << ": " << formatted_value.largeValue);
            GHULBUS_ASSERT(formatted_value.largeValue >= 0);
            ret += static_cast<std::size_t>(formatted_value.largeValue);
        }
    }
    return ret;
}

}
