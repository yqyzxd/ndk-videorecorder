//
// Created by 史浩 on 2023/3/29.
//

#ifndef NDK_PNG_OFFSCREEN_SURFACE_H
#define NDK_PNG_OFFSCREEN_SURFACE_H


#include "base_egl_surface.h"

class OffScreenSurface : public BaseEGLSurface{
public:
    OffScreenSurface(EGLCore *eglCore,int width,int height);
    ~OffScreenSurface();



};


#endif //NDK_PNG_OFFSCREEN_SURFACE_H
