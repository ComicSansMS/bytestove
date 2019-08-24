#include <filestove/cook.hpp>
#include <filestove/activity_monitor.hpp>

#include <gbBase/Finally.hpp>
#include <gbBase/Log.hpp>
#include <gbBase/LogHandlers.hpp>

#include <QApplication>

#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{
    Ghulbus::Log::initializeLogging();
    auto const guard_logging = Ghulbus::finally([]() { Ghulbus::Log::shutdownLogging(); });
    Ghulbus::Log::setLogLevel(Ghulbus::LogLevel::Trace);

    /*
    Ghulbus::Log::Handlers::LogToFile file_handler("filestove.log");
    Ghulbus::Log::Handlers::LogMultiSink upstream(Ghulbus::Log::Handlers::logToWindowsDebugger, file_handler);
    /*/
    auto upstream = Ghulbus::Log::Handlers::logToWindowsDebugger;
    //*/

    Ghulbus::Log::Handlers::LogAsync async_handler(upstream);
    Ghulbus::Log::setLogHandler(async_handler);
    async_handler.start();
    auto const guard_async_log_handler = Ghulbus::finally([&async_handler]() { async_handler.stop(); });

    QApplication the_app(argc, argv);

    auto const t0 = std::chrono::steady_clock::now();
    filestove::cook("c:\\doc\\SFML-2.5.1");
    auto const t1 = std::chrono::steady_clock::now();
    GHULBUS_LOG(Info, "Took " << std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count() << "s.");

    filestove::ActivityMonitor mon;
    for (int i = 0; i < 128; ++i) {
        mon.collect();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return the_app.exec();
}
