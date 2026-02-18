cmake_minimum_required(VERSION 3.10)

# Project information
project(C-lisp VERSION 1.0 LANGUAGES C)

# Enforce C99 standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Compiler Flags Fix ---
if(MSVC)
    # Flags for Microsoft Visual C++
    # /W4 : Warning level 4 (Equivalent to -Wall -Wextra)
    # _CRT_SECURE_NO_WARNINGS : Disable warnings about "unsafe" functions like strcpy/strdup
    add_compile_options(/W4)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    # Flags for GCC / Clang
    add_compile_options(-Wall -Wextra -g)
endif()

# Include directories
include_directories(src)

# Add source directory
add_subdirectory(src)