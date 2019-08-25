#include <filestove/config.hpp>
#include <filestove/ui/cooker.hpp>
#include <filestove/ui/widget.hpp>

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

    filestove::ui::StoveWidget widget;
    filestove::ui::Cooker cooker;
    QObject::connect(&the_app, &QApplication::aboutToQuit,
                     &cooker, &filestove::ui::Cooker::requestQuit);

    return the_app.exec();
}
