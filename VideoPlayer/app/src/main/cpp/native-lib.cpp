#include <jni.h>
#include <string>
#include "src/FfmpegPlayer.h"
#include "src/PlayStatus.h"
#include "src/CallBack.h"
#include "src/log.h"

FfmpegPlayer *ffmpegPlayer = NULL;
PlayStatus *playStatus = NULL;
CallBack *callBack = NULL;
pthread_t threadPlay;
_JavaVM *javaVM=NULL;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if(vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        return result;
    }
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_zw_videoplayer_PlayerWrapper_nativeInitPlayer(JNIEnv *env, jobject instance,
                                                               jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    if (ffmpegPlayer == NULL) {
        ALOGD("zw_debug:ffplayer is null");
        playStatus = new PlayStatus();
        callBack=new CallBack(javaVM,env,&instance);
        ffmpegPlayer = new FfmpegPlayer(playStatus,callBack);
        ffmpegPlayer->init(path);
    } else{
        ALOGD("zw_debug:ffplayer not null");
    }
    env->ReleaseStringUTFChars(path_, path);
}

void *playCallBack(void *data) {
    FfmpegPlayer *ffPlayer = (FfmpegPlayer *) data;
    ffPlayer->startPlay();
    //pthread_exit(&threadPlay);
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_zw_videoplayer_PlayerWrapper_nativePlay(JNIEnv *env, jobject instance) {
    if (ffmpegPlayer != NULL) {
        pthread_create(&threadPlay, NULL, playCallBack,ffmpegPlayer);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_zw_videoplayer_PlayerWrapper_nativePause(JNIEnv *env, jobject instance) {
    if(ffmpegPlayer!=NULL){
        ffmpegPlayer->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_zw_videoplayer_PlayerWrapper_nativeResume(JNIEnv *env, jobject instance) {
    if(ffmpegPlayer!=NULL){
        ffmpegPlayer->resume();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_zw_videoplayer_PlayerWrapper_nativeSeek(JNIEnv *env, jobject instance,
                                                         jint timeSec) {
    if(ffmpegPlayer!=NULL){
        ffmpegPlayer->seek(timeSec);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_zw_videoplayer_PlayerWrapper_nativeStop(JNIEnv *env, jobject instance) {
    if(ffmpegPlayer!=NULL){
        ffmpegPlayer->release();
        pthread_join(threadPlay,NULL);
        delete(ffmpegPlayer);
        ffmpegPlayer=NULL;
    }
    if(playStatus!=NULL){
        delete(playStatus);
        playStatus=NULL;
    }
    if(callBack!=NULL){
        delete(callBack);
        callBack=NULL;
    }
    jclass clz = env->GetObjectClass(instance);
    jmethodID jmid_stop = env->GetMethodID(clz,  "onCallStop", "()V");
    env->CallVoidMethod(instance,jmid_stop);
}