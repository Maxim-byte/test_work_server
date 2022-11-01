#include "../include/rpc_handlers.hpp"

#include <rpc/this_handler.h>

#include "../include/unique_container.hpp"

namespace {
    constexpr auto square = [](std::uint16_t value) -> std::uint32_t {
        return value * value;
    };
}
namespace {
    constexpr std::uint16_t max_count_in_container = 1024;
}


std::string rpc::main_handler() {
    return "Ok!";
}

double rpc::square_handler(std::uint16_t value) {
    if (value > max_count_in_container) {
        rpc::this_handler().respond_error(std::make_tuple(400, "Invalid data!"));
    }
    std::lock_guard<std::mutex> lock_guard(container::unique_container_mutex);
    container::unique_container.insert(value);
    auto modified_iterator = container::unique_container | std::views::transform(square);
    return std::accumulate(modified_iterator.begin(), modified_iterator.end(), 0.0) /
           static_cast<double>(container::unique_container.size());
}
