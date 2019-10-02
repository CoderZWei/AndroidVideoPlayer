//
// Created by zw on 2019/9/30.
//

#ifndef VIDEOPLAYER_FFMPEGPLAYER_H
#define VIDEOPLAYER_FFMPEGPLAYER_H

#include "pthread.h"
#include "VideoPlayer.h"
#include "AudioPlayer.h"
extern "C"{
#include "libavformat/avformat.h"
#include <libavutil/time.h>
};

class FfmpegPlayer{
public:
    FfmpegPlayer();
    FfmpegPlayer(PlayStatus *pStatus,CallBack *callback);
    ~FfmpegPlayer();
    void init(const char *url);
    void startPlay();
    void play();
    int getCodecContext(AVCodecParameters *codecpar,AVCodecContext **avCodecContext);

    const char *url=NULL;
    bool exit= false;
    int duration=0;
    PlayStatus *playStatus=NULL;
    CallBack *callBack=NULL;
    pthread_mutex_t initMutex;
    AVFormatContext *pFormatCtx=NULL;
    VideoPlayer *videoPlayer=NULL;
    AudioPlayer *audioPlayer=NULL;

    pthread_t playThread;


};
#endif //VIDEOPLAYER_FFMPEGPLAYER_H
