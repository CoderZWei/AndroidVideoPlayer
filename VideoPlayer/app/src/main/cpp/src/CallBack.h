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

    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jObj;

    jmethodID jmid_renderYUV;

};

#endif //VIDEOPLAYER_CALLBACK_H
