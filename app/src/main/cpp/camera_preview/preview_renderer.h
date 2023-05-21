//
// Created by 史浩 on 2023/5/6.
//

#ifndef NDK_CAMERARECORDER_PREVIEW_RENDERER_H
#define NDK_CAMERARECORDER_PREVIEW_RENDERER_H

#include "gles/gl_shaders.h"
#include "egl/gl_renderer.h"
#include "camera_filter.h"
#include "../camera_preview_scheduler_caller.h"
#include "gles/texture.h"
#include "screen_filter.h"
#include "gles/gl_utils.h"
#include "../encoder/soft/soft_video_encoder_adapter.h"
class PreviewRenderer: public GLRenderer{
private:
    CameraFilter* mCameraFilter;
    ScreenFilter* mScreenFilter;
    CameraPreviewSchedulerCaller* mCaller= nullptr;
    int mCameraFacingId;

    Texture* mTexture;
    int mTextureWidth;
    int mTextureHeight;

    EGLContext mSharedEGLContext;
    VideoEncoderAdapter* mVideoEncoderAdapter= nullptr;
    //是否开启编码
    bool mEncoding= false;
public:
    PreviewRenderer(JavaVM* javaVM,jobject jobj,int cameraFacingId);
    ~PreviewRenderer();
    void surfaceCreated() override;
    void surfaceChanged(int width, int height) override;
    void surfaceDestroyed() override;
    void onDrawFrame() override;

    void updateTexImage();

    void startPreview(int cameraFacingId);

    void setEGLContext(EGLContext pVoid);

    void startEncode(const char *h264File, int width, int height, int videoBitrate, int frameRate);

    void stopEncode();
};


#endif //NDK_CAMERARECORDER_PREVIEW_RENDERER_H
