cmake_minimum_required(VERSION 2.8)
include (InstallRequiredSystemLibraries)
#set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
include(CPack)

project(cvrptw)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(CMakeBuildTypes)

SET(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_FLAGS "-g -ggdb -Wall -Wno-unknown-pragmas -fPIC -fexceptions -std=c++11 -fopenmp -lm -lpthread -DIL_STD")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native")

#set(Boost_NO_SYSTEM_PATHS ON)
find_package(Boost 1.50.0 COMPONENTS program_options)
find_package(cplex)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${CPLEX_INCLUDE_DIRS})

add_executable(cvrptw main.cpp Instance.cpp Model.cpp Solution.cpp ILS.cpp)
target_link_libraries(cvrptw ${Boost_LIBRARIES} ${CPLEX_LIBRARIES} rt)
