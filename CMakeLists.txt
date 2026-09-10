cmake_minimum_required(VERSION 3.10)
project(smartnic_sim C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(smartnic_sim main.c ring_buffer.c dma_engine.c checksum_engine.c)

target_link_libraries(smartnic_sim pthread)