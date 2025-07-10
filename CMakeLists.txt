cmake_minimum_required(VERSION 3.15)
project(CAVLite)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Collect all .cpp and .h files
file(GLOB_RECURSE SOURCES "src/CAVLite/*.cpp")
file(GLOB_RECURSE HEADERS "src/CAVLite/*.h")

# Remove main.cpp from SOURCES
list(FILTER SOURCES EXCLUDE REGEX ".*/main\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX ".*/AgentInputFeeder\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX ".*/AgentInputFeeder\\.h$")

# Force runtime library to /MDd for Debug, /MD for Release
foreach(flag_var
        CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
    if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MDd" ${flag_var} "${${flag_var}}")
    endif()
endforeach()

# Create shared library
add_library(CAVLite SHARED ${SOURCES} ${HEADERS})

# Optional: Define macro to enable export
target_compile_definitions(CAVLite PRIVATE EXPORTING_DLL)
