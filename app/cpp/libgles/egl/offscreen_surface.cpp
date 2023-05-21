//
// Created by 史浩 on 2023/3/29.
//

#include "offscreen_surface.h"

OffScreenSurface::OffScreenSurface(EGLCore *eglCore,int width,int height) : BaseEGLSurface(eglCore) {

    createOffscreenSurface(width,height);
}

OffScreenSurface::~OffScreenSurface() {}


