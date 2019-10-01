#include <jni.h>
#include <string>
#include "src/FfmpegPlayer.h"
#include "src/PlayStatus.h"
#include "src/CallBack.h"

FfmpegPlayer *ffmpegPlayer = NULL;
PlayStatus *playStatus = NULL;
CallBack *callBack = NULL;
pthread_t threadPlay;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_zw_videoplayer_PlayerWrapper_nativeInitPlayer(JNIEnv *env, jobject instance,
                                                               jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    if (ffmpegPlayer == NULL) {
        playStatus = new PlayStatus();
        ffmpegPlayer = new FfmpegPlayer(playStatus);
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

