/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_wind_ndk_camera_CameraPreviewScheduler */

#ifndef _Included_com_wind_ndk_camera_CameraPreviewScheduler
#define _Included_com_wind_ndk_camera_CameraPreviewScheduler
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_wind_ndk_camera_CameraPreviewScheduler
 * Method:    nativeOnSurfaceCreated
 * Signature: (Landroid/view/Surface;I)V
 */
JNIEXPORT void JNICALL Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeOnSurfaceCreated
  (JNIEnv *, jobject, jobject, jint);

/*
 * Class:     com_wind_ndk_camera_CameraPreviewScheduler
 * Method:    nativeOnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeOnSurfaceChanged
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_wind_ndk_camera_CameraPreviewScheduler
 * Method:    nativeOnSurfaceDestroyed
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeOnSurfaceDestroyed
  (JNIEnv *, jobject);

/*
 * Class:     com_wind_ndk_camera_CameraPreviewScheduler
 * Method:    nativeOnFrameAvailable
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeOnFrameAvailable
  (JNIEnv *, jobject);

/*
 * Class:     com_wind_ndk_camera_CameraPreviewScheduler
 * Method:    nativeRelease
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeRelease
  (JNIEnv *, jobject);

/*
 * Class:     com_wind_ndk_camera_CameraPreviewScheduler
 * Method:    nativeSwitchCamera
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_wind_ndk_camera_CameraPreviewScheduler_nativeSwitchCamera
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_wind_ndk_camera_CameraPreviewScheduler
 * Method:    startEncode
 * Signature: (Ljava/lang/String;IIII)V
 */
JNIEXPORT void JNICALL Java_com_wind_ndk_camera_CameraPreviewScheduler_startEncode
  (JNIEnv *, jobject, jstring, jint, jint, jint, jint);

/*
 * Class:     com_wind_ndk_camera_CameraPreviewScheduler
 * Method:    stopEncode
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wind_ndk_camera_CameraPreviewScheduler_stopEncode
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif