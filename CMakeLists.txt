cmake_minimum_required(VERSION 3.26)
project(icon-changer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_executable(icon-changer)

file(GLOB SOURCES "src/*.cpp")
target_sources(icon-changer PRIVATE ${SOURCES})

if(BUILD_TESTS)
	add_subdirectory(tests)
else()
	message(STATUS "Tests are disabled. To enable them, pass -DBUILD_TESTS=ON")
endif()
