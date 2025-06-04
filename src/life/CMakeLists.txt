# Library
add_library(life)
target_sources(life
  PRIVATE
    life.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
      include/life.h
)

# Link library with another library
#find_library(BLUB_LIB blub)
#target_link_libraries(life PUBLIC ${BLUB_LIB})
#target_compile_definitions(life PUBLIC ${USE_BLUB})
target_include_directories(life PUBLIC ${CMAKE_CURRENT_LIST_DIR})
if("${COMPILE_WITH_COVERAGE}")
  append_coverage_compiler_flags_to_target(life)
endif()
