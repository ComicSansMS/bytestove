#include <filestove/ui/cooker.hpp>

#include <filestove/activity_monitor.hpp>
#include <filestove/file_collector.hpp>
#include <filestove/stove.hpp>

#include <gbBase/Log.hpp>

#include <chrono>
#include <optional>

namespace filestove::ui {

Cooker::Cooker(Config const& config)
    :QObject(), m_config(config), m_quitRequested(false)
{
    m_thread = std::thread([this]() { run(); });
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

void Cooker::onConfigUpdate()
{

}

void Cooker::run()
{
    Config config = [this]() {
        std::lock_guard lk(m_mtx);
        return m_config;
    }();
    filestove::ActivityMonitor monitor;
    filestove::FileCollector collector(config.directories);
    bool is_waiting = true;
    std::uintmax_t read_overall = 0;
    std::uintmax_t read_this_interval = 0;
    std::optional<filestove::Stove> stove;
    std::chrono::steady_clock::time_point t0;
    while (!stove || !stove->isDone()) {
        if (is_waiting) {
            emit startWaiting();
            {
                std::unique_lock lk(m_mtx);
                if (m_cv.wait_for(lk, config.scanInterval, [this]() { return m_quitRequested; })) {
                    return;
                }
            }
            if (monitor.collect() < config.readIdleThreshold) {
                is_waiting = false;
                t0 = std::chrono::steady_clock::now();
                emit startCooking();
            }
        } else {
            bool timeout = false;
            if (!stove) {
                if (!collector.collect(config.fileCollectChunkSize)) {
                    auto const n_files = collector.currentFileCount();
                    auto const total_size = collector.currentSizeCount();
                    auto files = collector.extractCollectedFiles();
                    stove = filestove::Stove{ files, config.readBufferSize };
                    {
                        std::lock_guard lk(m_mtx);
                        m_files = std::move(files);
                    }
                    emit collectCompleted(n_files, total_size);
                } else {
                    emit collectUpdate(collector.currentFileCount(), collector.currentSizeCount());
                }
            } else {
                while (stove->cook()) {
                    auto const t1 = std::chrono::steady_clock::now();
                    if (t1 - t0 > config.scanInterval) { timeout = true; break; }
                    emit cookingUpdate(read_overall + stove->readCount());
                }
                read_this_interval += stove->readCount();
                stove->resetReadCount();
                if (timeout) {
                    if (monitor.collect() > config.readIdleThreshold + read_this_interval) {
                        is_waiting = true;
                    }
                    read_overall += read_this_interval;
                    read_this_interval = 0;
                    t0 = std::chrono::steady_clock::now();
                }
            }
            std::lock_guard lk(m_mtx);
            if (m_quitRequested) { return; }
        }
    }
    GHULBUS_LOG(Info, "Done cooking. Read " << read_overall + read_this_interval << " bytes in total.");
    emit cookingCompleted();
}

}

