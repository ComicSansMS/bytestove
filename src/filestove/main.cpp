#include <filestove/cook.hpp>

#include <gbBase/Finally.hpp>
#include <gbBase/Log.hpp>
#include <gbBase/LogHandlers.hpp>

#include <QApplication>

#include <chrono>

int main(int argc, char* argv[])
{
    Ghulbus::Log::initializeLogging();
    auto const guard_logging = Ghulbus::finally([]() { Ghulbus::Log::shutdownLogging(); });
    Ghulbus::Log::setLogLevel(Ghulbus::LogLevel::Trace);

    Ghulbus::Log::Handlers::LogToFile file_handler("filestove.log");
    Ghulbus::Log::Handlers::LogMultiSink upstream(Ghulbus::Log::Handlers::logToWindowsDebugger, file_handler);

    Ghulbus::Log::Handlers::LogAsync async_handler(upstream);
    Ghulbus::Log::setLogHandler(async_handler);
    async_handler.start();
    auto const guard_async_log_handler = Ghulbus::finally([&async_handler]() { async_handler.stop(); });

    QApplication the_app(argc, argv);

    auto const t0 = std::chrono::steady_clock::now();
    filestove::cook("f:\\demo\\");
    auto const t1 = std::chrono::steady_clock::now();
    GHULBUS_LOG(Info, "Took " << std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count() << "s.");

    return the_app.exec();
}
