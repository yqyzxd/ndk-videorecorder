cmake_minimum_required(VERSION 3.22.1)

set(name "handler")
project(${name})

add_library(
        ${name}

        STATIC

        handler.cpp
        loop_thread.cpp
        looper.cpp
        message_queue.cpp
        message.cpp

        ../../utils/thread.cpp
        )



target_link_libraries(
        ${name}

        log
        android

        )