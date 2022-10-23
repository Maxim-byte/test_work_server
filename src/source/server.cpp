#include "../precompiled.hpp"
#include "../include/server.hpp"

#include <memory>
#include <iostream>

#include "../include/tcp_session.hpp"
#include "../include/config_manager.hpp"
#include "../include/logger_wrapper.hpp"

server::server(boost::asio::io_context &io, boost::asio::ip::tcp::endpoint &&endpoint, prometheus_manager &prometheus_manager) :
        io_(io), endpoint_(std::move(endpoint)), acceptor_(boost::asio::make_strand(io)), prometheus_manager_(prometheus_manager) {
    boost::system::error_code ec;
    acceptor_.open(endpoint_.protocol(), ec);
    if (ec) {
        throw std::runtime_error("Cant open acceptor!");
    }
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if (ec) {
        throw std::runtime_error("Cant set reusing address!");
    }
    acceptor_.bind(endpoint_, ec);
    if (ec) {
        throw std::runtime_error("Cant bind acceptor!");
    }
    acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) {
        throw std::runtime_error("Cant listen using acceptor!");
    }
}

void server::start() {
    start_accept();
}

void server::start_accept() {
    acceptor_.async_accept(boost::asio::make_strand(io_),
                           boost::beast::bind_front_handler(&server::accept_callback, shared_from_this()));
}

void server::accept_callback(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket) {
    if (!ec) {
        logger_wrapper::log_message_in_multiple_logger(
                config_manager::instance().get_logger_config().names_of_loggers,
                "New client accepted!", spdlog::level::info);
        std::make_shared<session>(std::move(socket), prometheus_manager_)->start_read();
    }
    start_accept();
}
