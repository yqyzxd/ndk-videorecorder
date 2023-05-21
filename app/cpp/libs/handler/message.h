//
// Created by wind on 2023/5/6.
//

#ifndef NDK_HANDLER_MESSAGE_H
#define NDK_HANDLER_MESSAGE_H
class Handler;
class Message {
public:
    int what;
    int arg1;
    int arg2;
    void* obj;

    Handler* target;
    Message* next;


public:
    Message(int what);
    Message(int what,int arg1,int arg2,void* obj);
    ~Message();



    void execute();
};


#endif //NDK_HANDLER_MESSAGE_H
