//
// Created by 史浩 on 2023/3/29.
//

#ifndef NDK_PNG_WINDOW_SURFACE_H
#define NDK_PNG_WINDOW_SURFACE_H


#include "base_egl_surface.h"

class WindowSurface : public BaseEGLSurface {

public:
    WindowSurface(EGLCore* eglCore,ANativeWindow* window);
    ~WindowSurface();

    void release() override;
    void recreate(EGLCore* eglCore);

private:
    ANativeWindow* mSurface;
    bool mReleaseSurface;
};

#endif //NDK_PNG_WINDOW_SURFACE_H
