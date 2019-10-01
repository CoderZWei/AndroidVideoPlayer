#include <jni.h>
#include <string>
#include "src/FfmpegPlayer.h"

FfmpegPlayer *ffmpegPlayer=NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_zw_videoplayer_PlayerWrapper_nativeInitPlayer(JNIEnv *env, jobject instance,
                                                               jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    if(ffmpegPlayer==NULL){
        ffmpegPlayer=new FfmpegPlayer();
        ffmpegPlayer->init(path);
    }


    env->ReleaseStringUTFChars(path_, path);
}