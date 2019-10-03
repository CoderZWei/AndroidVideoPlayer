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
}

FfmpegPlayer::~FfmpegPlayer() {
    pthread_mutex_destroy(&initMutex);
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
    audioPlayer->play();
    videoPlayer->audioPlayer = audioPlayer;
    videoPlayer->play();
    int videoCount = 0, audioCount = 0;
    while (playStatus != NULL && playStatus->getPlayStatus()) {
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
                    break;
                }
            }
        }
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




