cmake_minimum_required(VERSION 3.15)
project(kord C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(kord
        src/main.c
        include/staticDefinitions.h
        include/kstring.h
        include/kvariant.h
        include/ktoken.h
        include/kfile.h
        include/kerror.h
        include/kast.h
        include/kmemory.h
)

target_include_directories(kord PRIVATE include)