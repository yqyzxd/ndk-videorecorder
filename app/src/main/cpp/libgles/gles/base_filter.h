//
// Created by 史浩 on 2023/3/29.
//

#ifndef NDK_PNG_BASE_FILTER_H
#define NDK_PNG_BASE_FILTER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "gl_shaders.h"

class BaseFilter {

public:
    BaseFilter(const char *vertexSource, const char *fragmentSource);
    BaseFilter(): BaseFilter(base_vertex,base_fragment){}
    virtual ~BaseFilter() {};

    virtual void onReady(int width, int height);

    virtual GLuint onDrawFrame(GLuint textureId);

    virtual void dealloc();

    /**纹理类型 如GL_TEXTURE_2D*/
    virtual GLenum getTextureTarget();

    /**获取AttributeLocation，UniformLocation等*/
    virtual void getLocation();
    /**填充AttributeLocation，UniformLocation等*/
    virtual void inflateLocation(GLuint textureId);
    /**获取顶点坐标*/
    virtual GLfloat* getVertexData();
    /**获取纹理坐标*/
    virtual GLfloat* getTextureData();

protected:
    GLuint mProgram;
    int mWidth;
    int mHeight;

    //顶点坐标
    int mPositionLocation;
    //纹理坐标
    int mCoordLocation;
    //纹理
    int mTextureLocation;
};


#endif //NDK_PNG_BASE_FILTER_H
