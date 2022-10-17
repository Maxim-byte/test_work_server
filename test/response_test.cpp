#include <gtest/gtest.h>

#include <boost/beast.hpp>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "../src/include/api_handlers.hpp"
#include "../src/include/logger_wrapper.hpp"
#include "../src/include/config_manager.hpp"

TEST(base_test, response_test_base_target) {
    config_manager::path_to_config = "./../../../test/config/test_config.json";
    auto file_logger = logger_wrapper(spdlog::basic_logger_mt<spdlog::async_factory>(
            config_manager::instance().get_logger_config().name_of_file_logger, "./../logs/common.log"));

    auto console_logger = logger_wrapper(spdlog::stdout_color_mt(config_manager::instance().get_logger_config().name_of_console_logger));
    boost::beast::http::request<boost::beast::http::string_body> request{boost::beast::http::verb::get, "/v1/", 11};
    request.keep_alive(true);
    auto handler = api_handlers::api_context.find(request.target().to_string());
    EXPECT_TRUE(handler != api_handlers::api_context.end());
    auto res = handler->second(request);
    EXPECT_EQ(res.body(), "Ok!");
}

TEST(base_test, response_test_square_target) {
    config_manager::path_to_config = "./../../../test/config/test_config.json";
    auto file_logger = logger_wrapper(spdlog::basic_logger_mt<spdlog::async_factory>(
            config_manager::instance().get_logger_config().name_of_file_logger, "./../logs/common.log"));

    auto console_logger = logger_wrapper(spdlog::stdout_color_mt(config_manager::instance().get_logger_config().name_of_console_logger));
    boost::beast::http::request<boost::beast::http::string_body> request{boost::beast::http::verb::get, "/v1/square", 11};
    request.keep_alive(true);
    request.body() = std::to_string(10);
    boost::beast::http::request<boost::beast::http::string_body> request2{boost::beast::http::verb::get, "/v1/square", 11};
    request2.keep_alive(true);
    request2.body() = std::to_string(20);

    auto handler = api_handlers::api_context.find(request.target().to_string());
    EXPECT_TRUE(handler != api_handlers::api_context.end());
    auto res = handler->second(request);
    EXPECT_EQ(std::atoi(res.body().data()), 100);

    handler = api_handlers::api_context.find(request2.target().to_string());
    EXPECT_TRUE(handler != api_handlers::api_context.end());
    res = handler->second(request2);
    EXPECT_EQ(std::atoi(res.body().data()), 250);

}

TEST(base_test, response_test_invalid_target) {
    config_manager::path_to_config = "./../../../test/config/test_config.json";
    auto file_logger = logger_wrapper(spdlog::basic_logger_mt<spdlog::async_factory>(
            config_manager::instance().get_logger_config().name_of_file_logger, "./../logs/common.log"));

    auto console_logger = logger_wrapper(spdlog::stdout_color_mt(config_manager::instance().get_logger_config().name_of_console_logger));
    boost::beast::http::request<boost::beast::http::string_body> request{boost::beast::http::verb::get, "/v1/test", 11};
    request.keep_alive(true);

    auto handler = api_handlers::api_context.find(request.target().to_string());
    EXPECT_TRUE(handler == api_handlers::api_context.end());
}