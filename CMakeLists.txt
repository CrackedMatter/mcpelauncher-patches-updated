cmake_minimum_required(VERSION 3.5)

project(mcpelauncherpatchesupdated)

set(CMAKE_CXX_STANDARD 17)

set(MC_VERSION "1021040" CACHE STRING "Minecraft version")

add_library(mcpelauncherpatchesupdated SHARED src/main.cpp)

target_compile_definitions(mcpelauncherpatchesupdated PRIVATE "MC_VERSION=${MC_VERSION}")

install(TARGETS mcpelauncherpatchesupdated LIBRARY DESTINATION mods)
