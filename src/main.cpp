#include "precompiled.hpp"

#include <memory>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <boost/asio.hpp>

#include "include/server.hpp"
#include "include/dump_manager.hpp"
#include "include/logger_wrapper.hpp"
#include "include/config_manager.hpp"
#include "include/unique_container.hpp"

namespace {
    constexpr std::uint8_t logger_flush_sec = 5;
}

int main() {
    const auto &config_manager = config_manager::instance();

    //setup logger
    auto file_logger = logger_wrapper(spdlog::basic_logger_mt<spdlog::async_factory>(
            config_manager.get_logger_config().name_of_file_logger, "./../logs/common.log"));
    auto console_logger = logger_wrapper(spdlog::stdout_color_mt(config_manager.get_logger_config().name_of_console_logger));
    spdlog::flush_every(std::chrono::seconds(config_manager.get_flush_period_time()));

    //setup dump manager
    dump_manager<decltype(container::unique_container)> dump_manager(container::unique_container,
                                                                     config_manager::instance().get_dump_directory_time());
    dump_manager.start();

    boost::asio::io_context io;

    //correct closing by signal
    boost::asio::signal_set signals(io, SIGTERM);
    signals.async_wait([&](const boost::system::error_code &error, int signal_number) {
        console_logger.logger()->info("Close application, it can takes several minutes!");
        io.stop();
    });

    //setup http server
    std::shared_ptr<server> http_server;
    try {
        http_server = std::make_shared<server>(io,
                                               boost::asio::ip::tcp::endpoint
                                                       {config_manager.get_network_config().host,
                                                        config_manager.get_network_config().port
                                                       });
        http_server->start();
        logger_wrapper::log_message_in_multiple_logger(config_manager::instance().get_logger_config().names_of_loggers,
                                                       "Server successfully start!", spdlog::level::info);
    } catch (const std::exception &ex) {
        logger_wrapper::log_message_in_multiple_logger(config_manager::instance().get_logger_config().names_of_loggers,
                                                       ex.what(), spdlog::level::critical);
        std::exit(1);
    }
    io.run();
}
