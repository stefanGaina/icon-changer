cmake_minimum_required(VERSION 3.26)
project(icon-changer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_compile_options(-Wno-character-conversion)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(icon_changer_lib STATIC ${SOURCES})
target_include_directories(icon_changer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(icon-changer "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(icon-changer PRIVATE icon_changer_lib)

if(MSVC OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-D_WIN32 -D_WINDOWS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fms-extensions")
endif()


target_link_libraries(icon-changer PRIVATE gdiplus)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "Tests are disabled. To enable them, pass -DBUILD_TESTS=ON")
endif()