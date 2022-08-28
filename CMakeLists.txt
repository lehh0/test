cmake_minimum_required(VERSION 3.23)

project(beast-sandbox)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_CXX_STANDARD 20)

#add_executable(test test.cpp do_listen.cpp do_session.cpp)

target_include_directories(test PRIVATE ${CONAN_INCLUDE_DIRS})

target_link_libraries(test PRIVATE ${CONAN_LIBS})
