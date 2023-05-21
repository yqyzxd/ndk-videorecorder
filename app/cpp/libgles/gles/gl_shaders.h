//
// Created by 史浩 on 2023/5/7.
//

#ifndef NDK_CAMERARECORDER_GL_SHADERS_H
#define NDK_CAMERARECORDER_GL_SHADERS_H

#include <GLES2/gl2.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHADER_STRING(s) #s


static char *base_vertex = SHADER_STRING(
// 把顶点坐标给这个变量， 确定要画画的形状
        attribute vec4 a_Position;
        //接收纹理坐标，接收采样器采样图片的坐标
        attribute vec2 a_Coordinate;
        //传给片元着色器 像素点
        varying vec2 v_Coordinate;

        void main() {
            //内置变量 gl_Position ,我们把顶点数据赋值给这个变量 opengl就知道它要画什么形状了
            gl_Position = a_Position;
            v_Coordinate = a_Coordinate;
        }
);

static char *base_fragment = SHADER_STRING(
//float数据是什么精度的
        precision mediump float;
        //采样点的坐标
        varying vec2 v_Coordinate;
        //采样器
        uniform sampler2D u_Texture;

        void main() {
            //变量 接收像素值
            // texture2D：采样器 采集v_Coordinate的像素
            //赋值给 gl_FragColor 就可以了
            gl_FragColor = texture2D(u_Texture, v_Coordinate);
        }
);

static char *camera_vertex = SHADER_STRING(
        attribute vec4 a_Position;
        attribute vec4 a_Coordinate;
        varying vec2 v_Coordinate;
        uniform mat4 u_Matrix;
        void main() {
            v_Coordinate = (u_Matrix * a_Coordinate).xy;
            gl_Position = a_Position;
        }
);

static char *camera_fragment =
        "#extension GL_OES_EGL_image_external : require\n"
        "precision mediump float;\n"
        "uniform samplerExternalOES u_Texture;\n"
        "varying vec2 v_Coordinate;\n"
        "void main(){\n"
        "   gl_FragColor=texture2D(u_Texture,v_Coordinate);"
        "}\n";

// YUY2（和YUYV）格式为每个像素保留Y分量，而UV分量在水平方向上每两个像素采样一次。一个宏像素为4个字节，实际表示2个像素。（4:2:2的意思为一个宏像素中有4个Y分量、2个U分量和2个V分量。）
//图像数据中YUV分量排列顺序如下： Y0 U0 Y1 V0    Y2 U1 Y3 V1 …
static char *rgba_to_yuy2_fragment = SHADER_STRING(
        precision mediump float;
        //纹理坐标
        varying vec2 v_Coordinate;
        //采样器
        uniform sampler2D u_Texture;

        //rgb转yuv的系数
        uniform vec4 u_CoefficientY;
        uniform vec4 u_CoefficientU;
        uniform vec4 u_CoefficientV;

        //纹理图片的宽度为width，纹理坐标s范围为0-1，那么step为 1/width 。
        uniform float u_Step;

        void main() {
            vec2 step = vec2(u_Step, 0.0);

            //取出左右像素点，通过rgb转yuv的公式分别计算出y u v
            vec4 lRGBA = texture2D(u_Texture, v_Coordinate);
            vec4 rRGBA = texture2D(u_Texture, v_Coordinate + step);
            vec4 lRGB = vec4(lRGBA.rgb, 1);
            float y0 = dot(lRGB, u_CoefficientY);
            float u0 = dot(lRGB, u_CoefficientU);
            float v0 = dot(lRGB, u_CoefficientV);

            vec4 rRGB = vec4(rRGBA.rgb, 1);
            float y1 = dot(rRGB, u_CoefficientY);

            gl_FragColor = vec4(y0, u0, y1, v0);
        }

);

//4*4 单位矩阵
static GLfloat *identity_matrix = new GLfloat[16]{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
};

#ifdef __cplusplus
};
#endif

#endif //NDK_CAMERARECORDER_GL_SHADERS_H
