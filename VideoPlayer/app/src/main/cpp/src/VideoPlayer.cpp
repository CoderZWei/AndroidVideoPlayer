//
// Created by zw on 2019/10/1.
//



#include "VideoPlayer.h"
#include "log.h"

VideoPlayer::VideoPlayer(PlayStatus *pStatus,CallBack *callback) {
    this->playStatus=pStatus;
    this->callBack=callback;
    this->videoPktQueue=new PacketQueue(playStatus);
    pthread_mutex_init(&codecMutex,NULL);
}


VideoPlayer::~VideoPlayer() {

}
void *playVideo(void* data){

    VideoPlayer *videoPlayer= static_cast<VideoPlayer*>(data);
    while (videoPlayer->playStatus!=NULL && videoPlayer->playStatus->getPlayStatus()){
        if(videoPlayer->videoPktQueue->getQueueSize()==0){//正在加载
            av_usleep(1000*100);
            continue;
        }
        AVPacket *avPacket=av_packet_alloc();
        if(videoPlayer->videoPktQueue->getAVPacket(avPacket)!=0){//取packet失败
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket=NULL;
            continue;
        }
        pthread_mutex_lock(&videoPlayer->codecMutex);
        if(avcodec_send_packet(videoPlayer->avCodecCtx,avPacket)!=0){
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket=NULL;
            pthread_mutex_unlock(&videoPlayer->codecMutex);
            continue;
        }
        AVFrame *avFrame=av_frame_alloc();
        if(avcodec_receive_frame(videoPlayer->avCodecCtx,avFrame)!=0){
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame=NULL;
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket=NULL;
            pthread_mutex_unlock(&videoPlayer->codecMutex);
            continue;
        }
        //视频帧格式为yuv420p，则无需转换
        if(avFrame->format==AV_PIX_FMT_YUV420P){
            videoPlayer->callBack->onCallRenderYUV(
                    CHILD_THREAD,
                    videoPlayer->avCodecCtx->width,
                    videoPlayer->avCodecCtx->height,
                    avFrame->data[0],
                    avFrame->data[1],
                    avFrame->data[2]
                    );
        } else{
            AVFrame *pFrameYUV420P=av_frame_alloc();
            int num=av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                    videoPlayer->avCodecCtx->width,
                    videoPlayer->avCodecCtx->height,
                    1
            );
            uint8_t *buffer= static_cast<uint8_t *>(av_malloc(num* sizeof(uint8_t)));
            av_image_fill_arrays(
                    pFrameYUV420P->data,
                    pFrameYUV420P->linesize,
                    buffer,
                    AV_PIX_FMT_YUV420P,
                    videoPlayer->avCodecCtx->width,
                    videoPlayer->avCodecCtx->height,
                    1
            );
            SwsContext *sws_ctx=sws_getContext(
                    videoPlayer->avCodecCtx->width,
                    videoPlayer->avCodecCtx->height,
                    videoPlayer->avCodecCtx->pix_fmt,
                    videoPlayer->avCodecCtx->width,
                    videoPlayer->avCodecCtx->height,
                    AV_PIX_FMT_YUV420P,
                    SWS_BICUBIC,
                    NULL,NULL,NULL
            );
            //初始化失败
            if(!sws_ctx){
                ALOGD("zw_debug:swsCtx初始化失败");
                av_frame_free(&pFrameYUV420P);
                av_free(pFrameYUV420P);
                av_free(buffer);
                pthread_mutex_unlock(&videoPlayer->codecMutex);
                continue;
            }
            sws_scale(sws_ctx,
                      reinterpret_cast<const uint8_t *const *>(avFrame->data),
                      avFrame->linesize,
                      0,
                      avFrame->height,
                      pFrameYUV420P->data,
                      pFrameYUV420P->linesize
            );
            videoPlayer->callBack->onCallRenderYUV(
                    CHILD_THREAD,
                    videoPlayer->avCodecCtx->width,
                    videoPlayer->avCodecCtx->height,
                    pFrameYUV420P->data[0],
                    pFrameYUV420P->data[1],
                    pFrameYUV420P->data[2]
                    );
            av_frame_free(&pFrameYUV420P);
            av_free(pFrameYUV420P);
            av_free(buffer);
            sws_freeContext(sws_ctx);
        }
        av_free(avFrame);
        avFrame = NULL;
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
        pthread_mutex_unlock(&videoPlayer->codecMutex);
    }
    return 0;
}
void VideoPlayer::play() {
    if(playStatus!=NULL && playStatus->getPlayStatus()){
        pthread_create(&threadPlay,NULL,playVideo,this);
    }
}

void VideoPlayer::pause() {

}

void VideoPlayer::resume() {

}

void VideoPlayer::release() {

}

