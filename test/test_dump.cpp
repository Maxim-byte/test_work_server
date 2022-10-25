#include <gtest/gtest.h>

#include <string_view>
#include <filesystem>
#include <unordered_set>

#include <boost/archive/binary_iarchive.hpp>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "../src/include/dump_manager.hpp"
#include "../src/include/config_manager.hpp"

namespace {
    constexpr auto dump_samples = 5;
}


TEST(base_test, dump_test)
{
    config_manager::path_to_config = "./../../../test/config/test_config.json";
    auto file_logger = logger_wrapper(spdlog::basic_logger_mt<spdlog::async_factory>(
            config_manager::instance().get_logger_config().name_of_file_logger, "./../logs/common.log"));

    auto console_logger = logger_wrapper(spdlog::stdout_color_mt(config_manager::instance().get_logger_config().name_of_console_logger));
    std::unordered_set<std::uint16_t> serialize_set{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::mutex mutex;
    dump_manager<decltype(serialize_set), decltype(mutex)> dumpManager(serialize_set, mutex, "./");
    dumpManager.start();

    std::this_thread::sleep_for(
            std::chrono::seconds(config_manager::instance().get_period_making_dump() *dump_samples));

    std::unordered_set<std::uint16_t> deserialize_set;
    auto directory_it = std::filesystem::directory_iterator("./");
    auto most_recent_file = *begin(directory_it);
    for(auto & file : directory_it) {
        if(std::filesystem::last_write_time(file.path()) > std::filesystem::last_write_time(most_recent_file.path())) {
            most_recent_file = file;
        }
    }
    std::ifstream stream_to_backup(most_recent_file.path());
    EXPECT_TRUE(stream_to_backup.is_open());
    boost::archive::binary_iarchive ia(stream_to_backup, boost::archive::archive_flags::no_header);
    ia >> deserialize_set;
    EXPECT_TRUE(serialize_set == deserialize_set);
}

TEST(base_test, directory_not_exist_dump_test)
{
    constexpr std::string_view directory_name_for_dumping = "./test_directory";

    config_manager::path_to_config = "./../../../test/config/test_config.json";
    auto file_logger = logger_wrapper(spdlog::basic_logger_mt<spdlog::async_factory>(
            config_manager::instance().get_logger_config().name_of_file_logger, "./../logs/common.log"));

    auto console_logger = logger_wrapper(spdlog::stdout_color_mt(config_manager::instance().get_logger_config().name_of_console_logger));
    std::unordered_set<std::uint16_t> serialize_set{};
    std::mutex mutex;
    dump_manager<decltype(serialize_set), decltype(mutex)> dumpManager(serialize_set, mutex, directory_name_for_dumping);
    dumpManager.start();

    std::this_thread::sleep_for(std::chrono::seconds(config_manager::instance().get_period_making_dump() * dump_samples));

    EXPECT_TRUE(std::filesystem::exists(directory_name_for_dumping));
    std::filesystem::remove_all(directory_name_for_dumping);
}
