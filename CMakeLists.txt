cmake_minimum_required(VERSION 3.30)
project(QM_Algorithm_Implementation C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(include)

add_executable(QM_Algorithm_Implementation
    include/expression.h
    include/qm-minimizer.h
)
