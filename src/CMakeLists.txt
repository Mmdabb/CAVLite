cmake_minimum_required(VERSION 3.10)
project(CAVLite)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)  # For MSVC DLL symbol export

# Define source directory
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# List all source files
set(SOURCES
    ${SRC_DIR}/SimulationRunner.cpp
    ${SRC_DIR}/Simulation.cpp
    ${SRC_DIR}/Network.cpp
    ${SRC_DIR}/CACF.cpp
    ${SRC_DIR}/DataLoader.cpp
    ${SRC_DIR}/AgentInputFeeder.cpp
    ${SRC_DIR}/CSVParser.cpp
    ${SRC_DIR}/StopProgram.cpp
    # ${SRC_DIR}/SimulationAPI.cpp  # Optional
)

add_library(CAVLite SHARED
    DataLoader.cpp
    Simulation.cpp
    Network.cpp
    CACF.cpp
    AgentInputFeeder.cpp
    CSVParser.cpp
    StopProgram.cpp
    # and headers if needed
)

target_include_directories(CAVLite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_include_directories(CAVLite PUBLIC ${SRC_DIR})
