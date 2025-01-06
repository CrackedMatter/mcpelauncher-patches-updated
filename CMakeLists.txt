cmake_minimum_required(VERSION 3.12)

project(mcpelauncherpatchesupdated)

set(CMAKE_CXX_STANDARD 20)

add_library(mcpelauncherpatchesupdated SHARED src/main.cpp)

target_link_libraries(mcpelauncherpatchesupdated PRIVATE log)

install(TARGETS mcpelauncherpatchesupdated LIBRARY DESTINATION mods)
