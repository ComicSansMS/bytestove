#ifndef FILESTOVE_INCLUDE_GUARD_UI_COOKER_HPP_
#define FILESTOVE_INCLUDE_GUARD_UI_COOKER_HPP_

#include <filestove/config.hpp>
#include <filestove/file_info.hpp>

#include <QObject>

#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <thread>
#include <vector>

namespace filestove::ui {

class Cooker : public QObject {
    Q_OBJECT;
private:
    Config m_config;
    bool m_quitRequested;
    std::condition_variable m_cv;
    std::mutex m_mtx;
    std::thread m_thread;
    std::vector<FileInfo> m_files;
public:
    Cooker(Config const& config);
    ~Cooker() override;

    Cooker(Cooker const&) = delete;
    Cooker& operator=(Cooker const&) = delete;

public slots:
    void requestQuit();
    void onConfigUpdate();
signals:
    void startWaiting();
    void startCooking();
    void collectUpdate(std::uintmax_t n_files, std::uintmax_t total_size);
    void collectCompleted(std::uintmax_t n_files, std::uintmax_t total_size);
    void cookingUpdate(std::uintmax_t bytes_read);
    void cookingCompleted();
private:
    void run();
};

}

#endif
