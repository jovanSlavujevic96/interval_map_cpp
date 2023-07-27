cmake_minimum_required(VERSION 3.5)

project (interval_map)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package (Threads)

add_executable(interval_map_test
    main.cc
)
target_link_libraries (interval_map_test ${CMAKE_THREAD_LIBS_INIT})
