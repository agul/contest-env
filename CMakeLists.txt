cmake_minimum_required(VERSION 2.8.4)
project(contests)

add_executable(output output/main.cpp)

add_subdirectory(algolib/jngen)
add_executable(testrunner testrunner/main.cpp)

# set(USE_JNGEN ON)
if(USE_JNGEN)
    target_link_libraries(testrunner jngen)
    target_compile_options(testrunner PRIVATE -DJNGEN_DECLARE_ONLY -DUSE_JNGEN)
endif()


file(GLOB TASKS "tasks/*.cpp")
file(GLOB_RECURSE HEADERS "algolib/cpplib/*.hpp" )

include_directories(algolib)
include_directories(algolib/cpplib)

ADD_DEFINITIONS(-std=c++14 -O3)
ADD_DEFINITIONS(-DHOME)

add_executable(test ${TASKS} ${HEADERS})

add_subdirectory(algolib)