#pragma once

#include <cstdint>
#include <mutex>
#include <unordered_set>

namespace container {
    inline std::mutex unique_container_mutex;
    inline std::unordered_set<std::uint16_t> unique_container;
}
