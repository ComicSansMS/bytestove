#include <filestove/config.hpp>
#include <filestove/ui/config_holder.hpp>
#include <filestove/ui/cooker.hpp>
#include <filestove/ui/tray_icon.hpp>
#include <filestove/ui/widget.hpp>
#include <filestove/ui/options_dialog.hpp>

#include <gbBase/Finally.hpp>
#include <gbBase/Log.hpp>
#include <gbBase/LogHandlers.hpp>

#include <QApplication>
#include <QMessageBox>

#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{
    QApplication the_app(argc, argv);

    Ghulbus::Log::initializeLogging();
    auto const guard_logging = Ghulbus::finally([]() { Ghulbus::Log::shutdownLogging(); });
    Ghulbus::Log::setLogLevel(Ghulbus::LogLevel::Info);

#if defined WIN32 && !defined NDEBUG
    Ghulbus::Log::setLogHandler(Ghulbus::Log::Handlers::logToWindowsDebugger);
    Ghulbus::Log::setLogLevel(Ghulbus::LogLevel::Trace);
#endif

    filestove::Config config;
    try {
        config = filestove::readConfig();
    } catch (std::exception& e) {
        QMessageBox msgbox;
        msgbox.setText("Error reading config file");
        msgbox.setInformativeText(QString::fromStdString(e.what()));
        msgbox.setStandardButtons(QMessageBox::Ignore | QMessageBox::Cancel);
        msgbox.setStandardButtons(QMessageBox::Cancel);
        int ret = msgbox.exec();
        if (ret == QMessageBox::Cancel) { return 1; }
    }

    std::unique_ptr<Ghulbus::Log::Handlers::LogAsync> async_handler;
    if (config.doLogging) {
        Ghulbus::Log::Handlers::LogToFile file_handler("filestove.log");
        Ghulbus::Log::Handlers::LogMultiSink upstream(Ghulbus::Log::getLogHandler(),
                                                      file_handler);
        async_handler = std::make_unique<Ghulbus::Log::Handlers::LogAsync>(upstream);
        Ghulbus::Log::setLogHandler(*async_handler);
        async_handler->start();
    }
    auto const guard_async_log_handler = Ghulbus::finally(
        [&async_handler]() { if (async_handler) { async_handler->stop(); } });

    filestove::ui::ConfigHolder config_holder{ config };
    filestove::ui::StoveWidget widget{ config.directories };
    filestove::ui::Cooker cooker{ config };
    filestove::ui::TrayIcon tray_icon;
    QObject::connect(&the_app, &QApplication::aboutToQuit,
                     &cooker, &filestove::ui::Cooker::requestQuit);
    QObject::connect(&widget, &filestove::ui::StoveWidget::pathlistUpdate,
                     &config_holder, &filestove::ui::ConfigHolder::updatePathlist);
    QObject::connect(&config_holder, &filestove::ui::ConfigHolder::configChanged,
                     &cooker, &filestove::ui::Cooker::onConfigUpdate);
    QObject::connect(&cooker, &filestove::ui::Cooker::startWaiting,
                     &tray_icon, &filestove::ui::TrayIcon::extinguish);
    QObject::connect(&cooker, &filestove::ui::Cooker::startCooking,
                     &tray_icon, &filestove::ui::TrayIcon::ignite);
    QObject::connect(&cooker, &filestove::ui::Cooker::cookingCompleted,
                     &tray_icon, &filestove::ui::TrayIcon::finished);

    //filestove::ui::OptionsDialog dialg;
    //dialg.exec();

    return the_app.exec();
}
