#include <filestove/ui/cooker.hpp>

#include <filestove/activity_monitor.hpp>
#include <filestove/file_collector.hpp>
#include <filestove/stove.hpp>

#include <gbBase/Log.hpp>

#pragma warning(push)
#pragma warning(disable: 5054)
#include <QMessageBox>
#pragma warning(pop)

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

    try {
        writeConfig(m_config);
    } catch (std::exception& e) {
        QMessageBox msgbox;
        msgbox.setText("Error while saving config file");
        msgbox.setDetailedText(e.what());
        msgbox.exec();
    }
}

void Cooker::run()
{
    filestove::ActivityMonitor monitor;
    filestove::FileCollector collector(m_config.directories);
    std::unique_lock lk(m_mtx);
    bool done = false;
    bool is_waiting = true;
    std::size_t read_overall = 0;
    std::size_t read_this_interval = 0;
    std::optional<filestove::Stove> stove;
    std::chrono::steady_clock::time_point t0;
    for (;;) {
        if (is_waiting) {
            emit startWaiting();
            if (m_cv.wait_for(lk, m_config.scanInterval, [this]() { return m_quitRequested; })) {
                return;
            }
            if (monitor.collect() < m_config.readIdleThreshold) {
                is_waiting = false;
                t0 = std::chrono::steady_clock::now();
                emit startCooking();
            }
        } else {
            bool timeout = false;
            if (!stove) {
                if (done) { break; }
                if (!collector.collect(m_config.fileCollectChunkSize)) { done = true; }
                stove = filestove::Stove{ collector.extractCollectedFiles(), m_config.readBufferSize };
            }
            while (stove->cook()) {
                auto const t1 = std::chrono::steady_clock::now();
                if (t1 - t0 > m_config.scanInterval) { timeout = true; break; }
            }
            read_this_interval += stove->readCount();
            stove->resetReadCount();
            if (timeout) {
                if (monitor.collect() > m_config.readIdleThreshold + read_this_interval) {
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
    emit cookingCompleted();
}

}

