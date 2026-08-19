cmake_minimum_required(VERSION 3.15)
project(EndCommon)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(
    GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS 
    "src/*.hpp"
    "src/*.h"
    "src/*.cpp"  
)

add_library(EndCommon STATIC)

target_sources(EndCommon PRIVATE ${ALL_SOURCES})

target_include_directories(EndCommon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(WIN32)
    target_compile_definitions(EndCommon PUBLIC UNICODE _UNICODE)
    
    if(MSVC)
        target_compile_options(EndCommon PUBLIC /utf-8)
    endif()
endif()