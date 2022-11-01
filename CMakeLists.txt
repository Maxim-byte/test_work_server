cmake_minimum_required(VERSION 3.22)
project(test_work_server)

set(CMAKE_CXX_STANDARD 20)
set(path_to_conan_directory "./conan")

include(${path_to_conan_directory}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(test_work_server
        src/main.cpp
        src/source/config_manager.cpp
        src/source/tcp_session.cpp
        src/source/response_template_helper.cpp
        src/source/api_handlers.cpp
        src/source/server.cpp
        src/source/prometheus_details.cpp
        src/source/prometheus_manager.cpp
        src/source/logger_group.cpp
        src/source/rpc_handlers.cpp
        src/include/rpc_handlers.hpp
        src/include/logger_group.hpp
        src/include/prometheus_manager.hpp
        src/include/prometheus_details.hpp
        src/include/dump_manager.hpp
        src/include/unique_container.hpp
        )

add_subdirectory(test)

target_precompile_headers(test_work_server PRIVATE src/precompiled.hpp)

target_link_libraries(test_work_server ${CONAN_LIBS})

enable_testing()