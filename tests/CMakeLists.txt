FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1
)

FetchContent_MakeAvailable(Catch2)

add_executable(${PROJECT_NAME}_tests 
main.cpp
filesystem_tests.cpp)

target_include_directories(${PROJECT_NAME}_tests PRIVATE ${SOURCE_DIR} ${TERMIJA_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}_tests PRIVATE Catch2::Catch2WithMain termija raylib ${PROJECT_NAME}lib )
