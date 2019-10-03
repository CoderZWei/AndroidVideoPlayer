//
// Created by zw on 2019/10/1.
//

#ifndef VIDEOPLAYER_VIDEOPLAYER_H
#define VIDEOPLAYER_VIDEOPLAYER_H

#include "PlayStatus.h"
#include "CallBack.h"
#include "AudioPlayer.h"
#include "PacketQueue.h"
extern "C"{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

class VideoPlayer{
public:
    VideoPlayer(PlayStatus *pStatus,CallBack *callback);
    ~VideoPlayer();
    void play();
    void pause();
    void resume();
    void release();
    double getFrameDiffTime(AVFrame *avFrame,AVPacket *avPacket);
    double getDelayTime(double diff);

    int streamIndex=-1;
    AVCodecContext *avCodecCtx=NULL;
    AVCodecParameters *codecPar=NULL;
    pthread_t  threadPlay;
    pthread_mutex_t codecMutex;
    AudioPlayer *audioPlayer=NULL;
    AVRational timeBase;
    double clock=0;
    double delayTime=0;
    double defaultDelayTime=0.04;

    PlayStatus *playStatus=NULL;
    CallBack *callBack=NULL;
    PacketQueue *videoPktQueue=NULL;

};

#endif //VIDEOPLAYER_VIDEOPLAYER_H
