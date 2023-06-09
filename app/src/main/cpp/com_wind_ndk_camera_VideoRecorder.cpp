/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include "video_consumer.h"
#include "camera_preview/camera_preview_controller.h"
/* Header for class com_wind_ndk_camera_VideoRecorder */

#ifndef _Included_com_wind_ndk_camera_VideoRecorder
#define _Included_com_wind_ndk_camera_VideoRecorder
#ifdef __cplusplus
extern "C" {
#endif


VideoConsumer* videoConsumer= nullptr;


/*
 * Class:     com_wind_ndk_camera_VideoRecorder
 * Method:    nativeStartProducer
 * Signature: (II)I

JNIEXPORT jint JNICALL Java_com_wind_ndk_camera_VideoRecorder_nativeStartProducer
  (JNIEnv *, jobject, jint, jint){

} */

/*
 * Class:     com_wind_ndk_camera_VideoRecorder
 * Method:    nativeStopConsumer
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wind_ndk_camera_VideoRecorder_nativeStopConsumer
        (JNIEnv *, jobject){
   if (videoConsumer){
      videoConsumer->stop();
   }
}

/*
 * Class:     com_wind_ndk_camera_VideoRecorder
 * Method:    nativeStartConsumer
 * Signature: (Ljava/lang/String;IIIIII)I
 */
JNIEXPORT jint JNICALL Java_com_wind_ndk_camera_VideoRecorder_nativeStartConsumer
  (JNIEnv *env, jobject jobj, jstring joutputUri,jint videoFrameRate,jint videoBitrate, jint videoWidth, jint videoHeight, jint audioBitrate, jint audioSampleRate, jint audioChannels){

   const char* outputUri= env->GetStringUTFChars(joutputUri,0);
  // JavaVM* jVM= nullptr;
  // env->GetJavaVM(&jVM);
   //jobject gObj= env->NewGlobalRef(jobj);


   videoConsumer=new VideoConsumer();
   int ret=videoConsumer->init(outputUri,videoFrameRate,videoBitrate,videoWidth,videoHeight,audioBitrate,audioSampleRate,audioChannels);
   if (ret<0){
      //todo error
   }else{
      videoConsumer->start();
   }
   env->ReleaseStringUTFChars(joutputUri,outputUri);
   return ret;

}

JNIEXPORT void JNICALL Java_com_wind_ndk_camera_VideoRecorder_nativeRelease
        (JNIEnv *, jobject){
   if (videoConsumer){

   }
}

#ifdef __cplusplus
}
#endif
#endif
