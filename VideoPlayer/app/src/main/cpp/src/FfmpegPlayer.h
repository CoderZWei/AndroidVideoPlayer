//
// Created by zw on 2019/9/30.
//

#ifndef VIDEOPLAYER_FFMPEGPLAYER_H
#define VIDEOPLAYER_FFMPEGPLAYER_H

#include "pthread.h"
extern "C"{
#include "libavformat/avformat.h"
};

class FfmpegPlayer{
public:
    FfmpegPlayer();
    ~FfmpegPlayer();
    void init(const char *url);

    const char *url=NULL;
    bool exit= false;
    pthread_mutex_t initMutex;
    AVFormatContext *pFormatCtx=NULL;

};
#endif //VIDEOPLAYER_FFMPEGPLAYER_H
