//
// Created by 史浩 on 2023/5/11.
//

#ifndef NDK_CAMERARECORDER_COPY_TEXTURE_RENDERER_H
#define NDK_CAMERARECORDER_COPY_TEXTURE_RENDERER_H


#include "egl/gl_renderer.h"
#include "gles/texture.h"
#include "gles/base_fbo_filter.h"
#include "gles/rgb_to_yuy2_filter.h"
#include "../../utils/types.h"
#include "../../utils/log.h"

typedef void (*Callback) (void *ctx, void* bytes) ;
class CopyTextureRenderer : public GLRenderer {
public:
    CopyTextureRenderer(Callback callback,void* ctx);

    ~CopyTextureRenderer();


    void surfaceCreated();
    void surfaceChanged(int width, int height);
    void surfaceDestroyed();
    void onDrawFrame();


    void updateTex(int textureId);


private:
    BaseFboFilter *mFboFilter;
    BaseFboFilter* mYuy2Filter;

    int mInputTextureId;

    Callback mCallback;
    void* mCallbackCtx;
    int mWidth;
    int mHeight;
};


#endif //NDK_CAMERARECORDER_COPY_TEXTURE_RENDERER_H
