#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "prometheus_manager.hpp"

class server : public std::enable_shared_from_this<server> {
public:
    server(boost::asio::io_context &io, boost::asio::ip::tcp::endpoint &&endpoint, prometheus_manager &prometheus_manager);

    void start();

private:
    boost::asio::io_context &io_;
    boost::asio::ip::tcp::endpoint endpoint_;
    boost::asio::ip::tcp::acceptor acceptor_;
    prometheus_manager &prometheus_manager_;

    void start_accept();

    void accept_callback(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
};
