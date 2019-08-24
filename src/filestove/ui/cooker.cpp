#include <filestove/ui/cooker.hpp>

#include <filestove/activity_monitor.hpp>
#include <filestove/file_collector.hpp>
#include <filestove/stove.hpp>

#include <gbBase/Log.hpp>

#include <chrono>
#include <optional>

namespace filestove::ui {

Cooker::Cooker()
    :QObject(), m_quitRequested(false)
{
    m_thread = std::thread([this]() { run(); });
    m_paths.push_back("c:\\doc\\SFML-2.5.1");
    m_paths.push_back("c:\\cpp\\coroutines");
}

Cooker::~Cooker()
{
    requestQuit();
    m_thread.join();
}

void Cooker::requestQuit()
{
    std::lock_guard lk(m_mtx);
    m_quitRequested = true;
    m_cv.notify_all();
}

void Cooker::run()
{
    std::size_t constexpr threshold = 10 << 20;
    int constexpr file_collect_threshold = 1024;
    std::chrono::seconds constexpr interval = std::chrono::seconds{ 5 };
    filestove::ActivityMonitor monitor;
    filestove::FileCollector collector(std::move(m_paths));
    std::unique_lock lk(m_mtx);
    bool done = false;
    bool is_waiting = true;
    std::size_t read_overall = 0;
    std::size_t read_this_interval = 0;
    std::optional<filestove::Stove> stove;
    std::chrono::steady_clock::time_point t0;
    for (;;) {
        if (is_waiting) {
            if (m_cv.wait_for(lk, interval, [this]() { return m_quitRequested; })) {
                return;
            }
            if (monitor.collect() < threshold) {
                is_waiting = false;
                t0 = std::chrono::steady_clock::now();
            }
        } else {
            bool timeout = false;
            if (!stove) {
                if (done) { break; }
                if (!collector.collect(file_collect_threshold)) { done = true; }
                stove = filestove::Stove{ collector.extractCollectedFiles() };
            }
            while (stove->cook()) {
                auto const t1 = std::chrono::steady_clock::now();
                if (t1 - t0 > interval) { timeout = true; break; }
            }
            read_this_interval += stove->readCount();
            stove->resetReadCount();
            if (timeout) {
                if (monitor.collect() > threshold + read_this_interval) {
                    is_waiting = true;
                }
                read_overall += read_this_interval;
                read_this_interval = 0;
                t0 = std::chrono::steady_clock::now();
            } else {
                stove = std::nullopt;
            }
        }
    }
    GHULBUS_LOG(Info, "Done cooking. Read " << read_overall + read_this_interval << " bytes in total.");
}

}

