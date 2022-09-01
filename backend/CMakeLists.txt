cmake_minimum_required(VERSION 3.23)

project(backend)

file(GLOB SOURCE
     "*.h"
     "*.cpp"
)

file(GLOB NET_SOURCE
     "net/*/*.h"
     "net/*/*.cpp"
)


message("src list is ${SOURCE}")
message("net src list is ${NET_SOURCE}")

add_executable(backend ${SOURCE} ${NET_SOURCE})

target_include_directories(backend PRIVATE ${CONAN_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(backend PRIVATE ${CONAN_LIBS})
