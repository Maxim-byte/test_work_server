#pragma once

namespace prometheus_details {
    [[nodiscard]] double get_used_ram();
    [[nodiscard]] double get_cpu_average_load();
}
