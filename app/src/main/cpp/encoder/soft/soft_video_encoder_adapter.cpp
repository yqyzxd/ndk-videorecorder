//
// Created by 史浩 on 2023/5/11.
//

#include "soft_video_encoder_adapter.h"

#define LOG_TAG "SoftVideoEncoderAdapter"

void SoftVideoEncoderAdapter::onRenderFinishCallback(void *ctx, void *bytes) {
    LOGI("onRenderFinishCallback");
    SoftVideoEncoderAdapter *adapter = static_cast<SoftVideoEncoderAdapter *>(ctx);
    adapter->onRenderFinish(bytes);
    return;
}

void SoftVideoEncoderAdapter::encodeRunnable(void *ctx) {
    SoftVideoEncoderAdapter *adapter = static_cast<SoftVideoEncoderAdapter *>(ctx);
    adapter->runEncode();

}

SoftVideoEncoderAdapter::SoftVideoEncoderAdapter() {
    mEncodedFrameCount=0;
    mEncodeThread = new Thread(new Runnable(encodeRunnable, this));

    mCopyLock = new Lock();
    mCopyCond = mCopyLock->newCondition();
}

SoftVideoEncoderAdapter::~SoftVideoEncoderAdapter() {
}


void SoftVideoEncoderAdapter::runEncode() {

    //作为消费者：不断从VideoFrameQueue中获取VideoFrame
    mEncoder = new X264Encoder();
    int ret=mEncoder->init(h264File, width, height, bitRate, frameRate);

    if (ret>=0){
        while (!mQuit) {
            LOGI("runEncode");
            VideoFrame *videoFrame = nullptr;
            int ret = mVideoFrameQueue->take(&videoFrame);
            if (ret == 0 && videoFrame!= nullptr) {
                mEncoder->encode(videoFrame);
            }
            if (videoFrame != nullptr) {
                delete videoFrame;
                videoFrame = nullptr;
            }

        }
    }
    mEncoder->dealloc();
    delete mEncoder;
    mEncoder = nullptr;


}

void SoftVideoEncoderAdapter::prepare(EGLContext sharedContext) {
    mByteSize = width * height * 2;//yuy2占用字节数
    mStartTimeInMills=-1;
    mGLOffScreenSurface = new GLOffScreenSurface(sharedContext, width, height);
    mCopyTextureRenderer = new CopyTextureRenderer(onRenderFinishCallback, this);
    //启动纹理拷贝线程
    LOGI("GLOffScreenSurface setRenderer");
    mGLOffScreenSurface->setRenderer(mCopyTextureRenderer);
    mGLOffScreenSurface->surfaceCreated(nullptr);
    mGLOffScreenSurface->surfaceChanged(width,height);
    //创建VideoFrame阻塞队列
    mVideoFrameQueue = new LinkedBlockingQueue<VideoFrame *>();
    //启动编码线程
    mEncodeThread->start();
}

void SoftVideoEncoderAdapter::encode(int textureId) {

    if (mQuit){
        return;
    }
    LOGI("start encode");
    //判断渲染线程是否已经启动
    if (!mGLOffScreenSurface->isRenderPrepared()){
        LOGI("skip encode because render thread is not prepared");
        return;
    }
    if (mStartTimeInMills == -1) {
        mStartTimeInMills = currentTimeMills();
    }
    int64_t duration = currentTimeMills() - mStartTimeInMills;

    //在frameRate的帧率下 经过x秒后应该有的帧数量
    int standardFrameCount = duration / 1000.0f * frameRate + 0.5;
    LOGE("standardFrameCount:%d,mEncodedFrameCount:%d",standardFrameCount,mEncodedFrameCount);
    //帧率控制
    if (standardFrameCount < mEncodedFrameCount) {
        LOGI("drop frame because exceed frameRate");
        return;
    }
    mEncodedFrameCount++;
    mCopyLock->lock();
    //阻塞预览线程，待纹理拷贝线程拷贝成功之后继续预览
    if (this->mTextureId==-1){
        this->mTextureId = textureId;
        LOGI("updateTex");
        //设置纹理拷贝结束回调函数，纹理拷贝结束后，需要通知继续执行
        mCopyTextureRenderer->updateTex(textureId);
    }
    //唤醒纹理拷贝线程进行工作
    LOGI("requestRender");
    mGLOffScreenSurface->requestRender();
    //阻塞预览线程，需要通过回调函数进行唤醒
    LOGE("mCopyCond await");
    mCopyCond->await();

    LOGE("mCopyLock unlock");
    mCopyLock->unlock();
    LOGE("end encode");
}

void SoftVideoEncoderAdapter::stop() {
    mQuit = true;
    if (mGLOffScreenSurface!= nullptr){
        mGLOffScreenSurface->surfaceDestroyed();
        mGLOffScreenSurface->dealloc();
        mGLOffScreenSurface= nullptr;
    }
    if (mVideoFrameQueue!= nullptr){
        mVideoFrameQueue->flush();
        if (mEncodeThread != nullptr) {
            mEncodeThread->join();
            delete mEncodeThread;
            mEncodeThread = nullptr;
        }
        delete mVideoFrameQueue;
        mVideoFrameQueue= nullptr;
    }

    if (mCopyCond != nullptr) {
        delete mCopyCond;
        mCopyCond = nullptr;
    }
    if (mCopyLock != nullptr) {
        delete mCopyLock;
        mCopyLock = nullptr;
    }

}

void SoftVideoEncoderAdapter::dealloc() {
    stop();
}

//纹理拷贝结束回调函数
void SoftVideoEncoderAdapter::onRenderFinish(void *buffer) {
    LOGI("onRenderFinish");


    VideoFrame *videoFrame = new VideoFrame();
    videoFrame->buffer = static_cast<byte *>(buffer);
    videoFrame->size = mByteSize;
    videoFrame->timeInMills = currentTimeMills() - mStartTimeInMills;
    LOGI("before videoFrame enqueue timeMills:%ld",videoFrame->timeInMills);
    mVideoFrameQueue->put(videoFrame);
    LOGI("after videoFrame enqueue");
    LOGE("mCopyCond signal");
    mCopyCond->signal();
}
