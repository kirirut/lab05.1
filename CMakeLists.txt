cmake_minimum_required(VERSION 3.10)

# Set the project name
project(LAB05.1)

# Set C standard (adjust version if needed)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(includes)

# Source files
set(SOURCES
    src/main.c
    src/message.c
    src/queue.c
    src/consumer.c
    src/producer.c
)

# Create the executable
add_executable(lab05.1 ${SOURCES})

# Optional: if you need any libraries, link them here
# target_link_libraries(lab04 pthread)
