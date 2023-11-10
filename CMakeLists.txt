cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(stozer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(TERMIJA_SOURCE_DIR ${CMAKE_SOURCE_DIR}/termija/src)
set(MOMO_SOURCE_DIR ${CMAKE_SOURCE_DIR}/momo/src)



#include termija
add_subdirectory(termija termija_build)
add_subdirectory(${SOURCE_DIR} build)

#include momo
add_subdirectory(momo momo_build)

#lib
add_library(${PROJECT_NAME}lib ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}lib PRIVATE 
${SOURCE_DIR} 
${SOURCE_DIR}/userspace 
${SOURCE_DIR}/userspace/fs 
${SOURCE_DIR}/userspace/uredi
${SOURCE_DIR}/userspace/izvrsi 
${SOURCE_DIR}/userspace/posta
${SOURCE_DIR}/userspace/fakture
${TERMIJA_SOURCE_DIR}
${MOMO_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}lib PRIVATE termija raylib plog)


#exe
add_executable(${PROJECT_NAME} ${SOURCE_DIR}/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE 
${SOURCE_DIR} 
${SOURCE_DIR}/userspace 
${SOURCE_DIR}/userspace/fs 
${SOURCE_DIR}/userspace/uredi 
${SOURCE_DIR}/userspace/izvrsi 
${SOURCE_DIR}/userspace/posta
${SOURCE_DIR}/userspace/fakture
${TERMIJA_SOURCE_DIR}
${MOMO_SOURCE_DIR})
target_link_libraries(stozer PRIVATE ${PROJECT_NAME}lib termija momo raylib plog)

#tests
add_subdirectory(tests)


# add_executable(tmp tmp/tmp.cpp)
# target_include_directories(tmp PRIVATE ${SOURCE_DIR} ${SOURCE_DIR}/userspace ${SOURCE_DIR}/userspace/fs ${TERMIJA_SOURCE_DIR})
# target_link_libraries(tmp PRIVATE ${PROJECT_NAME}lib termija raylib plog)