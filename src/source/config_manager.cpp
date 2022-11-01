#include "../precompiled.hpp"
#include "../include/config_manager.hpp"

#include <fstream>
#include <iostream>
#include <exception>

#include <boost/asio.hpp>

std::string config_manager::path_to_config = "./../../config/common_config.json";

config_manager::config_manager() {
    std::ifstream config_stream(path_to_config);
    assert(config_stream.is_open());

    try {
        nlohmann::json config = nlohmann::json::parse(config_stream);

        read_rpc_config(config);
        read_common_config(config);
        read_logger_config(config);
        read_network_config(config);
        read_prometheus_config(config);


    } catch (const std::exception &ex) {
        std::cerr << "Can't init config_manager: " << ex.what() << std::endl;
        std::exit(1);
    }
}

void config_manager::read_rpc_config(nlohmann::json &config) {
    rpc_config_.host = config["rpc_config"]["host"].get<std::string>();
    rpc_config_.port = config["rpc_config"]["port"].get<std::uint16_t>();
    assert(rpc_config_.port != 0);

    boost::system::error_code ec;
    boost::asio::ip::make_address_v4(rpc_config_.host, ec);
    assert(!ec && "Rpc host is inconvertible to ipV4!");
}

void config_manager::read_common_config(nlohmann::json &config) {
    time_to_backup_in_s_ = config["common_settings"]["time_to_backup_s"].get<std::uint8_t>();
    assert(time_to_backup_in_s_ != 0);

    time_to_flush_logs_in_s_ = config["common_settings"]["time_to_flush_logs_s"].get<std::uint8_t>();
    assert(time_to_flush_logs_in_s_ != 0);

    directory_to_dump_ = config["common_settings"]["directory_to_dump"].get<std::string>();
    assert(!directory_to_dump_.empty());
}

void config_manager::read_network_config(nlohmann::json &config) {
    network_config_.port = config["rest_config"]["port"].get<std::uint16_t>();
    assert(network_config_.port != 0);
    auto host = config["rest_config"]["host"].get<std::string>();
    network_config_.time_socket_expires_seconds = config["rest_config"]["time_socket_expires_s"].get<std::uint8_t>();

    boost::system::error_code ec;
    network_config_.host = boost::asio::ip::make_address_v4(host, ec);
    assert(!ec && "Server host is inconvertible to ipV4!");
}

void config_manager::read_logger_config(nlohmann::json &config) {
    logger_config_.name_of_file_logger = config["logging"]["file_logger"].get<std::string>();
    logger_config_.name_of_console_logger = config["logging"]["console_logger"].get<std::string>();
    assert(logger_config_.name_of_console_logger != logger_config_.name_of_file_logger);
}

void config_manager::read_prometheus_config(nlohmann::json &config) {
    prometheus_config_.host = config["prometheus_config"]["host"].get<std::string>();
    prometheus_config_.port = config["prometheus_config"]["port"].get<std::uint16_t>();
    assert(prometheus_config_.port != 0);

    boost::system::error_code ec;
    boost::asio::ip::make_address_v4(prometheus_config_.host, ec);
    assert(!ec && "Prometheus host is inconvertible to ipV4!");
}

const rest_config &config_manager::get_network_config() const {
    return network_config_;
}

const logger_config &config_manager::get_logger_config() const {
    return logger_config_;
}

std::uint8_t config_manager::get_flush_period_time() const {
    return time_to_flush_logs_in_s_;
}

std::uint8_t config_manager::get_period_making_dump() const {
    return time_to_backup_in_s_;
}

const std::filesystem::path &config_manager::get_dump_directory_time() const {
    return directory_to_dump_;
}

const prometheus_config &config_manager::get_prometheus_config() const {
    return prometheus_config_;
}

const rpc_config &config_manager::get_rpc_config() const {
    return rpc_config_;
}
