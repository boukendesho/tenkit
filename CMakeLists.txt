cmake_minimum_required(VERSION 3.31)

project(TenKit VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER clang++)

find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

add_executable(tenkit src/main.cpp)

target_link_libraries(tenkit PRIVATE cpr::cpr nlohmann_json::nlohmann_json CLI11::CLI11)

set_property(TARGET tenkit PROPERTY CXX_STANDARD 23)