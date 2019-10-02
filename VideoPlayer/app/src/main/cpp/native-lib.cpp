#include <jni.h>
#include <string>
#include "src/FfmpegPlayer.h"
#include "src/PlayStatus.h"
#include "src/CallBack.h"

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
        playStatus = new PlayStatus();
        callBack=new CallBack(javaVM,env,&instance);
        ffmpegPlayer = new FfmpegPlayer(playStatus,callBack);
        ffmpegPlayer->init(path);
    }
    env->ReleaseStringUTFChars(path_, path);
}

void *playCallBack(void *data) {
    FfmpegPlayer *ffPlayer = (FfmpegPlayer *) data;
    ffPlayer->startPlay();
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_zw_videoplayer_PlayerWrapper_nativePlay(JNIEnv *env, jobject instance) {
    if (ffmpegPlayer != NULL) {
        pthread_create(&threadPlay, NULL, playCallBack,ffmpegPlayer);
    }
}

