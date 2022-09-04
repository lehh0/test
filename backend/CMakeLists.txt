cmake_minimum_required(VERSION 3.23)

project(backend)

# todo: dry includes

file(GLOB SOURCE
     "*.hpp"
     "*.cpp"
)

file(GLOB NET_SOURCE
     "net/*.hpp"
     "net/*.cpp"
     "net/*/*.hpp"
     "net/*/*.cpp"
)

file(GLOB BL_SOURCE
     "bl/*.hpp"
     "bl/*.cpp"
)

file(GLOB INCLUDE_SOURCE
     "include/*.hpp"
     "include/*.cpp"
)

message("src list is ${SOURCE}")
message("net src list is ${NET_SOURCE}")
message("bl src list is ${BL_SOURCE}")
message("include src list is ${INCLUDE_SOURCE}")

add_executable(backend ${SOURCE} ${NET_SOURCE} ${BL_SOURCE} ${INCLUDE_SOURCE})

target_include_directories(backend PRIVATE ${CONAN_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR})

find_package(Boost REQUIRED)

# todo: implement proper detection.
add_library(std::coroutines INTERFACE IMPORTED)
target_compile_options(std::coroutines INTERFACE -fcoroutines)

target_link_libraries(backend PUBLIC 
	std::coroutines
    Boost::headers
	${CONAN_LIBS})
