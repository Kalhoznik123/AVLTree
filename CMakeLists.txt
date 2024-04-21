cmake_minimum_required(VERSION 3.5)

project(AVLTree  VERSION 1.0.0)

find_package(Threads REQUIRED)
find_package(GTest)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AVLTree main.cpp AVLTree.h)


if(Threads_FOUND)
    target_link_libraries(AVLTree PRIVATE gtest gmock Threads::Threads)
else()
    target_link_libraries(AVLTree PRIVATE gtest gmock)
endif()





