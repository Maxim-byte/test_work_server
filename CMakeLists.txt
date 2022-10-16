cmake_minimum_required(VERSION 3.23)
project(test_work_server)

set(CMAKE_CXX_STANDARD 20)
set(path_to_conan_directory "./conan")

include(${path_to_conan_directory}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(test_work_server
        src/main.cpp
        src/source/config_manager.cpp
        src/source/logger_wrapper.cpp
        src/source/tcp_session.cpp
        src/source/response_template_helper.cpp
        src/source/api_handlers.cpp
        src/source/server.cpp
        src/include/dump_manager.hpp
        src/include/unique_container.hpp
        )

target_link_libraries(test_work_server ${CONAN_LIBS})