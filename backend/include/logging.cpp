#include "logging.h"

#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

std::shared_ptr<spdlog::logger> IntializeLogging(std::filesystem::path path)
{
    spdlog::level::level_enum level { spdlog::level::debug };
    
    auto consoleSink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(level);
    consoleSink->set_pattern("%Y-%m-%d %X %^%6l%$: %v");

    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(std::move(consoleSink));

    if (!path.empty()) 
    {
        auto fileSink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.string());
        fileSink->set_level(spdlog::level::debug);
        fileSink->set_pattern("%Y-%m-%d %X %6l: [%s:%#] %!(): %v");
        sinks.emplace_back(std::move(fileSink));
    }
    auto newLogger = std::make_shared<spdlog::logger>("basic_logger", std::begin(sinks),
                                                       std::end(sinks));
    newLogger->flush_on(level);
    newLogger->set_level(level);
    return newLogger;
}
