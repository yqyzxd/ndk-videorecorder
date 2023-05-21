//
// Created by wind on 2023/5/6.
//

#ifndef NDK_HANDLER_HANDLER_H
#define NDK_HANDLER_HANDLER_H



#include "looper.h"

class Handler{
public:

    Handler(Looper* looper);
    virtual ~Handler();

    virtual void handleMessage(Message* message)=0;
    virtual void sendMessage(Message* message);

private:
    Looper* mLooper;
};


#endif //NDK_HANDLER_HANDLER_H
