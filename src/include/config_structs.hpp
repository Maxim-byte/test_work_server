#pragma once

#include <string>
#include <cstdint>

#include <boost/asio.hpp>

struct rpc_config {
    std::string host;
    std::uint16_t port;
};

struct rest_config {
    boost::asio::ip::address_v4 host;
    std::uint16_t port;
    std::uint8_t time_socket_expires_seconds;
};

struct logger_config {
    std::string name_of_file_logger;
    std::string name_of_console_logger;
};


struct prometheus_config {
    std::string host;
    std::uint16_t port;
};
