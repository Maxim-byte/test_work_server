#pragma once

#include <string_view>
#include <unordered_map>

#include <boost/beast.hpp>

struct api_handlers {
    using response_with_string_body = boost::beast::http::response<boost::beast::http::string_body>;
    using request_with_string_body = boost::beast::http::request<boost::beast::http::string_body>;

    static response_with_string_body main_handler(request_with_string_body &request);

    static response_with_string_body square_handler(request_with_string_body &request);

    inline const static std::unordered_map<std::string_view, std::function<api_handlers::response_with_string_body(api_handlers::request_with_string_body&)>> api_context {
            {"/v1/", &api_handlers::main_handler},
            {"/v1/square", &api_handlers::square_handler}
    };
};
