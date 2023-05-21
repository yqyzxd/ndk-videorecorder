//
// Created by 史浩 on 2023/3/29.
//

#ifndef NDK_PNG_GL_SURFACE_H
#define NDK_PNG_GL_SURFACE_H


#include <pthread.h>
#include "gl_renderer.h"
#include "window_surface.h"
#include "../../utils/runnable.h"
#include "../../utils/log.h"
#include <unistd.h>
#include "gles/gl_utils.h"
#include <queue>

enum RenderMode{
    RENDER_MODE_CONTINUOUSLY, RENDER_MODE_WHEN_DIRTY
};
enum SurfaceEvent{
    SURFACE_EVENT_NONE,SURFACE_EVENT_CREATED, SURFACE_EVENT_CHANGED,SURFACE_EVENT_DESTROYED
};
class GLSurface {
public:
    GLSurface(EGLContext sharedContext);
    GLSurface(): GLSurface(EGL_NO_CONTEXT){};
    virtual  ~GLSurface();

    void setRenderMode(RenderMode mode);
    void setRenderer(GLRenderer* renderer);
    virtual GLRenderer* getRenderer();
    virtual void surfaceCreated(ANativeWindow* window);
    virtual void surfaceChanged(int width,int height);
    virtual void surfaceDestroyed(void);
    /**request to render frame*/
    void requestRender();
    void queueEvent(Runnable* runnable);
    virtual void dealloc();

    /**return WindowSurface or OffScreenSurface*/
    virtual BaseEGLSurface* createEGLSurface();

    virtual EGLContext getEGLContext();

    //渲染环境是否准备就绪
    virtual bool isRenderPrepared();
    virtual bool isOffScreen();
protected:
    //共享EGLContext
    EGLContext mSharedContext;
private:
    pthread_t _rendererThreadId=0;
    pthread_mutex_t mLock;
    pthread_cond_t mCond;
    //是否结束渲染线程
    bool mKillRendererThread= false;
    //渲染线程是否已经启动
    bool mRenderThreadStarted= false;
    //surface事件
    SurfaceEvent mSurfaceEvent= SURFACE_EVENT_NONE;

    //渲染环境是否准备就绪
    bool mRenderPrepared;

    int mSurfaceWidth;
    int mSurfaceHeight;
    ANativeWindow* window;
    //渲染器
    GLRenderer* mRenderer=0;
    //EGL Surface
    BaseEGLSurface* mSurface=0;
    //渲染模式
    RenderMode mRenderMode=RENDER_MODE_CONTINUOUSLY;
    std::queue<Runnable*>* mRunnables= nullptr;
    bool mWakeUpFromDestroyed= false;



private:
    static void* threadStartRoutine(void *myself);
    void renderLoop();
    void releaseSurface();
};


#endif //NDK_PNG_GL_SURFACE_H
