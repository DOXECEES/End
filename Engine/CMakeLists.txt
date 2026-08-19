cmake_minimum_required(VERSION 3.15)
project(EndEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(
    GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/*.hpp"
)

add_executable(EndEngine WIN32 ${ALL_SOURCES})

target_link_libraries(EndEngine PRIVATE EndGui)

if(WIN32)
    target_compile_definitions(EndEngine PRIVATE UNICODE _UNICODE)
    
    if(MSVC)
        target_compile_options(EndEngine PRIVATE /utf-8)
        target_link_options(EndEngine PRIVATE "/ENTRY:mainCRTStartup")

    endif()
endif()