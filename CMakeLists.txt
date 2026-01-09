cmake_minimum_required(VERSION 3.20)

project(ImCloneDetect LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all .cpp files in implementation/
file(GLOB IMPL_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/implementation/*.cpp
)

add_executable(ImCloneDetect
    main/main.cpp
    ${IMPL_SOURCES}
)

add_executable(test
    main/test.cpp
    ${IMPL_SOURCES}
)

# Tell compiler where to find headers
target_include_directories(ImCloneDetect PRIVATE
    ${CMAKE_SOURCE_DIR}/headers
)

target_include_directories(test PRIVATE
    ${CMAKE_SOURCE_DIR}/headers
)
