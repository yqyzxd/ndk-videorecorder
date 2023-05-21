//
// Created by 史浩 on 2023/3/29.
//

#include "window_surface.h"
WindowSurface::WindowSurface(EGLCore *eglCore, ANativeWindow *window) :BaseEGLSurface(eglCore){

    mSurface=window;
    createWindowSurface(mSurface);
}

WindowSurface::~WindowSurface() {

}
void WindowSurface::release() {
    BaseEGLSurface::release();
    if (mSurface){
        ANativeWindow_release(mSurface);
        mSurface=NULL;
    }
    if (mEGLCore){
        mEGLCore->release();
        delete mEGLCore;
        mEGLCore=NULL;
    }

}

void WindowSurface::recreate(EGLCore *eglCore) {
    if (mSurface==NULL){
        return;
    }
    mEGLCore=eglCore;
    createWindowSurface(mSurface);
}