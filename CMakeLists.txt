cmake_minimum_required(VERSION 3.22)
project(MDSC VERSION 0.1.0 LANGUAGES C CXX)

option(USE_CUDA "Build with CUDA acceleration" OFF)
option(BUILD_TESTS "Build unit tests" ON)

option(MDSC_ENABLE_DEBUG_SYMBOLS "Add -g3" OFF)
option(MDSC_ENABLE_PROFILING "Add -pg (gprof)" OFF)
option(MDSC_ENABLE_SANITIZERS "Enable Address/UB sanitizers (Debug only)" OFF)
option(MDSC_NATIVE_OPT "Enable -march=native" OFF)
option(MDSC_LTO "Enable link-time optimization" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

# Compiler flags
if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  if (MDSC_ENABLE_DEBUG_SYMBOLS)
    add_compile_options(-g3)
  endif()
  if (MDSC_ENABLE_PROFILING)
    add_compile_options(-pg)
    add_link_options(-pg)
  endif()
  if (MDSC_NATIVE_OPT)
    add_compile_options(-march=native)
  endif()
  if (MDSC_LTO AND CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_ok OUTPUT ipo_err)
    if (ipo_ok)
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
  endif()
  if (MDSC_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
  endif()
endif()

# Library (flat src files)
add_library(mdsc
  src/vec.c
  src/system.c
  src/neigh_verlet.c
  src/forces_lj.c
  src/integrator_vv.c
  src/thermostat.c
  src/io_and_parse.c
)

target_include_directories(mdsc PUBLIC include)

# Optional CUDA
if (USE_CUDA)
  enable_language(CUDA)
  add_library(mdsc_cuda src/lj_cuda.cu)
  target_include_directories(mdsc_cuda PUBLIC include)
  set_target_properties(mdsc_cuda PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
  target_link_libraries(mdsc PUBLIC mdsc_cuda)
endif()

# Executable
add_executable(mdsc_bin src/main.c)
set_target_properties(mdsc_bin PROPERTIES OUTPUT_NAME "mdsc")
target_link_libraries(mdsc_bin PRIVATE mdsc)

# Tests
if (BUILD_TESTS)
  enable_testing()
  add_executable(test_vec tests/test_vec.c third_party/unity/unity.c)
  target_link_libraries(test_vec PRIVATE mdsc)
  add_test(NAME test_vec COMMAND test_vec)

  add_executable(test_lj tests/test_lj.c third_party/unity/unity.c)
  target_link_libraries(test_lj PRIVATE mdsc)
  add_test(NAME test_lj COMMAND test_lj)
endif()

# Install
include(GNUInstallDirs)
install(TARGETS mdsc mdsc_bin
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Packaging hints
set(CPACK_PACKAGE_NAME "MDSC")
include(CPack)
