//
// Created by zw on 2019/9/30.
//

#include "FfmpegPlayer.h"
#include "log.h"

FfmpegPlayer::FfmpegPlayer() {

}

FfmpegPlayer::FfmpegPlayer(PlayStatus *pStatus, CallBack *callback) {
    this->playStatus = pStatus;
    this->callBack=callback;
    this->exit = false;

    pthread_mutex_init(&initMutex, NULL);
    pthread_mutex_init(&seekMutex, NULL);
}

FfmpegPlayer::~FfmpegPlayer() {
    pthread_mutex_destroy(&initMutex);
    pthread_mutex_destroy(&seekMutex);
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
    if (avformat_open_input(&pFormatCtx, this->url, NULL, NULL) != 0) {
        ALOGD("zw_debug:can't open url:%s", this->url);
        exit = true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        ALOGD("zw_debug:can't find stream info:%s", this->url);
        exit = true;
        pthread_mutex_unlock(&initMutex);
        return;
    }
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            ALOGD("zw_debug:video stream");
            if (videoPlayer == NULL) {
                videoPlayer = new VideoPlayer(playStatus,callBack);
                videoPlayer->streamIndex = i;
                videoPlayer->codecPar = pFormatCtx->streams[i]->codecpar;
                videoPlayer->timeBase = pFormatCtx->streams[i]->time_base;
            }
        } else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            ALOGD("zw_debug:audio stream");
            if (audioPlayer == NULL) {
                audioPlayer = new AudioPlayer(playStatus,callBack,this->pFormatCtx->streams[i]->codecpar->sample_rate);
                audioPlayer->streamIndex = i;
                audioPlayer->codecPar = pFormatCtx->streams[i]->codecpar;
                audioPlayer->duration = pFormatCtx->duration / AV_TIME_BASE;
                audioPlayer->timeBase = pFormatCtx->streams[i]->time_base;
                this->duration = audioPlayer->duration;
            }
        }
    }
    if(audioPlayer!=NULL){
        getCodecContext(audioPlayer->codecPar,&audioPlayer->avCodecCtx);
    }
    if(videoPlayer!=NULL){
        getCodecContext(videoPlayer->codecPar,&videoPlayer->avCodecCtx);
    }
    pthread_mutex_unlock(&initMutex);
}

void *ffPlayCallBack(void *data) {
    FfmpegPlayer *ffPlayer = (FfmpegPlayer *) data;
    ffPlayer->play();
    return 0;
}

void FfmpegPlayer::startPlay() {
    pthread_create(&playThread, NULL, ffPlayCallBack, this);
}

void FfmpegPlayer::play() {
    videoPlayer->audioPlayer = audioPlayer;
    audioPlayer->play();
    videoPlayer->play();
    int videoCount = 0, audioCount = 0;
    while (playStatus != NULL && playStatus->getPlayStatus()) {
        if(playStatus->getSeekStatus()){
            av_usleep(1000*100);
            continue;
        }
        if (audioPlayer->audioPktQueue->getQueueSize() > 50) {
            av_usleep(1000 * 100);
            continue;
        }
        AVPacket *avPacket = av_packet_alloc();
        if (av_read_frame(pFormatCtx, avPacket) == 0) {
            //解码视频流
            if (avPacket->stream_index == videoPlayer->streamIndex) {
                videoCount++;
                videoPlayer->videoPktQueue->putAVPacket(avPacket);
                //ALOGD("zw_debug:size %d", videoPlayer->videoPktQueue->getQueueSize());
            }
                //解码音频流
            else if (avPacket->stream_index == audioPlayer->streamIndex) {
                audioCount++;
                audioPlayer->audioPktQueue->putAVPacket(avPacket);
                //ALOGD("zw_debug:audio %d", audioCount);
            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
            }
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            while (playStatus != NULL && playStatus->getPlayStatus()) {
                if (audioPlayer->audioPktQueue->getQueueSize() > 0) {
                    av_usleep(1000 * 100);
                    continue;
                } else {
                        if(playStatus->getSeekStatus()== false){
                            av_usleep(1000*100);
                            playStatus->setPlayStatus(false);
                            //playStatus->setPlayStatus(false);
                        }
                        break;
                }
            }
            break;
        }
    }
    if(callBack!=NULL){

        callBack->onCallComplete(CHILD_THREAD);
    }
    exit = true;
    ALOGD("ze_debug:解码完成");
}

int FfmpegPlayer::getCodecContext(AVCodecParameters *codecpar, AVCodecContext **avCodecContext) {
    AVCodec *dec=avcodec_find_decoder(codecpar->codec_id);
    if(!dec){
        ALOGD("zw_debug:can't find decoder");
        exit= true;
        pthread_mutex_unlock(&initMutex);
        return -1;
    }
    *avCodecContext=avcodec_alloc_context3(dec);
    if(avcodec_parameters_to_context(*avCodecContext,codecpar)<0){
        ALOGE("zw:can't fill decodecctx");
        exit= true;
        pthread_mutex_unlock(&initMutex);
        return -1;
    }
    if(avcodec_open2(*avCodecContext,dec,0)!=0){
        ALOGE("zw:can't open audio sreams");
        exit= true;
        pthread_mutex_unlock(&initMutex);
        return -1;
    }
    return 0;
}

void FfmpegPlayer::pause() {
    if(playStatus!=NULL){
        playStatus->setPauseStatus(true);
    }
    if(audioPlayer!=NULL){
        audioPlayer->pause();
    }
}

void FfmpegPlayer::resume() {
    if(playStatus!=NULL){
        playStatus->setPauseStatus(false);
    }
    if(audioPlayer!=NULL){
        audioPlayer->resume();
    }
}

void FfmpegPlayer::seek(int64_t timeSec) {
    if(duration<0 || timeSec>duration){
        return;
    }
    playStatus->setSeekStatus(true);
    pthread_mutex_lock(&seekMutex);
    int64_t rel=timeSec*AV_TIME_BASE;
    avformat_seek_file(pFormatCtx, -1, INT64_MIN, rel, INT64_MAX, 0);
    if(audioPlayer!=NULL){
        audioPlayer->audioPktQueue->clearQueue();
        audioPlayer->clock=0;
        audioPlayer->lastTime=0;
        pthread_mutex_lock(&audioPlayer->codecMutex);
        avcodec_flush_buffers(audioPlayer->avCodecCtx);
        pthread_mutex_unlock(&audioPlayer->codecMutex);
    }
    if(videoPlayer!=NULL){
        videoPlayer->videoPktQueue->clearQueue();
        videoPlayer->clock=0;
        pthread_mutex_lock(&videoPlayer->codecMutex);
        avcodec_flush_buffers(videoPlayer->avCodecCtx);
        pthread_mutex_unlock(&videoPlayer->codecMutex);
    }
    pthread_mutex_unlock(&seekMutex);
    playStatus->setSeekStatus(false);
}

void FfmpegPlayer::release() {
    playStatus->setPlayStatus(false);
    pthread_join(playThread,NULL);
    pthread_mutex_lock(&initMutex);
    if(audioPlayer!=NULL){
        audioPlayer->release();
        delete(audioPlayer);
        audioPlayer=NULL;
    }
    if(videoPlayer!=NULL){
        videoPlayer->release();
        delete(videoPlayer);
        videoPlayer=NULL;
    }
    if(pFormatCtx!=NULL){
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx=NULL;
    }
    if(callBack!=NULL){
        callBack=NULL;
    }
    if(playStatus!=NULL){
        playStatus=NULL;
    }
    pthread_mutex_unlock(&initMutex);

}




