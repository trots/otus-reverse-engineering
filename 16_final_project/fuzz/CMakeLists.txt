cmake_minimum_required(VERSION 3.20)
project(license_fuzz CXX)

set(CMAKE_CXX_LINK_EXECUTABLE
    "<CMAKE_CXX_COMPILER> <FLAGS> <OBJECTS> -o <TARGET> -fuse-ld=lld -link <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES>")

set(CORE_DIR ../license_core)

add_executable(fuzz_harness
    harness.cpp
    ${CORE_DIR}/license_core.cpp
)

target_include_directories(fuzz_harness PRIVATE ${CORE_DIR})

target_compile_options(fuzz_harness PRIVATE -fsanitize=fuzzer,address)
target_link_options(fuzz_harness PRIVATE -fsanitize=fuzzer,address)
