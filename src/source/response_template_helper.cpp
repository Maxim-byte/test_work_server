#include "../precompiled.hpp"
#include "../include/response_template_helper.hpp"

auto response::templates::get_bad_response(
        std::string_view what_happens) -> response::templates::response_with_string_body {
    boost::beast::http::response<boost::beast::http::string_body>
            response{boost::beast::http::status::bad_request, 11};
    response.body() = what_happens;
    response.keep_alive(false);
    response.prepare_payload();
    return response;
}

auto response::templates::get_not_found_response(
        std::string_view what_happens) -> response::templates::response_with_string_body {
    boost::beast::http::response<boost::beast::http::string_body>
            response{boost::beast::http::status::not_found, 11};
    response.body() = what_happens;
    response.keep_alive(false);
    response.prepare_payload();
    return response;
}
