//
// Created by 史浩 on 2023/3/29.
//

#ifndef NDK_PNG_BASE_EGL_SURFACE_H
#define NDK_PNG_BASE_EGL_SURFACE_H


#include "egl_core.h"
#include <GLES2/gl2.h>
class BaseEGLSurface {

public:
    BaseEGLSurface(EGLCore* eglCore);
    virtual ~BaseEGLSurface();


    //创建窗口Surface
    void createWindowSurface(ANativeWindow* window);
    //创建离屏Surface
    void createOffscreenSurface(int width,int height);
    //获取宽度
    int getWidth();
    //获取高度
    int getHeight();

    //切换当前上下文
    virtual void makeCurrent();
    //交换缓冲区 显示图像
    virtual bool swapBuffers();
    //设置显示时间戳
    void setPresentationTime(long nsecs);
    //获取当前帧缓冲
    char* getCurrentFrame();

    virtual EGLContext getEGLContext();
    virtual void release();
private:
    //释放EGLSurface
    void releaseEGLSurface();

protected:
    EGLCore* mEGLCore;
    EGLSurface mEGLSurface=EGL_NO_SURFACE;
    int mWidth;
    int mHeight;
};

#endif //NDK_PNG_BASE_EGL_SURFACE_H
