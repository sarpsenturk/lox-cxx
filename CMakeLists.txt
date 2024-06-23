cmake_minimum_required(VERSION 3.21)

project(
        lox-cxx
        VERSION 0.1.0
        DESCRIPTION "A modern C++ implementation of the Lox programming language"
)

find_package(fmt CONFIG REQUIRED)

add_library(
        lox
        src/lox.h src/lox.cpp
)
target_compile_features(lox PUBLIC cxx_std_20)
target_include_directories(lox PUBLIC src)
target_link_libraries(lox PUBLIC fmt::fmt)

add_executable(lox-cxx src/main.cpp)
target_link_libraries(lox-cxx lox)

if (NOT PROJECT_IS_TOP_LEVEL)
    return()
endif ()

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(test)
endif ()
