cmake_minimum_required(VERSION 3.10)

project(Clox C)

file(GLOB SOURCES "src/*.c")

add_executable(clox ${SOURCES})