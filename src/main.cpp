#include "precompiled.hpp"

#include <memory>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <boost/asio.hpp>

#include "include/server.hpp"
#include "include/logger_group.hpp"
#include "include/dump_manager.hpp"
#include "include/config_manager.hpp"
#include "include/unique_container.hpp"
#include "include/prometheus_manager.hpp"

int main() {
    const auto &config_manager = config_manager::instance();

    //setup logger
    auto file_logger = spdlog::basic_logger_mt<spdlog::async_factory>(
            config_manager.get_logger_config().name_of_file_logger, "./../logs/common.log");
    file_logger->set_error_handler([](const std::string &string) {
        std::cerr << string << std::endl;
    });

    auto console_logger = spdlog::stdout_color_mt(config_manager.get_logger_config().name_of_console_logger);
    console_logger->set_error_handler([](const std::string &string) {
        std::cerr << string << std::endl;
    });

    spdlog::flush_every(std::chrono::seconds(config_manager.get_flush_period_time()));

    logger_group::add_logger_to_group(config_manager.get_logger_config().name_of_console_logger);
    logger_group::add_logger_to_group(config_manager.get_logger_config().name_of_file_logger);

    //setup prometheus
    prometheus_manager prometheus_manager(config_manager.get_prometheus_config().host + ":"
                                          + std::to_string(config_manager.get_prometheus_config().port));
    prometheus_manager.start_post_system_metrics();

    //setup dump manager
    dump_manager<decltype(container::unique_container), decltype(container::unique_container_mutex)> dump_manager(
            container::unique_container,
            container::unique_container_mutex,
            config_manager::instance().get_dump_directory_time());
    dump_manager.start();

    boost::asio::io_context io;

    //correct closing by signal
    boost::asio::signal_set signals(io, SIGTERM);
    signals.async_wait([&](const boost::system::error_code &error, int signal_number) {
        console_logger->info("Close application and making dump, it can takes several minutes!");
        io.stop();
    });

    //setup http server
    std::shared_ptr<server> http_server;
    try {
        http_server = std::make_shared<server>(
                io,
                boost::asio::ip::tcp::endpoint
                        {config_manager.get_network_config().host,
                         config_manager.get_network_config().port
                        },
                prometheus_manager);

        http_server->start();

        logger_group::log_message_to_group("Server successfully start!", spdlog::level::info);
    } catch (const std::exception &ex) {
        logger_group::log_message_to_group(ex.what(), spdlog::level::err);
    }
    io.run();
    spdlog::drop_all();
}
