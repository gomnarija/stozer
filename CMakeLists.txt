cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(stozer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(TERMIJA_SOURCE_DIR ${CMAKE_SOURCE_DIR}/termija/src)



#include termija
add_subdirectory(termija termija_build)
add_subdirectory(${SOURCE_DIR} build)


add_executable(stozer ${SOURCE_FILES})
target_include_directories(stozer PRIVATE ${SOURCE_DIR} ${TERMIJA_SOURCE_DIR})
target_link_libraries(stozer PRIVATE termija raylib plog)