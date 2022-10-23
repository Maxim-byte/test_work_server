#pragma once

#include <thread>
#include <string_view>
#include <unordered_map>

#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

class prometheus_manager {
public:
    explicit prometheus_manager(std::string &&address);

    ~prometheus_manager();

    auto &get_request_metric(std::string_view metric_name) {
        if (auto return_value = request_metrics_.find(metric_name); return_value != request_metrics_.end()) {
            return return_value->second;
        }
        throw std::runtime_error("Metric doesn't exist!" + std::string(metric_name));
    }

    auto &get_response_metric(std::string_view metric_name) {
        if (auto return_value = response_metrics_.find(metric_name); return_value != response_metrics_.end()) {
            return return_value->second;
        }
        throw std::runtime_error("Metric doesn't exist!" + std::string(metric_name));
    }

    void start_post_system_metrics();

    void stop();

private:
    std::thread thread_;
    prometheus::Exposer exposer_;
    std::atomic<bool> is_running = true;
    std::shared_ptr<prometheus::Registry> registry_;

    //monitoring request
    prometheus::Family<prometheus::Counter> &request_counter_;
    std::unordered_map<std::string_view, prometheus::Counter &> request_metrics_;

    //monitoring response status
    prometheus::Family<prometheus::Counter> &response_status_counter_;
    std::unordered_map<std::string_view, prometheus::Counter &> response_metrics_;

    //monitoring server at all
    prometheus::Family<prometheus::Gauge> &general_metrics_gauge_;
    std::unordered_map<std::string_view, prometheus::Gauge &> system_metrics_;

    void post_general_metric();
};