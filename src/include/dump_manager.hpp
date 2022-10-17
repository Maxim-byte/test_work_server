#pragma once

#include <thread>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <system_error>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../include/config_manager.hpp"
#include "../include/logger_wrapper.hpp"

template<class T> requires std::ranges::input_range<T>
class dump_manager {
public:
    explicit dump_manager(const T &container, const std::filesystem::path &path_to_directory) :
    container_(container),
    path_to_dump_directory_(path_to_directory)
    {}

    ~dump_manager() {
        is_running = false;
        thread_.join();
    }

    void start() {
        thread_ = std::thread(&dump_manager::main_loop_for_dumping, this);
    }

    void stop() {
        is_running = false;
    }

private:
    const T &container_;
    std::thread thread_;
    std::atomic<bool> is_running = true;
    const std::filesystem::path path_to_dump_directory_;

    void main_loop_for_dumping() {
        while (is_running) {
            std::this_thread::sleep_for(std::chrono::seconds(config_manager::instance().get_period_making_dump()));
            make_dump(path_to_dump_directory_);
        }
    }

    std::error_code make_dump(const std::filesystem::path &path_to_directory) {
        std::error_code ec;
        if (!std::filesystem::exists(path_to_directory)) {
            std::filesystem::create_directory(path_to_directory, ec);
        }

        if (ec) {
            logger_wrapper::log_message_in_multiple_logger(config_manager::instance().get_logger_config().names_of_loggers,
                                                           "Can't create dump directory:", spdlog::level::err);
        }

        boost::posix_time::ptime local_time = boost::posix_time::second_clock::local_time();
        static std::stringstream file_name;
        file_name.str("");
        file_name << "dump_" << local_time << ".bin";
        std::ofstream stream_to_backup_file(path_to_directory.string() + "/" + file_name.str());

        if (!stream_to_backup_file.is_open()) {
            logger_wrapper::log_message_in_multiple_logger(config_manager::instance().get_logger_config().names_of_loggers,
                                                           "Can't open stream to dump file:", spdlog::level::err);
            return std::make_error_code(std::errc::text_file_busy);
        }

        boost::archive::binary_oarchive serialization(stream_to_backup_file, boost::archive::archive_flags::no_header);
        serialization & container_;
        logger_wrapper::log_message_in_multiple_logger(config_manager::instance().get_logger_config().names_of_loggers,
                                                       "Dump created successfully:", spdlog::level::info);
        return {};
    }
};
