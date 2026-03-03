# Collect source files recursively
file(GLOB_RECURSE SOURCES 
    "runtime/*.c"
    "parser/*.c"
    "*.c"
)

list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.c")

# Create the core library (libnucleus)
add_library(nucleus_core ${SOURCES})

# Create the executable (CLI tool)
add_executable(c-lisp main.c)

# Link the core library
target_link_libraries(c-lisp PRIVATE nucleus_core)