//
// Created by zw on 2019/10/1.
//

#ifndef VIDEOPLAYER_CALLBACK_H
#define VIDEOPLAYER_CALLBACK_H

#include "jni.h"
#define MAIN_THREAD 0
#define CHILD_THREAD 1
class CallBack {
public:
    CallBack(_JavaVM *javaVM, JNIEnv *env, jobject *obj);
    ~CallBack();
    void onCallRenderYUV(int threadType,int width,int height,uint8_t *fy,uint8_t *fu,uint8_t *fv);
    void onCallTimeUpdate(int threadType,int currentTime,int totalTime);
    void onCallComplete(int threadType);
    void onCallStop(int threadType);

    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jObj;

    jmethodID jmid_renderYUV;
    jmethodID jmid_timeUpdate;
    jmethodID jmid_complete;
    jmethodID jmid_stop;
};

#endif //VIDEOPLAYER_CALLBACK_H
