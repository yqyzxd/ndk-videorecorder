//
// Created by 史浩 on 2023/5/6.
//

#include "preview_renderer.h"



#define  LOG_TAG "PreviewRenderer"
PreviewRenderer::PreviewRenderer(JavaVM*javaVm,jobject jobj,int cameraFacingId) {
    mCameraFacingId=cameraFacingId;
    mCaller=new CameraPreviewSchedulerCaller(javaVm,jobj);


    mCameraFilter= nullptr;
    mScreenFilter= nullptr;

}

PreviewRenderer::~PreviewRenderer() {
    if (mCaller){
        delete mCaller;
        mCaller= nullptr;
    }
    if (mVideoEncoderAdapter!= nullptr){
        mVideoEncoderAdapter->dealloc();
        delete mVideoEncoderAdapter;
        mVideoEncoderAdapter= nullptr;
    }
    if (mTexture){
        mTexture->dealloc();
        delete mTexture;
        mTexture= nullptr;
    }

    if (mCameraFilter){
        mCameraFilter->dealloc();
        delete mCameraFilter;
        mCameraFilter= nullptr;
    }
    if (mScreenFilter){
        mScreenFilter->dealloc();
        delete mScreenFilter;
        mScreenFilter= nullptr;
    }


}

void PreviewRenderer::surfaceCreated() {

    mTexture=new Texture(GL_TEXTURE_EXTERNAL_OES);
    mTexture->createTexture();

    mScreenFilter=new ScreenFilter();

    startPreview(mCameraFacingId);
    checkGlError("startPreview");


}


void PreviewRenderer::startPreview(int cameraFacingId) {
    mCaller->releaseCamera();
    CameraInfo* cameraInfo=mCaller->configCamera(cameraFacingId);
    mTextureWidth=cameraInfo->previewWidth;
    mTextureHeight=cameraInfo->previewHeight;
    if (mCameraFilter== nullptr){//如重新创建CameraFilter，会导致切换摄像头时黑屏，因为重新创建时没有调用onReady设置宽高
        mCameraFilter=new CameraFilter();
    }
    LOGI("before startPreview");
    mCaller->startPreview(mTexture->getTextureId());
    LOGI("after startPreview");
    if (mCameraFacingId==1){ //前置摄像头
        mScreenFilter->setTextureSize(mTextureHeight,mTextureWidth);
    }else{
        mScreenFilter->setTextureSize(mTextureWidth,mTextureHeight);
    }

}


void PreviewRenderer::surfaceChanged(int width, int height) {


    mCameraFilter->onReady(width,height);
    mScreenFilter->onReady(width,height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1,1,1,1);
    checkGlError("glClearColor");


}

void PreviewRenderer::onDrawFrame() {
    checkGlError("onDrawFrame");
    int textureId=mCameraFilter->onDrawFrame(mTexture->getTextureId());
    checkGlError("mCameraFilter onDrawFrame");
    textureId=mScreenFilter->onDrawFrame(textureId);
    checkGlError("mScreenFilter onDrawFrame");


    if (mEncoding){
        //encode
        mVideoEncoderAdapter->encode(textureId);
    }


}
void PreviewRenderer::surfaceDestroyed() {
    //通知java层释放camera
    mCaller->releaseCamera();

    mCameraFilter->dealloc();
    mScreenFilter->dealloc();


}

void PreviewRenderer::updateTexImage() {

    checkGlError("updateTexImage");
    GLfloat* matrix=mCaller->updateTexImage();
    mCameraFilter->setMatrix(matrix);
}

void PreviewRenderer::setEGLContext(EGLContext sharedEGLContext) {
    this->mSharedEGLContext=sharedEGLContext;
}

void PreviewRenderer::startEncode( int width, int height, int videoBitrate,
                                  int frameRate) {

    mVideoEncoderAdapter=new SoftVideoEncoderAdapter();
    mVideoEncoderAdapter->init(width,height,videoBitrate,frameRate);
    mVideoEncoderAdapter->prepare(mSharedEGLContext);
    mEncoding= true;
}

void PreviewRenderer::stopEncode() {
    if (mVideoEncoderAdapter!= nullptr){
        mEncoding= false;
        //LOGI("before stop");
        mVideoEncoderAdapter->stop();
        //LOGI("after stop");
        delete mVideoEncoderAdapter;
        //LOGI("delete mVideoEncoderAdapter");
        mVideoEncoderAdapter= nullptr;
    }

}


