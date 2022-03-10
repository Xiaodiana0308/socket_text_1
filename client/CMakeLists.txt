project(MYSWAP)

cmake_minimum_required(VERSION 3.16)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/inc/*.h)

add_executable(main client.cpp ${sources} ${headers})