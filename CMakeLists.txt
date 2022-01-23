cmake_minimum_required(VERSION 3.14)
project(os_course)

include(cmake/CompileOptions.cmake)
include(cmake/Helpers.cmake)
include(cmake/Sanitize.cmake)
include(cmake/Task.cmake)

add_subdirectory(intro)
add_subdirectory(memory)
add_subdirectory(scheduling)
add_subdirectory(synchronization)
add_subdirectory(userspace)
