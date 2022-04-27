cmake_minimum_required(VERSION 3.17)

project(mdissect)

add_library(${PROJECT_NAME} STATIC
    src/mdissect.cpp
    )

target_include_directories(${PROJECT_NAME} PUBLIC include)