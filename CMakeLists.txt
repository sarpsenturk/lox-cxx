cmake_minimum_required(VERSION 3.21)

project(
        lox-cxx
        VERSION 0.1.0
        DESCRIPTION "A modern C++ implementation of the Lox programming language"
)

add_library(
        lox
        src/lox.h src/lox.cpp
        src/ast.h src/ast.cpp
        src/environment.h src/environment.cpp
        src/error.h src/error.cpp
        src/lexer.h src/lexer.cpp
        src/lox_boolean.h src/lox_boolean.cpp
        src/lox_nil.h src/lox_nil.cpp
        src/lox_number.h src/lox_number.cpp
        src/lox_object.h src/lox_object.cpp
        src/lox_string.h src/lox_string.cpp
        src/parser.h src/parser.cpp
        src/source_location.h src/source_location.cpp
        src/token.h src/token.cpp
        src/tree_walk.h src/tree_walk.cpp
)
target_compile_features(lox PUBLIC cxx_std_20)
target_include_directories(lox PUBLIC src)

find_package(fmt CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)
target_link_libraries(lox PUBLIC fmt::fmt tl::expected)

add_executable(lox-cxx src/main.cpp)
target_link_libraries(lox-cxx lox)

if (NOT PROJECT_IS_TOP_LEVEL)
    return()
endif ()

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(test)
endif ()
