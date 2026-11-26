cmake_minimum_required(VERSION 3.20)

project(MacroTools
    VERSION 1.0.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(macrotools_distance_calculator STATIC
    src/Coord-GeometryTools/distanceCalculator.c
)

add_library(MacroTools::DistanceCalculator
    ALIAS macrotools_distance_calculator
)

target_include_directories(macrotools_distance_calculator
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Coord-GeometryTools"
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_link_libraries(macrotools_distance_calculator
        PUBLIC m
    )
endif()

if(MSVC)
    target_compile_options(macrotools_distance_calculator PRIVATE /W4)
else()
    target_compile_options(macrotools_distance_calculator
        PRIVATE -Wall -Wextra -Wpedantic
    )
endif()