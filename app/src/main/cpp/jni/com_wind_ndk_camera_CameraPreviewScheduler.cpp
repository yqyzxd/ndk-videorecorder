//
// Created by 史浩 on 2023/5/5.
//
#include "com_wind_ndk_camera_CameraPreviewScheduler.h"
#include "../camera_preview/camera_preview_controller.h"
#include <jni.h>
#include <android/native_window_jni.h>


static CameraPreviewController* controller=0;
JNIEXPORT void JNICALL
Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeOnSurfaceCreated(JNIEnv* env,jobject jobj, jobject surface,jint cameraFacingId) {
    jobject jGlobalRef=env->NewGlobalRef(jobj);
    JavaVM *javaVM = NULL;
    env->GetJavaVM(&javaVM);
    controller=new CameraPreviewController(javaVM,jGlobalRef,cameraFacingId);
    ANativeWindow* window=ANativeWindow_fromSurface(env,surface);
    controller->onSurfaceCreated(window);
}


JNIEXPORT void JNICALL
Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeOnSurfaceChanged(JNIEnv* env,jobject thiz, jint width, jint height) {
    if (controller){
        controller->onSurfaceChanged(width,height);
    }
}


JNIEXPORT void JNICALL
Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeOnSurfaceDestroyed(JNIEnv* env,jobject thiz) {
    if (controller){
        controller->onSurfaceDestroyed();
        delete controller;
        controller= nullptr;
    }
}

JNIEXPORT void JNICALL
Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeOnFrameAvailable(JNIEnv *env, jobject thiz) {
    if (controller){
        controller->onFrameAvailable();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeSwitchCamera(JNIEnv *env, jobject thiz,jint cameraFacingId) {
    if (controller){
        controller->switchCamera(cameraFacingId);
    }
}

JNIEXPORT void JNICALL
Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeRelease(JNIEnv *env, jobject thiz) {
    /*if (controller){
        delete controller;
        controller= nullptr;
    }*/
}
JNIEXPORT void JNICALL Java_com_wind_ndk_camera_CameraPreviewScheduler_startEncode
        (JNIEnv *env, jobject,  jint width, jint height,  jint videoBitrate, jint frameRate){
    if (controller){

        controller->startEncode(width,height,videoBitrate,frameRate);
    }
}

JNIEXPORT void JNICALL Java_com_wind_ndk_camera_CameraPreviewScheduler_stopEncode
        (JNIEnv *, jobject){
    if (controller){
        controller->stopEncode();
    }
}