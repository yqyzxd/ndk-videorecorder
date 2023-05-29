//
// Created by 史浩 on 2023/5/11.
//

#ifndef NDK_CAMERARECORDER_SOFT_VIDEO_ENCODER_ADAPTER_H
#define NDK_CAMERARECORDER_SOFT_VIDEO_ENCODER_ADAPTER_H

#include "../video_encoder_adapter.h"
#include "../../utils/thread.h"
#include "x264_encoder.h"
#include "../../libs/blocking_queue/blocking_queue.h"
#include "../video_frame.h"
#include "egl/gl_offscreen_surface.h"
#include "copy_texture_renderer.h"
#include "../../utils/time.h"
class SoftVideoEncoderAdapter : public VideoEncoderAdapter {
public:
    SoftVideoEncoderAdapter();

    ~SoftVideoEncoderAdapter();

    void prepare(EGLContext sharedContext);

    void encode(int textureId);
    void stop();
    void dealloc();

    static void encodeRunnable(void* ctx);

    void runEncode();

    //纹理拷贝成功回调
    static void onRenderFinishCallback(void* ctx,void* bytes);
private:
    /**编码线程*/
    Thread* mEncodeThread;

    X264Encoder* mEncoder;

    BlockingQueue<VideoFrame*>* mVideoFrameQueue;


    GLOffScreenSurface* mGLOffScreenSurface;
    CopyTextureRenderer* mCopyTextureRenderer;

    Lock* mCopyLock;
    Condition* mCopyCond;

    int mByteSize;

    //是否退出编码
    bool mQuit= false;
    int mTextureId=-1;
    /**
     * 纹理结束回调
     * @param bytes yuy2格式的数据
     */
    void onRenderFinish(void *buffer);
};


#endif //NDK_CAMERARECORDER_SOFT_VIDEO_ENCODER_ADAPTER_H
