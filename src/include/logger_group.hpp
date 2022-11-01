#pragma once

#include <vector>
#include <memory>

#include <spdlog/spdlog.h>

class logger_group {
public:
    static void add_logger_to_group(const std::string & name_of_logger);

    template<class T>
    static void log_message_to_group(T && object_to_log, spdlog::level::level_enum log_level) {
        std::for_each(loggers_.begin(), loggers_.end(), [&](const std::shared_ptr<spdlog::logger> & logger) {
            logger->log(log_level, object_to_log);
        });
    }

private:
    static std::vector<std::shared_ptr<spdlog::logger>> loggers_;
};
