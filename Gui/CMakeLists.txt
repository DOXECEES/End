cmake_minimum_required(VERSION 3.15)
project(EndGui)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(
    GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/*.hpp"
)

add_library(EndGui STATIC ${ALL_SOURCES})

target_include_directories(EndGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(EndGui PUBLIC EndCommon)


if(WIN32)
    target_compile_definitions(EndGui PRIVATE UNICODE _UNICODE)
    
    if(MSVC)
        target_compile_options(EndGui PRIVATE /utf-8)
    endif()
endif()