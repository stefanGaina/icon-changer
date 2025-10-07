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
include_directories(mocks)

add_compile_options(-fprofile-instr-generate -fcoverage-mapping -O0 -g)

set(TEST_OBJECT_ARGS "")

foreach(test_file IN LISTS TEST_SOURCES)
	get_filename_component(test_name ${test_file} NAME_WE)

	add_executable(${test_name} ${test_file})
	target_link_libraries(${test_name} gtest gmock gtest_main)
	target_compile_definitions(${test_name} PRIVATE TEST_DATA_PATH="${CMAKE_SOURCE_DIR}/tests/data/")

	gtest_discover_tests(${test_name})
	list(APPEND TEST_OBJECT_ARGS -object=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${test_name}$<TARGET_FILE_SUFFIX:${test_name}>)
endforeach()

add_custom_target(clean_coverage
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/tests/coverage
)

add_custom_target(run_tests
	COMMAND ${CMAKE_COMMAND}
		-E env LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/tests/coverage/%p.profraw
		ctest --output-on-failure
	DEPENDS clean_coverage
)

add_custom_target(merge_coverage
	COMMAND llvm-profdata merge
		-sparse ${CMAKE_BINARY_DIR}/tests/coverage/*.profraw
		-o ${CMAKE_BINARY_DIR}/tests/coverage/merged.profdata
	DEPENDS run_tests
)

add_custom_target(coverage_report
	COMMAND_EXPAND_LISTS
	COMMAND llvm-cov show
		${TEST_OBJECT_ARGS}
		--sources ${SOURCES}
		--instr-profile=${CMAKE_BINARY_DIR}/tests/coverage/merged.profdata
		--format=html
		--output-dir=${CMAKE_BINARY_DIR}/tests/coverage_report
		--show-branches=count
		--tab-size=4
	DEPENDS merge_coverage
)
