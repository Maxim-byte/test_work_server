#include <memory>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <boost/asio.hpp>

#include "include/server.hpp"
#include "include/config_manager.hpp"
#include "include/logger_wrapper.hpp"
#include "include/unique_container.hpp"
#include "include/dump_manager.hpp"

int main() {
    const auto &config_manager = config_manager::instance();

    //setup logger
    auto file_logger = logger_wrapper(spdlog::basic_logger_mt<spdlog::async_factory>(
            config_manager.get_logger_config().name_of_file_logger, "/home/maxim/CLionProjects/test_work_server/logs/common.log"));

    auto console_logger = logger_wrapper(spdlog::stdout_color_mt(config_manager.get_logger_config().name_of_console_logger));

    //setup dump manager
    dump_manager<decltype(container::unique_container)> dump_manager(container::unique_container);
    dump_manager.start();
    boost::asio::io_context io{8};

    std::shared_ptr<server> http_server;

    //setup http server
    try {
        http_server = std::make_shared<server>(io,
                                               boost::asio::ip::tcp::endpoint
                                                       {config_manager.get_network_config().host,
                                                        config_manager.get_network_config().port
                                                       });
        http_server->start();
    } catch (const std::exception &ex) {
        logger_wrapper::log_message_in_multiple_logger(
                std::vector<std::string>{config_manager::instance().get_logger_config().name_of_file_logger,
                                         config_manager::instance().get_logger_config().name_of_console_logger},
                ex.what(), spdlog::level::critical);
        std::exit(1);
    }

    //run main task-queue
    io.run();
}
