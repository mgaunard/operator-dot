cmake_minimum_required(VERSION 2.6)

find_package(Boost 1.56 COMPONENTS system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})

include_directories(../emulation)
add_definitions(-std=c++14)

file(GLOB files *.cpp)

foreach(file ${files})
    get_filename_component(file_ne ${file} NAME_WE)
    add_executable(${file_ne} ${file})
endforeach()
