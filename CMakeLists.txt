cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
 
project(random_ranges)

# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -polly -mtune=native -std=c++14 -Wall")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

include_directories(sparsepp)


add_executable(random_ranges
	random_ranges.cpp
	)

target_link_libraries(random_ranges
	boost_program_options
	)

