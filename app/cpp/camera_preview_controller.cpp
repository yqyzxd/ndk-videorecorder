//
// Created by 史浩 on 2023/5/5.
//

#include "camera_preview_controller.h"
#define LOG_TAG "CameraPreviewController"
void CameraPreviewController::updateTexImage(void *ctx) {
    CameraPreviewController* controller= static_cast<CameraPreviewController *>(ctx);
    PreviewRenderer*renderer= dynamic_cast<PreviewRenderer *>(controller->glSurface->getRenderer());
    renderer->updateTexImage();

}

void CameraPreviewController::innerSwitchCamera(void *ctx) {
    CameraPreviewController* controller= static_cast<CameraPreviewController *>(ctx);
    PreviewRenderer*renderer= dynamic_cast<PreviewRenderer *>(controller->glSurface->getRenderer());
    renderer->startPreview(controller->cameraFacingId);
    controller->mSwitchingCamera= false;
}


CameraPreviewController::CameraPreviewController(JavaVM *javaVM, jobject jobj,int cameraFacingId) {
    mSwitchingCamera= false;
    this->cameraFacingId=cameraFacingId;
    glSurface=new GLSurface;
    mPreviewRenderer=new PreviewRenderer(javaVM,jobj,cameraFacingId);

    glSurface->setRenderer(mPreviewRenderer);
}

CameraPreviewController::~CameraPreviewController() {
    glSurface->dealloc();
    delete glSurface;
    glSurface= nullptr;
}

void CameraPreviewController::onSurfaceCreated(ANativeWindow *window) {
    LOGI("onSurfaceCreated");
    glSurface->surfaceCreated(window);

}

void CameraPreviewController::onSurfaceChanged(int width, int height) {
    while (glSurface->getEGLContext()== nullptr){
        LOGI("glSurface->getEGLContext()== nullptr");
    }
    mPreviewRenderer->setEGLContext(glSurface->getEGLContext());
    glSurface->surfaceChanged(width,height);

}

void CameraPreviewController::onSurfaceDestroyed() {
    LOGI("surfaceDestroyed");
    mSwitchingCamera= true;
    glSurface->surfaceDestroyed();




}

void CameraPreviewController::onFrameAvailable() {
    if (!mSwitchingCamera){
        LOGI("onFrameAvailable");
        glSurface->queueEvent(new Runnable(updateTexImage,this));
    }


}

void CameraPreviewController::switchCamera(int cameraFacingId) {
    //切换相机时忽略onFrameAvailable，否则切换时可能出现画面渲染异常的情况
    mSwitchingCamera= true;
    this->cameraFacingId=cameraFacingId;
    glSurface->queueEvent(new Runnable(innerSwitchCamera,this));

}

void
CameraPreviewController::startEncode(const char *h264File, int width, int height, int videoBitrate,
                                     int frameRate) {
    mPreviewRenderer->startEncode(h264File,width,height,videoBitrate,frameRate);
}

void CameraPreviewController::stopEncode() {
    mPreviewRenderer->stopEncode();
}





