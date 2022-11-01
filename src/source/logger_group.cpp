#include "../include/logger_group.hpp"

std::vector<std::shared_ptr<spdlog::logger>> logger_group::loggers_;

void logger_group::add_logger_to_group(const std::string &name_of_logger) {
    if(auto logger = spdlog::get(name_of_logger); logger) {
        loggers_.push_back(logger);
    }
}
