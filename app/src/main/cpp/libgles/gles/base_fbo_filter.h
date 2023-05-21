//
// Created by 史浩 on 2023/5/6.
//

#ifndef NDK_CAMERARECORDER_BASE_FBO_FILTER_H
#define NDK_CAMERARECORDER_BASE_FBO_FILTER_H


#include "base_filter.h"
#include "gl_utils.h"
#include "../../utils/log.h"
class BaseFboFilter: public BaseFilter{
private:
    GLuint mFbo;
    GLuint mFboTextureId;
public:
    BaseFboFilter(const char *vertexSource, const char *fragmentSource);
    BaseFboFilter(): BaseFboFilter(base_vertex,base_fragment){};
    virtual ~BaseFboFilter();
    virtual void onReady(int width,int height);
    virtual GLuint onDrawFrame(GLuint textureId);
    int readPixels(void *pixels);
    virtual void dealloc();
private:
    void bindFramebuffer();
    void unBindFramebuffer();

};


#endif //NDK_CAMERARECORDER_BASE_FBO_FILTER_H
