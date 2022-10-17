#include "../precompiled.hpp"
#include "../include/config_manager.hpp"

#include <fstream>
#include <iostream>
#include <exception>

#include <boost/asio.hpp>

std::string config_manager::path_to_config =  "./../../config/common_config.json";

config_manager::config_manager() {
    std::ifstream config_stream(path_to_config);
    assert(config_stream.is_open());

    try {
        nlohmann::json config = nlohmann::json::parse(config_stream);

        time_to_backup_in_s_ = config["time_to_backup_s"].get<std::uint8_t>();
        assert(time_to_backup_in_s_ != 0);

        time_to_flush_logs_in_s_ = config["time_to_flush_logs_s"].get<std::uint8_t>();
        assert(time_to_flush_logs_in_s_ != 0);

        directory_to_dump_ = config["directory_to_dump"].get<std::string>();

        network_config_.port = config["network"]["port"].get<std::uint16_t>();
        auto host = config["network"]["host"].get<std::string>();
        network_config_.time_socket_expires_seconds = config["network"]["time_socket_expires_s"].get<std::uint8_t>();

        boost::system::error_code ec;
        network_config_.host = boost::asio::ip::make_address_v4(host, ec);
        assert(!ec && "Host is inconvertible to ipV4!");

        logger_config_.name_of_file_logger = config["logging"]["file_logger"].get<std::string>();
        logger_config_.name_of_console_logger = config["logging"]["console_logger"].get<std::string>();

        assert(logger_config_.name_of_console_logger != logger_config_.name_of_file_logger);
        logger_config_.names_of_loggers.push_back(logger_config_.name_of_console_logger);
        logger_config_.names_of_loggers.push_back(logger_config_.name_of_file_logger);

    } catch (const std::exception & ex) {
        std::cerr << "Can't init config_manager: " << ex.what() << std::endl;
        std::exit(1);
    }
}

const network_config &config_manager::get_network_config() const {
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
