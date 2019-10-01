//
// Created by zw on 2019/10/1.
//

#include "CallBack.h"

CallBack::CallBack(_JavaVM *javaVM, JNIEnv *env, jobject *obj) {
    this->javaVM=javaVM;
    this->jniEnv=env;
    this->jObj=*obj;

    jclass jlz=jniEnv->GetObjectClass(jObj);
    if(!jlz){
        return ;
    }
}

CallBack::~CallBack() {

}


