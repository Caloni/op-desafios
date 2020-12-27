cmake_minimum_required(VERSION 3.16)

project (samples C CXX)

#set(Boost_USE_STATIC_LIBS ON) 
#set(Boost_USE_MULTITHREADED ON)
#find_package(Boost 1.72.0 EXACT REQUIRED COMPONENTS date_time filesystem system thread program_options regex iostreams)
find_package(Threads)

add_executable(samples)

add_compile_options(-pthread)

target_sources(samples PRIVATE main.cpp)

#target_include_directories(samples PRIVATE ${Boost_INCLUDE_DIRS})

#target_link_libraries(samples PRIVATE ${Boost_LIBRARIES})

target_link_libraries(samples PRIVATE ${CMAKE_THREAD_LIBS_INIT})
