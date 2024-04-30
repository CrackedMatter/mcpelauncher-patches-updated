cmake_minimum_required(VERSION 3.5)

project(mcpelauncherpatchesupdated)

set(CMAKE_CXX_STANDARD 20)

add_library(mcpelauncherpatchesupdated SHARED src/main.cpp src/sig.cpp)
target_link_libraries(mcpelauncherpatchesupdated PUBLIC log)

install(TARGETS mcpelauncherpatchesupdated LIBRARY DESTINATION mods)
