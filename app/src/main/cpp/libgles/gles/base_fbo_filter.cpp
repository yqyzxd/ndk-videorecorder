//
// Created by 史浩 on 2023/5/6.
//

#include "base_fbo_filter.h"

#define LOG_TAG "BaseFboFilter"
BaseFboFilter::BaseFboFilter(const char *vertexSource, const char *fragmentSource)
        : BaseFilter(vertexSource, fragmentSource) {

}

BaseFboFilter::~BaseFboFilter(){

}

void BaseFboFilter::bindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER,mFbo);
    glBindTexture(GL_TEXTURE_2D, mFboTextureId);
    checkGlError("glBindTexture");
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,mFboTextureId, 0);
}

void BaseFboFilter::unBindFramebuffer() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void BaseFboFilter::onReady(int width, int height) {
    BaseFilter::onReady(width,height);

    createFrameBuffer(&mFbo,&mFboTextureId,width,height);
    checkGlError("createFrameBuffer");


}

GLuint BaseFboFilter::onDrawFrame(GLuint textureId) {
    bindFramebuffer();
    checkGlError("glFramebufferTexture2D");
    BaseFilter::onDrawFrame(textureId);
    checkGlError("after BaseFboFilter onDrawFrame");
    unBindFramebuffer();
    return mFboTextureId;
}

int BaseFboFilter::readPixels(void *pixels) {
    bindFramebuffer();
    //LOGI("readPixels width:%d,height:%d",mWidth,mHeight);
    glReadPixels(0,0,mWidth,mHeight,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
    checkGlError("glReadPixels");

    unBindFramebuffer();

    return 0;
}
void BaseFboFilter::dealloc() {
    BaseFilter::dealloc();
    if (mFboTextureId){
        glDeleteTextures(1,&mFboTextureId);
        mFboTextureId=0;
    }
    if (mFbo){
        glDeleteFramebuffers(1,&mFbo);
        mFbo=0;
    }

}