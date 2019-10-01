//
// Created by zw on 2019/10/1.
//

#ifndef VIDEOPLAYER_CALLBACK_H
#define VIDEOPLAYER_CALLBACK_H

#include "jni.h"
class CallBack{
public:
    CallBack(_JavaVM *javaVM,JNIEnv *env,jobject *obj);
    ~CallBack();

    _JavaVM *javaVM=NULL;
    JNIEnv *jniEnv=NULL;
    jobject jObj;

};

#endif //VIDEOPLAYER_CALLBACK_H
