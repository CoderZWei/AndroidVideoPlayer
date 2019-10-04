//
// Created by zw on 2019/10/1.
//

#include "CallBack.h"
#include "log.h"

CallBack::CallBack(_JavaVM *javaVM1, JNIEnv *env, jobject *obj) {
    this->javaVM=javaVM1;
    this->jniEnv=env;
    this->jObj=*obj;
    this->jObj = env->NewGlobalRef(jObj);

    jclass jlz=jniEnv->GetObjectClass(jObj);
    if(!jlz){
        ALOGD("zw_debug:error");
        return ;
    }

    this->jmid_renderYUV=env->GetMethodID(jlz,"onCallRenderYUV","(II[B[B[B)V");
    this->jmid_timeUpdate=env->GetMethodID(jlz,"onCallTimeUpdate","(II)V");
    //this->jmid_renderYUV=env->GetMethodID(jlz,"onCallRenderYUV", "()V");
}

CallBack::~CallBack() {

}

void CallBack::onCallRenderYUV(int threadType,int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv) {
    JNIEnv *jniEnvLocal;
    //主线程调用
    if(threadType==MAIN_THREAD){
        //jniEnv->CallVoidMethod(jObj,jmid_renderYUV,width,height,fy,fu,fv);
        jniEnvLocal=jniEnv;
    }
    //子线程
    else{
        if(javaVM->AttachCurrentThread(&jniEnvLocal,0)!=JNI_OK){
            ALOGD("zw_debug:call onCallRenderYUV worng");
            return;
        }
    }
    jbyteArray y=jniEnvLocal->NewByteArray(width*height);
    jniEnvLocal->SetByteArrayRegion(y,0,width*height, reinterpret_cast<const jbyte *>(fy));
    jbyteArray u=jniEnvLocal->NewByteArray(width*height/4);
    jniEnvLocal->SetByteArrayRegion(u,0,width*height/4, reinterpret_cast<const jbyte *>(fu));
    jbyteArray v=jniEnvLocal->NewByteArray(width*height/4);
    jniEnvLocal->SetByteArrayRegion(v,0,width*height/4, reinterpret_cast<const jbyte *>(fv));
    jniEnvLocal->CallVoidMethod(jObj,jmid_renderYUV,width,height,y,u,v);

    jniEnvLocal->DeleteLocalRef(y);
    jniEnvLocal->DeleteLocalRef(u);
    jniEnvLocal->DeleteLocalRef(v);
    if(threadType==CHILD_THREAD){
        javaVM->DetachCurrentThread();
    }
}

void CallBack::onCallTimeUpdate(int threadType, int currentTime, int totalTime) {
    JNIEnv *jniEnvLocal;
    //主线程调用
    if(threadType==MAIN_THREAD){
        //jniEnv->CallVoidMethod(jObj,jmid_renderYUV,width,height,fy,fu,fv);
        jniEnvLocal=jniEnv;
    }
        //子线程
    else{
        if(javaVM->AttachCurrentThread(&jniEnvLocal,0)!=JNI_OK){
            ALOGD("zw_debug:call onCallRenderYUV worng");
            return;
        }
    }
    jniEnvLocal->CallVoidMethod(jObj, jmid_timeUpdate,currentTime,totalTime );
    if(threadType==CHILD_THREAD){
        javaVM->DetachCurrentThread();
    }
}


