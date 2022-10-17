#pragma once

#include <filesystem>
#include <string_view>

#include <nlohmann/json.hpp>

#include "config_structs.hpp"

class config_manager {
public:
    static std::string path_to_config;

    static config_manager &instance() {
        static config_manager config_manager;
        return config_manager;
    }

    [[nodiscard]] const network_config &get_network_config() const;

    [[nodiscard]] const logger_config &get_logger_config() const;

    [[nodiscard]] std::uint8_t get_flush_period_time() const;

    [[nodiscard]] std::uint8_t get_period_making_dump() const;

    [[nodiscard]] const std::filesystem::path &get_dump_directory_time() const;

private:
    config_manager();

private:
    network_config network_config_{};
    logger_config logger_config_{};

    std::uint8_t time_to_flush_logs_in_s_{};
    std::uint8_t time_to_backup_in_s_{};
    std::filesystem::path directory_to_dump_{};
};