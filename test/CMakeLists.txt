# Set up doctest
include(CTest)

# Add lizard as ctest target
find_program(LIZARD_COMMAND NAMES lizard)
if(LIZARD_COMMAND)
  add_test(NAME lizard COMMAND ${LIZARD_COMMAND} "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/test")
endif()

include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest
  GIT_TAG master # or any SHA, Branch or tag like v2.4.9
)
FetchContent_MakeAvailable(doctest)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

# Include doctest.h as -isystem to silence clang-tidy warnings
get_property(DOCTEST_INCLUDE_DIR TARGET doctest PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
include_directories(SYSTEM ${DOCTEST_INCLUDE_DIR})

# Building tests
add_executable(doctest_main)
target_sources(doctest_main
  PRIVATE
    doctest_main.cpp
    dtest.cpp
    test_life.cpp
)
target_include_directories(doctest_main
  PRIVATE
    "${CMAKE_SOURCE_DIR}/src/life/include"
)
target_link_libraries(doctest_main
  PRIVATE
    doctest::doctest
    life
)
doctest_discover_tests(doctest_main WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if("${COMPILE_WITH_COVERAGE}")
  setup_target_for_coverage_lcov(
      NAME coverage
      EXECUTABLE "${CMAKE_CTEST_COMMAND}" "--verbose"
      DEPENDENCIES
        doctest_main
      #EXCLUDE "/usr/*" "*/_deps/*" "test"
  )
endif()
