cmake_minimum_required(VERSION 3.21...4.0)

project(mcpelauncherpatchesupdated)

set(CPM_SOURCE_CACHE ${PROJECT_SOURCE_DIR}/cpm-cache CACHE PATH "")
include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 23)

add_library(mcpelauncherpatchesupdated SHARED src/main.cpp)

CPMAddPackage("gh:BasedInc/libhat@0.6.0")
target_link_libraries(mcpelauncherpatchesupdated PRIVATE log libhat::libhat)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(mcpelauncherpatchesupdated PRIVATE -flto)
    target_link_options(mcpelauncherpatchesupdated PRIVATE
        -flto -Wl,--version-script,${PROJECT_SOURCE_DIR}/map.txt -Wl,--no-undefined-version)
    set_target_properties(mcpelauncherpatchesupdated PROPERTIES
        LINK_DEPENDS ${PROJECT_SOURCE_DIR}/map.txt)
endif()

install(TARGETS mcpelauncherpatchesupdated LIBRARY DESTINATION mods)
