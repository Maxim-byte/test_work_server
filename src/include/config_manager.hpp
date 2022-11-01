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

    [[nodiscard]] const rest_config &get_network_config() const;

    [[nodiscard]] const logger_config &get_logger_config() const;

    [[nodiscard]] std::uint8_t get_flush_period_time() const;

    [[nodiscard]] std::uint8_t get_period_making_dump() const;

    [[nodiscard]] const std::filesystem::path &get_dump_directory_time() const;

    [[nodiscard]] const prometheus_config &get_prometheus_config() const;

    [[nodiscard]] const rpc_config &get_rpc_config() const;

private:
    config_manager();

    void read_rpc_config(nlohmann::json & config);

    void read_common_config(nlohmann::json & config);

    void read_network_config(nlohmann::json & config);

    void read_logger_config(nlohmann::json & config);

    void read_prometheus_config(nlohmann::json & config);

private:
    rpc_config rpc_config_{};
    rest_config network_config_{};
    logger_config logger_config_{};
    prometheus_config prometheus_config_{};

    std::uint8_t time_to_flush_logs_in_s_{};
    std::uint8_t time_to_backup_in_s_{};
    std::filesystem::path directory_to_dump_{};
};