//
// Created by 史浩 on 2023/5/11.
//

#ifndef NDK_CAMERARECORDER_GL_OFFSCREEN_SURFACE_H
#define NDK_CAMERARECORDER_GL_OFFSCREEN_SURFACE_H

#include "gl_surface.h"
#include "offscreen_surface.h"
class GLOffScreenSurface: public GLSurface{
public:
    GLOffScreenSurface(EGLContext sharedContext,int width,int height);
    GLOffScreenSurface(int width,int height): GLOffScreenSurface(EGL_NO_CONTEXT,width,height){};

    BaseEGLSurface* createEGLSurface() override;

    bool isOffScreen() override;
private:
    int width;
    int height;
};


#endif //NDK_CAMERARECORDER_GL_OFFSCREEN_SURFACE_H
