//
// Created by 史浩 on 2023/5/7.
//

#include "screen_filter.h"
ScreenFilter::ScreenFilter(): BaseFilter() {

}

ScreenFilter::~ScreenFilter() {

}

void ScreenFilter::setTextureSize(int textureWidth, int textureHeight) {
    this->mTextureWidth=textureWidth;
    this->mTextureHeight=textureHeight;

}

/*void ScreenFilter::updateTexImage(GLuint textureId, void *bytes, int width, int height) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textureId);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,bytes);

    glBindTexture(GL_TEXTURE_2D,0);
}*/
/**纹理宽高和视口宽高(也可以说是surfaceView的宽宽)比例可能不是一致的，为了不使画面被拉伸，需要对纹理坐标进行裁剪*/
GLfloat *ScreenFilter::getTextureData() {

    float textureRatio =mTextureWidth/(float)mTextureHeight;
    float surfaceRatio=mWidth/(float)mHeight;

    //1. 计算offset
    float xOffset=0;
    float yOffset=0;
    if (textureRatio>=surfaceRatio){
        int expectTextureWidth=mTextureHeight * surfaceRatio;
        xOffset= (mTextureWidth - expectTextureWidth)/(float)(2*mTextureWidth);
    } else {
        int expectTextureHeight=mTextureWidth/surfaceRatio;
        yOffset= (mTextureHeight-expectTextureHeight)/float(2*mTextureHeight);
    }

    //2. 替换；原纹理坐标 0 的地方使用xOffset/yOffset，1的地方使用1-xOffset / 1-yOffset

    return new GLfloat[8]{
            xOffset, 1-yOffset,
            1-xOffset, 1-yOffset,
            xOffset, yOffset,
            1-xOffset, yOffset,
    };
}


