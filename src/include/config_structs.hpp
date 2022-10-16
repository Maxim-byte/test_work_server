#pragma once

#include <string>
#include <cstdint>

#include "boost/asio.hpp"

struct network_config {
    boost::asio::ip::address_v4 host;
    std::uint16_t port;
    std::uint8_t time_socket_expires_seconds;
};

struct logger_config {
    std::string name_of_file_logger;
    std::string name_of_console_logger;
    std::vector<std::string> names_of_loggers;
};
