cmake_policy(SET CMP0054 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

file(GLOB TEST_SOURCES "unit/*.cpp")
include_directories(${CMAKE_SOURCE_DIR}/src)

foreach(test_file IN LISTS TEST_SOURCES)
	get_filename_component(test_name ${test_file} NAME_WE)

	add_executable(${test_name} ${test_file})
	target_link_libraries(${test_name} gtest gtest_main)

	gtest_discover_tests(${test_name})
endforeach()
