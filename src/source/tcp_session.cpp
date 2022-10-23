#include "../precompiled.hpp"
#include "../include/tcp_session.hpp"

#include <chrono>
#include <iostream>

#include "../include/api_handlers.hpp"
#include "../include/logger_wrapper.hpp"
#include "../include/config_manager.hpp"
#include "../include/response_template_helper.hpp"

namespace {
    std::uint8_t time_expires_socket_in_seconds = config_manager::instance().get_network_config().time_socket_expires_seconds;
}

session::session(boost::asio::ip::tcp::socket &&socket, prometheus_manager &prometheus_manager) :
        stream_(std::move(socket)),
        prometheus_manager_(prometheus_manager) {
}

void session::start_read() {
    req_ = {};
    stream_.expires_after(std::chrono::seconds(time_expires_socket_in_seconds));
    auto ptr = shared_from_this();
    boost::beast::http::async_read(stream_, buffer_, req_, boost::beast::bind_front_handler(&session::read_callback, shared_from_this()));
}

void session::read_callback(boost::beast::error_code ec, std::size_t /*bytes_transferred*/) {
    if (ec == boost::beast::http::error::end_of_stream)
        return close_stream();

    if (ec != boost::beast::error::timeout && ec) {
        logger_wrapper::log_message_in_multiple_logger(
                config_manager::instance().get_logger_config().names_of_loggers,
                "Error occurred while reading:" + ec.what(), spdlog::level::err);
        close_stream();
    } else {
        auto response = validate_request_and_get_response(req_);
        //lifetime of response need to extend.
        response_ = response;
        try {
            prometheus_manager_.get_request_metric(std::string_view(req_.method_string().data(),
                                                                    req_.method_string().size())).Increment();
        } catch (const std::runtime_error &ex) {
            logger_wrapper::log_message_in_multiple_logger(
                    config_manager::instance().get_logger_config().names_of_loggers,
                    ex.what(), spdlog::level::warn);
        }

        boost::beast::http::async_write(stream_, *response,
                                        boost::beast::bind_front_handler(&session::write_callback, shared_from_this(),
                                                                         response->need_eof()));
    }
}

void session::write_callback(bool need_to_close, boost::beast::error_code ec, std::size_t) {
    if (ec) {
            logger_wrapper::log_message_in_multiple_logger(
                    config_manager::instance().get_logger_config().names_of_loggers,
                    "Error occurred while writing:" + ec.what(), spdlog::level::err);
    }
    if (need_to_close) {
        close_stream();
    }
    try {
        prometheus_manager_.get_response_metric(std::to_string(response_->result_int())).Increment();
    } catch (const std::runtime_error &ex) {
        logger_wrapper::log_message_in_multiple_logger(
                config_manager::instance().get_logger_config().names_of_loggers,
                ex.what(), spdlog::level::warn);
    }
    response_ = nullptr;
    start_read();
}

void session::close_stream() {
    boost::beast::error_code ec;
    if (stream_.socket().is_open()) {
        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    }
    if (ec) {
        logger_wrapper::log_message_in_multiple_logger(
                config_manager::instance().get_logger_config().names_of_loggers,
                "Error occurred while close session:" + ec.what(), spdlog::level::err);
    }
}

std::shared_ptr<boost::beast::http::response<boost::beast::http::string_body>> session::validate_request_and_get_response(
        boost::beast::http::request<boost::beast::http::string_body> &request) {

    if (req_.method() != boost::beast::http::verb::get) {
        logger_wrapper::log_message_in_multiple_logger(
                config_manager::instance().get_logger_config().names_of_loggers,
                "Invalid method of request:" + std::string(req_.method_string()), spdlog::level::info);
        return std::make_shared<boost::beast::http::response<boost::beast::http::string_body>>
                (response::templates::get_bad_response("Invalid verb of response!"));
    }

    //take handler and use it if target correct, in another way - return not found.
    if (auto api_callback = api_handlers::api_context.find(request.target().to_string()); api_callback != api_handlers::api_context.end()) {
        return std::make_shared<boost::beast::http::response<boost::beast::http::string_body>>(api_callback->second(request));
    } else {
        logger_wrapper::log_message_in_multiple_logger(
                config_manager::instance().get_logger_config().names_of_loggers,
                "Invalid target of request:" + req_.target().to_string(), spdlog::level::info);
        return std::make_shared<boost::beast::http::response<boost::beast::http::string_body>>
                (response::templates::get_not_found_response("Invalid target of response!"));
    }
}
