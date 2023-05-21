//
// Created by wind on 2023/5/6.
//

#include "message.h"
#include "handler.h"
Message::Message(int what) {
    this->what=what;
    this->arg1=0;
    this->arg2=0;
    this->obj= nullptr;
    next= nullptr;
}

Message::Message(int what, int arg1, int arg2, void *obj) {
    this->what=what;
    this->arg1=arg1;
    this->arg2=arg2;
    this->obj= obj;
    next= nullptr;
}
Message::~Message() {
    this->what=0;
    this->arg1=0;
    this->arg2=0;
    this->obj= nullptr;
    next= nullptr;
}

void Message::execute() {
    if (target!= nullptr){
        target->handleMessage(this);
    }

}