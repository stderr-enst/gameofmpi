add_executable(main main.cpp)
target_include_directories(main PRIVATE
    ${MPI_CXX_INCLUDE_DIRS}
    "${CMAKE_SOURCE_DIR}/src/life/include"
)
target_link_libraries(main PRIVATE
  MPI::MPI_CXX
  life
)
if("${COMPILE_WITH_COVERAGE}")
  append_coverage_compiler_flags_to_target(main)
endif()


install(TARGETS main DESTINATION bin)
