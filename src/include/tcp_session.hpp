#pragma once

#include <boost/beast.hpp>

class session : public std::enable_shared_from_this<session>
{
public:
    explicit session(boost::asio::ip::tcp::socket &&socket);

    void start_read();

private:
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::string_body> req_;
    std::shared_ptr<void> response_type_erasure_;

    void read_callback(boost::beast::error_code ec, std::size_t /*bytes_transferred*/);

    void write_callback(bool need_to_close, boost::beast::error_code ec, std::size_t /*bytes_transferred*/);

    void close_stream();

    [[nodiscard]] std::shared_ptr<boost::beast::http::response<boost::beast::http::string_body>> validate_request_and_get_response(
            boost::beast::http::request<boost::beast::http::string_body> & request);
};
