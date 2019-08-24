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
    std::size_t read_overall = 0;
    std::size_t files_overall = 0;
    std::size_t read_this_interval = 0;
    for (;;) {
        std::optional<filestove::Stove> stove;
        if (m_cv.wait_for(lk, interval, [this]() { return m_quitRequested; })) {
            return;
        }
        auto const t0 = std::chrono::steady_clock::now();
        bool timeout = false;
        while (monitor.collect() < threshold + read_this_interval) {
            if (!stove) {
                if (done) { return; }
                if (!collector.collect(file_collect_threshold)) { done = true; }
                stove = filestove::Stove{collector.extractCollectedFiles()};
            }
            while (stove->cook()) {
                auto const t1 = std::chrono::steady_clock::now();
                if (t1 - t0 > interval) { timeout = true; break; }
            }
            read_this_interval += stove->readCount();
            stove->resetReadCount();
            if (timeout) { break; }
            stove = std::nullopt;
        }
    }
}

void Cooker::collectFiles()
{
    filestove::FileCollector collector(std::move(m_paths));
    while (collector.collect(1)) ;
    m_paths = collector.extractCollectedFiles();
    return;
}


}

