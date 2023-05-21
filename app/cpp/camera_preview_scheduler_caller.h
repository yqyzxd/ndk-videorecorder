//
// Created by 史浩 on 2023/5/7.
//

#ifndef NDK_CAMERARECORDER_CAMERA_PREVIEW_SCHEDULER_CALLER_H
#define NDK_CAMERARECORDER_CAMERA_PREVIEW_SCHEDULER_CALLER_H

#include <jni.h>
#include <GLES2/gl2.h>
#include "utils/log.h"
#include "camera_preview/camera_info.h"

class CameraPreviewSchedulerCaller {
public:
    CameraPreviewSchedulerCaller(JavaVM* javaVM,jobject jobj);
    ~CameraPreviewSchedulerCaller();

    CameraInfo* configCamera(int cameraFacingId);
    void startPreview(int textureId);


    GLfloat* updateTexImage();

    void releaseCamera();

private:
    JavaVM* javaVM;
    jobject jobj;

};


#endif //NDK_CAMERARECORDER_CAMERA_PREVIEW_SCHEDULER_CALLER_H
