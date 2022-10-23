#include "../include/prometheus_manager.hpp"

#include <chrono>
#include <iostream>

#include "../include/config_manager.hpp"
#include "../include/logger_wrapper.hpp"
#include "../include/prometheus_details.hpp"

prometheus_manager::prometheus_manager(std::string &&address) :
        exposer_{address},
        registry_(std::make_shared<prometheus::Registry>()),
        request_counter_(prometheus::BuildCounter()
                                 .Name("observed_request_total")
                                 .Help("Number of observed request")
                                 .Register(*registry_)),
        response_status_counter_(prometheus::BuildCounter()
                                         .Name("observed_response_status_total")
                                         .Help("Number of observed response statuses")
                                         .Register(*registry_)),
        general_metrics_gauge_(prometheus::BuildGauge()
                                       .Name("observed_system_status")
                                       .Help("System metrics")
                                       .Register(*registry_)) {
    try {
        request_metrics_.insert({"GET", request_counter_.Add({{"request", "GET"}})});
        request_metrics_.insert({"POST", request_counter_.Add({{"request", "POST"}})});

        response_metrics_.insert({"200", response_status_counter_.Add({{"response", "200"}})});
        response_metrics_.insert({"404", response_status_counter_.Add({{"response", "404"}})});
        response_metrics_.insert({"500", response_status_counter_.Add({{"response", "500"}})});

        system_metrics_.insert({"CPU", general_metrics_gauge_.Add({{"load_cpu_in_percentage", "usage"}})});
        system_metrics_.insert({"memory", general_metrics_gauge_.Add({{"used_memory_usage", "usage"}})});

        exposer_.RegisterCollectable(registry_);
    } catch (...) {
        std::cerr << "Invalid initializing prometheus manager!" << std::endl;
        std::exit(1);
    }
}

void prometheus_manager::start_post_system_metrics() {
    thread_ = std::move(std::thread(&prometheus_manager::post_general_metric, this));
}

prometheus_manager::~prometheus_manager() {
    stop();
}

void prometheus_manager::stop() {
    is_running = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void prometheus_manager::post_general_metric() {
    //it makes no sense to do less than 20ms sleep, because taking metrics takes 15ms
    while (is_running) {
        if (auto used_memory = prometheus_details::get_used_ram(); used_memory == 0) {
            logger_wrapper::log_message_in_multiple_logger(
                    config_manager::instance().get_logger_config().names_of_loggers,
                    "Can't get used memory!", spdlog::level::warn);
        } else {
            system_metrics_.find("memory")->second.Set(used_memory);
        }
        if (auto used_cpu = prometheus_details::get_cpu_average_load(); used_cpu == 0) {
            logger_wrapper::log_message_in_multiple_logger(
                    config_manager::instance().get_logger_config().names_of_loggers,
                    "Can't get used cpu!", spdlog::level::warn);
        } else {
            system_metrics_.find("CPU")->second.Set(used_cpu);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
