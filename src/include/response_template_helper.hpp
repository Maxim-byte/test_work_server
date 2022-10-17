#pragma once

#include <boost/beast.hpp>

namespace response::templates {
    using response_with_string_body = boost::beast::http::response<boost::beast::http::string_body>;

    auto get_bad_response(std::string_view what_happens) -> response_with_string_body;

    auto get_not_found_response(std::string_view what_happens) -> response_with_string_body;
}
