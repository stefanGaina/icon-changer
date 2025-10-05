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

add_compile_options(-fprofile-instr-generate -fcoverage-mapping -O0 -g)

foreach(test_file IN LISTS TEST_SOURCES)
	get_filename_component(test_name ${test_file} NAME_WE)

	add_executable(${test_name} ${test_file})
	target_link_libraries(${test_name} gtest gmock gtest_main)
	target_compile_definitions(${test_name} PRIVATE TEST_DATA_PATH="${CMAKE_SOURCE_DIR}/tests/data/")

	gtest_discover_tests(${test_name})
endforeach()

add_custom_target(clean_coverage
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/tests/coverage
	COMMENT "Cleaning up the coverage directory"
)

add_custom_target(run_tests
	COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/tests/coverage/${test_name}-%p.profraw
		ctest --output-on-failure --test-dir ${CMAKE_BINARY_DIR}/tests
	COMMENT "Running tests to generate coverage data"
	DEPENDS clean_coverage
)

add_custom_target(merge_coverage
	COMMAND llvm-profdata merge -sparse ${CMAKE_BINARY_DIR}/tests/coverage/*.profraw -o ${CMAKE_BINARY_DIR}/tests/coverage/merged.profdata
	COMMENT "Merging coverage data"
	DEPENDS run_tests
)

add_custom_target(coverage_report
	COMMAND llvm-cov show ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/icon_test.exe ${SOURCES}
		--instr-profile=${CMAKE_BINARY_DIR}/tests/coverage/merged.profdata -format=html
		-output-dir=${CMAKE_BINARY_DIR}/tests/coverage_report -show-branches=percent
	COMMENT "Generating coverage report"
	DEPENDS merge_coverage
)
