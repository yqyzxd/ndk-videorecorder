cmake_minimum_required(VERSION 3.22.1)
set(NAME "yuv")
project(${NAME})

# 把当前目录下所有得文件 变成一个 SOURCE变量表示
include_directories(./include)
aux_source_directory(./source SOURCE)

add_library(
        ${NAME}

        STATIC

        ${SOURCE}
        )

