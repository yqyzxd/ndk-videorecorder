//
// Created by 史浩 on 2023/3/29.
//

#include "base_egl_surface.h"
BaseEGLSurface::BaseEGLSurface(EGLCore *eglCore) {
    this->mEGLCore=eglCore;
    this->mEGLSurface=EGL_NO_SURFACE;
    mWidth=-1;
    mHeight=-1;
}
BaseEGLSurface::~BaseEGLSurface() {}

void BaseEGLSurface::createWindowSurface(ANativeWindow *window) {

    if (mEGLSurface!=EGL_NO_SURFACE){
        //已经创建了
        return;
    }
    mEGLSurface=mEGLCore->createWindowSurface(window);
}

void BaseEGLSurface::createOffscreenSurface(int width, int height) {
    if (mEGLSurface!=EGL_NO_SURFACE){
        //已经创建了
        return;
    }
    mEGLSurface=mEGLCore->createOffscreenSurface(width,height);
    mWidth=width;
    mHeight=height;
}

EGLContext BaseEGLSurface::getEGLContext() {
    return mEGLCore->getContext();
}
void BaseEGLSurface::makeCurrent() {
    mEGLCore->makeCurrent(mEGLSurface);
}

bool BaseEGLSurface::swapBuffers() {
    bool result=mEGLCore->swapBuffers(mEGLSurface);
    if (!result){
        //swapBuffers failed
    }
    return result;

}

void BaseEGLSurface::setPresentationTime(long nsecs) {
    mEGLCore->setPresentationTime(mEGLSurface,nsecs);
}
char* BaseEGLSurface::getCurrentFrame() {
    char* pixels;
    glReadPixels(0,0,getWidth(),getHeight(),GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    return pixels;
}
int BaseEGLSurface::getWidth() {
    return mWidth;
}
int BaseEGLSurface::getHeight() {
    return mHeight;
}

void BaseEGLSurface::releaseEGLSurface() {
    if (mEGLSurface!=EGL_NO_SURFACE){
        mEGLCore->releaseSurface(mEGLSurface);
        mEGLSurface=EGL_NO_SURFACE;
    }

    mWidth=mHeight=-1;
}

void BaseEGLSurface::release() {
    releaseEGLSurface();
}