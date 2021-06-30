cmake_minimum_required(VERSION 3.12)
project(kodo_python)

find_package(Python REQUIRED COMPONENTS Interpreter Development)
# Use waf to resolve dependencies
if(NOT DEFINED STEINWURF_RESOLVE)
  message(STATUS "Resolving dependencies...")
  execute_process(
    COMMAND ${Python_EXECUTABLE} waf resolve ${STEINWURF_RESOLVE_OPTIONS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE STATUS)
  if(STATUS AND NOT STATUS EQUAL 0)
    message(FATAL_ERROR "Failed: ${STATUS}")
  endif()
  set(STEINWURF_RESOLVE "${CMAKE_CURRENT_SOURCE_DIR}/resolve_symlinks")
  set(STEINWURF_TOP_NAME kodo_python)
endif()

# Use position independent code in this library and all dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# kodo dependency
if(NOT TARGET steinwurf::kodo)
  add_subdirectory("${STEINWURF_RESOLVE}/kodo" kodo EXCLUDE_FROM_ALL)
endif()

# pybind11 dependency
add_subdirectory("${STEINWURF_RESOLVE}/pybind11-source")

# Define library
file(GLOB_RECURSE kodo_python_sources ./src/**/*.cpp)
pybind11_add_module(kodo_python ${kodo_python_sources})
set_target_properties(kodo_python PROPERTIES OUTPUT_NAME kodo)

# Get all steinwurf object libraries and link directly with them.
get_property(steinwurf_object_libraries GLOBAL
             PROPERTY steinwurf::object_libraries)
target_link_libraries(kodo_python PRIVATE ${steinwurf_object_libraries})

# Install library
install(FILES $<TARGET_FILE:kodo_python>
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
