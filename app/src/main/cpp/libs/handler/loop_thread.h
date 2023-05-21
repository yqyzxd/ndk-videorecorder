//
// Created by wind on 2023/5/6.
//

#ifndef NDK_HANDLER_LOOP_THREAD_H
#define NDK_HANDLER_LOOP_THREAD_H


#include "looper.h"
#include "../../utils/thread.h"

class LoopThread : public Thread{

private:
    Looper* mLooper;

public:
    LoopThread();
    virtual ~LoopThread();

    void run() override;
    virtual void quit();

    virtual Looper* getLooper();

};


#endif //NDK_HANDLER_LOOP_THREAD_H
