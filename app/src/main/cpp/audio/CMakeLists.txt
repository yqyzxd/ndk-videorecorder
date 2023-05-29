cmake_minimum_required(VERSION 3.22.1)
set(NAME "audio_recorder")
project(${NAME})

add_library(
        NAME
        SHARED

        com_wind_ndk_audio_recorder_AudioEncoder.cpp
        audio_encoder.cpp
)