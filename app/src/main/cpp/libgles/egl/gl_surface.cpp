//
// Created by 史浩 on 2023/3/29.
//


#include "gl_surface.h"



#define LOG_TAG "GLSurface"
GLSurface::GLSurface(EGLContext sharedContext) {
    mSharedContext=sharedContext;
    mRenderPrepared= false;
    mRenderMode=RENDER_MODE_WHEN_DIRTY;
    mRunnables=new std::queue<Runnable*>();
    pthread_mutex_init(&mLock,NULL);
    pthread_cond_init(&mCond,NULL);
}

GLSurface::~GLSurface() {
    delete mRunnables;
    mRunnables= nullptr;

    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCond);
}

void *GLSurface::threadStartRoutine(void *myself) {
    GLSurface* glSurface= (GLSurface *)myself;
    glSurface->renderLoop();
    return 0;
}

void GLSurface::setRenderer(GLRenderer *renderer) {
    LOGI("setRenderer %d",mRenderThreadStarted);
    this->mRenderer=renderer;
    if (mRenderThreadStarted){
        //already started
        return;
    }


    pthread_create(&_rendererThreadId,0,threadStartRoutine,this);
}

GLRenderer *GLSurface::getRenderer() {
    return mRenderer;
}

void GLSurface::surfaceCreated(ANativeWindow *window) {
    pthread_mutex_lock(&mLock);
    while (!mRenderThreadStarted){
    }
    LOGI("surfaceCreated mRenderThreadStarted:%d",mRenderThreadStarted);
    this->window=window;
    mSurfaceEvent=SURFACE_EVENT_CREATED;
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mLock);
}

void GLSurface::surfaceChanged(int width, int height) {

    pthread_mutex_lock(&mLock);
    LOGE("surfaceChanged width: %d",width);
    while (!mSurface&&mRenderer){
        //wait for surface
        pthread_cond_wait(&mCond,&mLock);
    }
    LOGE("surfaceChanged mRenderThreadStarted:%d",mRenderThreadStarted);
    mSurfaceEvent=SURFACE_EVENT_CHANGED;
    mSurfaceWidth=width;
    mSurfaceHeight=height;
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mLock);
}

void GLSurface::surfaceDestroyed() {
    pthread_mutex_lock(&mLock);
    LOGE("GLSurface::surfaceDestroyed");
    mSurfaceEvent=SURFACE_EVENT_DESTROYED;
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mLock);

}

void GLSurface::queueEvent(Runnable* runnable) {
    pthread_mutex_lock(&mLock);

    mRunnables->push(runnable);
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mLock);

}

void GLSurface::requestRender() {
    pthread_mutex_lock(&mLock);
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mLock);
}

void GLSurface::dealloc() {
    LOGE("dealloc:%d",mRenderPrepared);

    while (mRenderPrepared){
        surfaceDestroyed();
        usleep(0.5*(1e6));
    }
    pthread_mutex_lock(&mLock);
   // releaseSurface();
    //delete mRenderer;
    //mRenderer=NULL;
    mKillRendererThread= true;
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mLock);
    LOGI("wait GL THREAD");
    //等待线程结束
    pthread_join(_rendererThreadId, 0);
    mRenderThreadStarted= false;
    LOGI("GL THREAD dead");
}
void GLSurface::renderLoop() {
    mRenderThreadStarted= true;

    while (!mKillRendererThread){
        mWakeUpFromDestroyed= false;
        pthread_mutex_lock(&mLock);
        switch (mSurfaceEvent) {
            case SURFACE_EVENT_NONE:
                break;
            case SURFACE_EVENT_CREATED:
                mSurfaceEvent=SURFACE_EVENT_NONE;
                if (mRenderer){
                    mSurface=createEGLSurface();
                    mSurface->makeCurrent();
                    LOGI("before surfaceCreated ");
                    mRenderer->surfaceCreated();
                    LOGI("after surfaceCreated ");
                    pthread_cond_signal(&mCond);
                }
                break;
            case SURFACE_EVENT_CHANGED:
                mSurfaceEvent=SURFACE_EVENT_NONE;
                if (mRenderer){
                    LOGI("before surfaceChanged ");
                    mSurface->makeCurrent();
                    mRenderer->surfaceChanged(mSurfaceWidth,mSurfaceHeight);
                    mRenderPrepared= true;
                    LOGI("after surfaceChanged ");
                }
                break;
            case SURFACE_EVENT_DESTROYED:
                mRenderPrepared= false;
                mSurfaceEvent=SURFACE_EVENT_NONE;
                releaseSurface();
                //GL THREAD 进入等待状态，渲染线程依旧存在，当下次surfaceCreate时无须重启渲染线程
                LOGI("SURFACE_EVENT_DESTROYED before");
                pthread_cond_wait(&mCond,&mLock);
                mWakeUpFromDestroyed = true;
                LOGI("SURFACE_EVENT_DESTROYED after");
                break;
        }




        if (mRenderPrepared){
            while (!mRunnables->empty()){
                Runnable* runnable=mRunnables->front();
                mRunnables->pop();
                //LOGI("before runnable  run");
                runnable->run();
                delete runnable;
                //LOGI("after runnable  run");
            }
            mSurface->makeCurrent();
            //LOGI("before onDrawFrame");
            checkGlError("onDrawFrame");
            mRenderer->onDrawFrame();
            //LOGI("after onDrawFrame");
            if (!isOffScreen()){
                mSurface->swapBuffers();
            }

        }
        if (mRenderMode == RENDER_MODE_WHEN_DIRTY && !mKillRendererThread&& !mWakeUpFromDestroyed){
            //LOGI("before pthread_cond_wait");
            pthread_cond_wait(&mCond,&mLock);
            //LOGI("after pthread_cond_wait");
        }
        //LOGE("GL thread next loop");
        pthread_mutex_unlock(&mLock);
    }
}

bool GLSurface::isRenderPrepared() {
    return mRenderPrepared;
}
BaseEGLSurface *GLSurface::createEGLSurface() {
    EGLCore* egl=new EGLCore();
    egl->init(mSharedContext);
    return new WindowSurface(egl,window);
}

void GLSurface::releaseSurface() {
    if (mRenderer){
        mRenderer->surfaceDestroyed();
        delete mRenderer;
        mRenderer= nullptr;
    }
    if (mSurface){
        mSurface->release();
        delete mSurface;
        mSurface=NULL;
    }
}

EGLContext GLSurface::getEGLContext() {
    if (mSurface!= nullptr){
        return  mSurface->getEGLContext();
    }
    return EGL_NO_CONTEXT;

}

bool GLSurface::isOffScreen() {
    return false;
}