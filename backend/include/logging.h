#ifndef BACKEND_INCLUDE_LOGGING_HPP
#define BACKEND_INCLUDE_LOGGING_HPP

#include <filesystem>
#include <spdlog/logger.h>

std::shared_ptr<spdlog::logger> IntializeLogging(std::filesystem::path path = "");
    
#endif// BACKEND_INCLUDE_LOGGING_HPP