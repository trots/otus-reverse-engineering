cmake_minimum_required(VERSION 3.20)
project(license_fuzz_project CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(license_core)
add_subdirectory(demo_app)
