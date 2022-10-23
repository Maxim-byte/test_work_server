#include "../include/prometheus_details.hpp"

#include <string>
#include <fstream>
#include <charconv>
#include <iostream>

#include <boost/algorithm/string.hpp>

double prometheus_details::get_used_ram() {
    system("free -h > memory.txt");
    std::ifstream memory_stream("memory.txt", std::ios_base::in);
    if(!memory_stream.is_open()) {
        return 0.;
    }
    static std::string empty_string, mem_word, total_memory, used_memory;
    std::getline(memory_stream, empty_string);
    memory_stream >> mem_word >> total_memory >> used_memory;
    boost::replace_all(used_memory, ",", ".");
    double return_value = 0;
    std::from_chars(used_memory.data(), used_memory.data() + used_memory.size(), return_value);
    return return_value;
}

double prometheus_details::get_cpu_average_load() {
    constexpr auto max_percentage = 100;
    system("mpstat -P ALL > cpu.txt");
    std::ifstream cpu_stream("cpu.txt", std::ios_base::in);
    if(!cpu_stream.is_open()) {
        return 0.;
    }
    static std::string empty_string, time, cpu, usr, nice, sys, iowait, irq, soft, steal, guest, gnice, idle;
    //skip headers
    std::getline(cpu_stream, empty_string);
    std::getline(cpu_stream, empty_string);
    std::getline(cpu_stream, empty_string);

    double sum = 0.;
    std::size_t count = 0;
    double current_value = 0;
    while(true) {
        current_value = 0;
        cpu_stream >> time >> cpu >> usr >> nice >> sys >> iowait >> irq >> soft >> steal >> guest >> gnice >> idle;
        if(cpu_stream.eof()) {
            break;
        }

        boost::replace_all(idle, ",", ".");
        std::from_chars(idle.data(), idle.data() + idle.size(), current_value);
        if (current_value == 0) {
            return 0;
        }
        sum += max_percentage - current_value;
        ++count;
    }
    return sum / static_cast<double>(count);
}
