cmake_minimum_required(VERSION 3.22.1)
set(NAME "video_recorder")
project(${NAME})


set(THIRD_PARTY "${CMAKE_SOURCE_DIR}/thirdparty")
set(FFMPEG_LIBS "${THIRD_PARTY}/prebuilt/${CMAKE_ANDROID_ARCH_ABI}/")
include_directories(${THIRD_PARTY}/ffmpeg/include/)
#链接ffmpeng
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -L${FFMPEG_LIBS}")


include_directories(libs/handler)
add_subdirectory(libs/handler)

include_directories(libgles)
add_subdirectory(libgles)

include_directories(libs/lock)
add_subdirectory(libs/lock)


include_directories(libyuv/include)
add_subdirectory(libyuv)


add_library( # Sets the name of the library.
        ${NAME}

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        camera_preview/com_wind_ndk_camera_CameraPreviewScheduler.cpp
        camera_preview/camera_preview_controller.cpp
        camera_preview/camera_filter.cpp
        camera_preview/preview_renderer.cpp
        camera_preview/screen_filter.cpp

        camera_preview/camera_preview_scheduler_caller.cpp
        camera_preview/camera_info.cpp

        encoder/video_encoder_adapter.cpp
        encoder/soft/soft_video_encoder_adapter.cpp
        encoder/soft/x264_encoder.cpp
        encoder/soft/copy_texture_renderer.cpp

        encoder/soft/x264_parser.cpp

        video_packet_pool.cpp

        com_wind_ndk_camera_VideoRecorder.cpp
        video_consumer.cpp
        video_publisher.cpp

        audio/audio_encoder.cpp
        audio/com_wind_ndk_audio_recorder_PcmCollector.cpp
        audio/pcm_collector.cpp
        audio/audio_encoder_adapter.cpp
        audio/audio_process_encoder_adapter.cpp
        audio/merger/music_merger.cpp

        audio/player/com_wind_ndk_audio_player_AudioTrackPlayer.cpp
        audio/player/audio_track_player.cpp
        audio/player/audio_track_caller.cpp
        audio/player/audio_decoder.cpp

        utils/jni_caller.cpp

        )


target_link_libraries( # Specifies the target library.
        ${NAME}

        # Links the target library to the log library
        # included in the NDK.
        log
        android
        z

        handler
        lock
        eglcore

        fdk-aac
        x264
        avcodec
        avfilter
        avformat
        avutil
        swresample
        swscale

        yuv
        )