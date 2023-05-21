//
// Created by 史浩 on 2023/5/7.
//

#ifndef NDK_CAMERARECORDER_SCREEN_FILTER_H
#define NDK_CAMERARECORDER_SCREEN_FILTER_H


#include "gles/base_filter.h"
class ScreenFilter: public BaseFilter{
private:
    int mTextureWidth;
    int mTextureHeight;
public:
    ScreenFilter();
    ~ScreenFilter();
    //void updateTexImage(GLuint textureId,void* bytes,int width,int height);

    //为了纹理与视口的比例保持一致（这样才能避免纹理被拉伸），需要裁剪纹理坐标，因此重写
    GLfloat *getTextureData() override;

    void setTextureSize(int width, int height);
};


#endif //NDK_CAMERARECORDER_SCREEN_FILTER_H
