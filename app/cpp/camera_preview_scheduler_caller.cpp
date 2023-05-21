//
// Created by 史浩 on 2023/5/7.
//

#include "camera_preview_scheduler_caller.h"

#define LOG_TAG "CameraPreviewSchedulerCaller"
CameraPreviewSchedulerCaller::CameraPreviewSchedulerCaller(JavaVM *javaVM, jobject jobj):javaVM(javaVM),jobj(jobj){

    //获取需要反射的方法


}
CameraPreviewSchedulerCaller::~CameraPreviewSchedulerCaller() {

}

CameraInfo* CameraPreviewSchedulerCaller::configCamera(int cameraFacingId) {
    JNIEnv* env;
    if (javaVM->AttachCurrentThread(&env,0)!=JNI_OK){
        LOGE("AttachCurrentThread failed");
        return nullptr;
    }
    CameraInfo* result= nullptr;

    jclass jclazz=env->GetObjectClass(jobj);
    if (jclazz!= nullptr){
        jmethodID configCameraMethodID=env->GetMethodID(jclazz,"configCameraFromNative","(I)Lcom/wind/ndk/camera/CameraInfo;");
        if (configCameraMethodID){
            jobject jcameraInfoObj=env->CallObjectMethod(jobj,configCameraMethodID,cameraFacingId);

            jclass jcameraInfoClass= env->GetObjectClass(jcameraInfoObj);
            jmethodID getCameraInfoWidthMethodID =env->GetMethodID(jcameraInfoClass,"getWidth","()I");
            jmethodID getCameraInfoHeightMethodID =env->GetMethodID(jcameraInfoClass,"getHeight","()I");

            int previewWidth=env->CallIntMethod(jcameraInfoObj,getCameraInfoWidthMethodID);
            int previewHeight=env->CallIntMethod(jcameraInfoObj,getCameraInfoHeightMethodID);

            result=new CameraInfo(previewWidth,previewHeight,cameraFacingId);
        }
    }



    if (javaVM->DetachCurrentThread()!=JNI_OK){
        LOGE("DetachCurrentThread failed");
    }

    return result;



}

void CameraPreviewSchedulerCaller::startPreview(int textureId) {

    JNIEnv* env;
    if(javaVM->AttachCurrentThread(&env,0)!=JNI_OK){
        LOGE("AttachCurrentThread failed");
        return;
    }
    jclass jclazz=env->GetObjectClass(jobj);
    jmethodID  startPreviewMethodID=env->GetMethodID(jclazz,"startPreviewFromNative","(I)V");

    env->CallVoidMethod(jobj,startPreviewMethodID,textureId);


    if(javaVM->DetachCurrentThread()!=JNI_OK){
        LOGE("DetachCurrentThread failed");
    }
}

GLfloat *CameraPreviewSchedulerCaller::updateTexImage() {
    JNIEnv* env;
    if(javaVM->AttachCurrentThread(&env,0)!=JNI_OK){
        LOGE("AttachCurrentThread failed");
        return nullptr;
    }
    GLfloat * result;
    jclass jclazz=env->GetObjectClass(jobj);
    jmethodID  updateTexImageMethodID=env->GetMethodID(jclazz,"updateTexImageFromNative","()[F");
     jobject resultObj= static_cast<jfloatArray>(env->CallObjectMethod(jobj,
                                                                          updateTexImageMethodID));
    jfloatArray jfloatArr= static_cast<jfloatArray>(resultObj);

    result=env->GetFloatArrayElements(jfloatArr,0);
    if(javaVM->DetachCurrentThread()!=JNI_OK){
        LOGE("DetachCurrentThread failed");
    }

    return result;
}

void CameraPreviewSchedulerCaller::releaseCamera() {
    JNIEnv* env;
    if(javaVM->AttachCurrentThread(&env,0)!=JNI_OK){
        LOGE("AttachCurrentThread failed");
        return;
    }
    jclass jclazz=env->GetObjectClass(jobj);
    jmethodID  releaseCameraMethodID=env->GetMethodID(jclazz,"releaseCameraFromNative","()V");

    env->CallVoidMethod(jobj,releaseCameraMethodID);

    if(javaVM->DetachCurrentThread()!=JNI_OK){
        LOGE("DetachCurrentThread failed");
    }



}
