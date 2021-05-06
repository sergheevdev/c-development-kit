cmake_minimum_required(VERSION 3.17)
project(src C)

set(CMAKE_C_STANDARD 11)

include_directories(core/strings/string-builder)

### Core ###

#### Strings ####

# string builder
add_executable(
        src
        core/strings/string-builder/string-builder.c
        core/strings/string-builder/string-builder.h
)

