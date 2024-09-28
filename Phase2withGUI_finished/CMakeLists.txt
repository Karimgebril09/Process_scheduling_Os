cmake_minimum_required(VERSION 3.27)
project(code C)

set(CMAKE_C_STANDARD 11)

add_executable(code process_generator.c
        process.c
        scheduler.c
        test_generator.c
        MinHeap.h
        clk.c
        Queue.h
        RR_Queue.h)
