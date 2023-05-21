//
// Created by 史浩 on 2023/5/6.
//

#ifndef NDK_CAMERARECORDER_CAMERA_FILTER_H
#define NDK_CAMERARECORDER_CAMERA_FILTER_H

#include "../utils/log.h"

#include "gles/base_fbo_filter.h"



class CameraFilter : public BaseFboFilter{
private:
    GLuint uMatrixLocation;
    GLfloat* matrix = nullptr;

public:

    CameraFilter();
    ~CameraFilter();



    /**填充AttributeLocation，UniformLocation等*/
    virtual void inflateLocation(GLuint textureId) override;

    virtual GLenum getTextureTarget() override;


    void setMatrix(GLfloat* matrix);

};


#endif //NDK_CAMERARECORDER_CAMERA_FILTER_H
