cmake_minimum_required(VERSION 3.23)
project(test_work_server_test)

set(CMAKE_CXX_STANDARD 20)
set(path_to_conan_directory "./../conan")

include(${path_to_conan_directory}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(test_work_server_test
        main.cpp
        test_dump.cpp
        response_test.cpp
        ../src/source/config_manager.cpp
        ../src/source/logger_wrapper.cpp
        ../src/source/api_handlers.cpp
        ../src/source/response_template_helper.cpp
        )

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
target_link_libraries(test_work_server_test ${CONAN_LIBS})

enable_testing()