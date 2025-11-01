cmake_minimum_required(VERSION 3.15)

project(server C)

# Set the C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Add source files
set(SOURCES
    src/main.c
    src/file_info.c
    src/mime_type.c
    src/response.c
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add warnings
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
