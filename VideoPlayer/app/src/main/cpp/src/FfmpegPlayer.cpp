//
// Created by zw on 2019/9/30.
//

#include "FfmpegPlayer.h"
#include "log.h"

FfmpegPlayer::FfmpegPlayer() {
    pthread_mutex_init(&initMutex, NULL);
}

int avformat_callback(void *ctx) {
//    FfmpegPlayer *fFmpeg = (FfmpegPlayer *) ctx;
//    if(fFmpeg->playStatus->getPlayStatus()== false)
//    {
//        return AVERROR_EOF;
//    }
    return 0;
}

void FfmpegPlayer::init(const char *url) {
    pthread_mutex_lock(&initMutex);
    this->url = url;
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    pFormatCtx->interrupt_callback.callback = avformat_callback;
    pFormatCtx->interrupt_callback.opaque = this;
    if(avformat_open_input(&pFormatCtx,this->url,NULL,NULL)!=0){
        ALOGE("zw_debug:can't open url:%s",this->url);
        exit= true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        ALOGE("zw_debug:can't find stream info:%s",this->url);
        exit= true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    for(int i=0;i<pFormatCtx->nb_streams;i++){
        if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
            ALOGE("zw_debug:video stream");
        }else if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            ALOGE("zw_debug:audio stream");
        }
    }
    pthread_mutex_unlock(&initMutex);
}

FfmpegPlayer::~FfmpegPlayer() {

}


