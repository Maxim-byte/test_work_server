#include "../precompiled.hpp"
#include "../include/api_handlers.hpp"

#include <ranges>
#include <charconv>

#include "../include/logger_group.hpp"
#include "../include/config_manager.hpp"
#include "../include/unique_container.hpp"
#include "../include/response_template_helper.hpp"

namespace {
    constexpr std::uint16_t max_count_in_container = 1024;
}

namespace {
    constexpr auto square = [](std::uint16_t value) -> std::uint32_t {
        return value * value;
    };
}

api_handlers::response_with_string_body api_handlers::main_handler(api_handlers::request_with_string_body &request) {
    boost::beast::http::response<boost::beast::http::string_body>
            response{boost::beast::http::status::ok, 11};

    logger_group::log_message_to_group("Success receive request: " + request.target().to_string(), spdlog::level::info);

    response.body() = "Ok!";
    response.keep_alive(request.keep_alive());
    response.prepare_payload();
    return response;
}

api_handlers::response_with_string_body api_handlers::square_handler(api_handlers::request_with_string_body &request) {
    boost::beast::http::response<boost::beast::http::string_body>
            response{boost::beast::http::status::ok, 11};

    logger_group::log_message_to_group("Success receive request: " + request.target().to_string(), spdlog::level::info);

    std::uint16_t value = 0;
    auto result = std::from_chars(request.body().data(), request.body().data() + request.body().size(), value);
    if (result.ec != std::errc() || value > max_count_in_container) {
        return response::templates::get_bad_response("Invalid data inside request!");
    }

    //processing container operation
    std::unique_lock<std::mutex> unique_lock(container::unique_container_mutex);
    container::unique_container.insert(value);

    auto modified_iterator = container::unique_container | std::views::transform(square);
    response.body() = std::to_string(std::accumulate(modified_iterator.begin(), modified_iterator.end(), 0.0) /
                                     static_cast<double>(container::unique_container.size()));
    unique_lock.unlock();

    response.keep_alive(request.keep_alive());
    response.prepare_payload();
    return response;
}
