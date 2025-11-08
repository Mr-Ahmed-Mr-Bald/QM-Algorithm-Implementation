cmake_minimum_required(VERSION 3.10)
project(QM_Algorithm_Implementation CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/utils)

# Source files
set(SOURCES
    src/main.cpp
    src/expression.cpp
    src/implicant.cpp
    src/qm-minimizer.cpp
    src/verilog-generator.cpp
    src/quine-mccluskey-driver.cpp
    utils/verilog_utils.cpp
)

# Header files (for IDE project generation)
set(HEADERS
    include/expression.h
    include/implicant.h
    include/qm-minimizer.h
    include/verilog-generator.h
    include/quine-mccluskey-driver.h
    utils/enums.utils.h
    utils/verilog_utils.h
)

# Create executable
add_executable(QM_Algorithm_Implementation ${SOURCES} ${HEADERS})

# Set output directory
set_target_properties(QM_Algorithm_Implementation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build
)

# Compiler warnings
if(MSVC)
    target_compile_options(QM_Algorithm_Implementation PRIVATE /W4)
else()
    target_compile_options(QM_Algorithm_Implementation PRIVATE -Wall -Wextra -pedantic)
endif()

# Print build information
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output Directory: ${PROJECT_SOURCE_DIR}/build")
