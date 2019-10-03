//
// Created by zw on 2019/10/1.
//

#ifndef VIDEOPLAYER_AUDIOPLAYER_H
#define VIDEOPLAYER_AUDIOPLAYER_H

#include "PlayStatus.h"
#include "PacketQueue.h"
#include "CallBack.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include <libavutil/time.h>
#include"SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"
};

class AudioPlayer {
public:
    AudioPlayer(PlayStatus *pStatus, CallBack *callback,int spRate);

    ~AudioPlayer();

    void play();

    void pause();

    void resume();

    void release();

    void initOpenSLES();
    int resampleAudio();
    int getCurrentSampleRateForOpenSLES(int samplerate);

    int streamIndex = -1;
    AVCodecContext *avCodecCtx = NULL;
    AVCodecParameters *codecPar = NULL;
    pthread_t threadPlay;
    pthread_mutex_t codecMutex;
    PlayStatus *playStatus = NULL;
    CallBack *callBack = NULL;

    int duration = 0;
    AVRational timeBase;
    double clock;//总的播放时长
    double nowTime;//当前播放时长
    double lastTime;//上一次调用时间

    PacketQueue *audioPktQueue = NULL;

    AVPacket *avPacket=NULL;
    AVFrame *avFrame=NULL;
    int sampleRate=0;

    uint8_t  *buffer=NULL;
    int dataSize=0;
    int ret=0;
    //OpenSL ES相关变量
    //引擎接口
    SLObjectItf engineObject=NULL;
    SLEngineItf engineEngine=NULL;
    //混音器
    SLObjectItf outputMixObject=NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb=NULL;
    SLEnvironmentalReverbSettings reverbSettings=SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    //pcm
    SLObjectItf  pcmPlayerObject=NULL;
    SLPlayItf pcmPlayerPlay=NULL;
    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue=NULL;

};

#endif //VIDEOPLAYER_AUDIOPLAYER_H
