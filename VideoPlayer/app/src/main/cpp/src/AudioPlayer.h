//
// Created by zw on 2019/10/1.
//

#ifndef VIDEOPLAYER_AUDIOPLAYER_H
#define VIDEOPLAYER_AUDIOPLAYER_H

#include "PlayStatus.h"
#include "PacketQueue.h"
extern "C"{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include <libavutil/time.h>
};
class AudioPlayer{
public:
    AudioPlayer(PlayStatus *pStatus);
    ~AudioPlayer();
    void play();
    void pause();
    void resume();
    void release();

    int streamIndex=-1;
    AVCodecContext *avCodecCtx=NULL;
    AVCodecParameters *codecPar=NULL;
    pthread_t  threadPlay;
    pthread_mutex_t codecMutex;
    PlayStatus *playStatus=NULL;

    int duration=0;
    AVRational timeBase;
    double clock;//总的播放时长
    double nowTime;//当前播放时长
    double lastTime;//上一次调用时间

    PacketQueue *audioPktQueue=NULL;
};
#endif //VIDEOPLAYER_AUDIOPLAYER_H
