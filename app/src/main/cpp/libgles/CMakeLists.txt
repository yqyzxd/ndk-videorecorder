cmake_minimum_required(VERSION 3.22.1)

project("eglcore")

add_library(
        eglcore

        STATIC

        egl/egl_core.cpp
        egl/base_egl_surface.cpp
        egl/window_surface.cpp
        egl/offscreen_surface.cpp
        egl/gl_surface.cpp
        egl/gl_offscreen_surface.cpp
        gles/base_filter.cpp
        gles/gl_utils.cpp
        gles/texture.cpp
        gles/base_fbo_filter.cpp
        gles/rgb_to_yuy2_filter.cpp


)

target_link_libraries(
        eglcore
        # Links the target library to the log library
        # included in the NDK.
        log
        android
        GLESv2
        EGL)