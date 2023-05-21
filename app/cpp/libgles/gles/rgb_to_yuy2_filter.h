//
// Created by 史浩 on 2023/5/13.
//

#ifndef NDK_CAMERARECORDER_RGB_TO_YUY2_FILTER_H
#define NDK_CAMERARECORDER_RGB_TO_YUY2_FILTER_H



/**
 *
 RGB转YUV公式
Y = 0.299R + 0.587G + 0.114B
U = -0.147R - 0.289G + 0.436B
V = 0.615R - 0.515G - 0.100B

 YUV转RGB公式
R = Y + 1.14V
G = Y - 0.39U - 0.58V
B = Y + 2.03U
 *
 */


#include "base_fbo_filter.h"

#define COEFFICIENT_R_Y 0.299f
#define COEFFICIENT_R_U -0.147f
#define COEFFICIENT_R_V 0.615f

#define COEFFICIENT_G_Y 0.587f
#define COEFFICIENT_G_U -0.289f
#define COEFFICIENT_G_V -0.515f

#define COEFFICIENT_B_Y 0.114f
#define COEFFICIENT_B_U 0.436f
#define COEFFICIENT_B_V -0.100f

class RgbToYuy2Filter : public BaseFboFilter{
private:
    //顶点坐标
    int mUniformCoefficientY;
    int mUniformCoefficientU;
    int mUniformCoefficientV;
    int mUniformStep;

    //原rgba的图片宽度
    int mSrcWidth;

public:
    RgbToYuy2Filter();
    ~RgbToYuy2Filter();

    void inflateLocation(GLuint textureId) override;
    void onReady(int width,int height) override;
    // GLfloat* getTextureData() override;
};


#endif //NDK_CAMERARECORDER_RGB_TO_YUY2_FILTER_H
