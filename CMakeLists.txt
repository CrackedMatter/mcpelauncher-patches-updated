cmake_minimum_required(VERSION 3.5)

project(mcpelauncherpatchesupdated)

set(CMAKE_CXX_STANDARD 17)

add_library(mcpelauncherpatchesupdated SHARED src/main.cpp)
target_link_libraries(mcpelauncherpatchesupdated PUBLIC)

install(TARGETS mcpelauncherpatchesupdated LIBRARY DESTINATION mods)
