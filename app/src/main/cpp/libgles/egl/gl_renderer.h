//
// Created by 史浩 on 2023/3/29.
//

#ifndef NDK_PNG_GL_RENDERER_H
#define NDK_PNG_GL_RENDERER_H

#include <android/native_window.h>

class GLRenderer{
public:
    GLRenderer(){};
    virtual ~GLRenderer() {}; //注意，最好定义此虚析构函数，够避免子类不能正常调用析构函数；如果定义为纯虚析构函数，则必须带定义体，因为子类隐含调用该析构函数。

    virtual void surfaceCreated()=0;
    virtual void surfaceChanged(int width,int height)=0;
    virtual void surfaceDestroyed()=0;
    virtual void onDrawFrame()=0;
};

#endif //NDK_PNG_GL_RENDERER_H
