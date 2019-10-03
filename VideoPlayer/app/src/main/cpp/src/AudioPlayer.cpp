//
// Created by zw on 2019/10/1.
//

#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(PlayStatus *pStatus, CallBack *callback, int spRate) {
    this->playStatus = pStatus;
    this->callBack = callback;
    this->sampleRate = spRate;
    this->audioPktQueue = new PacketQueue(playStatus);
    this->buffer=(uint8_t*)av_malloc(sampleRate*2*2);
    pthread_mutex_init(&codecMutex, NULL);
}


AudioPlayer::~AudioPlayer() {

}

void *playAudio(void *data) {
    AudioPlayer *audioPlayer = (AudioPlayer *) data;
    audioPlayer->initOpenSLES();
    return 0;
}

void AudioPlayer::play() {
    if (playStatus != NULL && playStatus->getPlayStatus()) {
        pthread_create(&threadPlay, NULL, playAudio, this);
    }
}


void AudioPlayer::pause() {
    if(pcmPlayerPlay!=NULL){
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay,SL_PLAYSTATE_PAUSED);
    }
}

void AudioPlayer::resume() {
    if(pcmPlayerPlay!=NULL){
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay,SL_PLAYSTATE_PLAYING);
    }
}

void AudioPlayer::release() {

}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    AudioPlayer *audioPlayer = (AudioPlayer *) context;
    if (audioPlayer != NULL) {
        int buffersize = audioPlayer->resampleAudio();
        if (buffersize > 0) {
            // ALOGD("zw:buffersize_%d",buffersize);
            audioPlayer->clock += buffersize / ((double) (audioPlayer->sampleRate * 2 * 2));
            if (audioPlayer->clock - audioPlayer->lastTime >= 0.1) {
                audioPlayer->lastTime = audioPlayer->clock;
                //回调应用层
                //audioPlayer->callBack->onCallTimeUpdate(CHILD_THREAD,audioPlayer->clock, audioPlayer->duration);
            }
            //往里填充数据
            (*audioPlayer->pcmBufferQueue)->Enqueue(audioPlayer->pcmBufferQueue,
                                                    (char *) audioPlayer->buffer, buffersize);
        }
    }
}


void AudioPlayer::initOpenSLES() {
    SLresult result;
    result = slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    //第二步，创建混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    (void) result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    (void) result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void) result;
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, 0};

    // 第三步，配置PCM格式信息
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};

    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            2,//2个声道（立体声）
            static_cast<SLuint32>(getCurrentSampleRateForOpenSLES(sampleRate)),//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};

    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSnk, 1,
                                       ids, req);
    //初始化播放器
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);

    //    得到接口后调用  获取Player接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);

//    注册回调缓冲区 获取缓冲队列接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    //缓冲接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);
//    获取播放状态接口
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    pcmBufferCallBack(pcmBufferQueue, this);

}

int AudioPlayer::resampleAudio() {
    dataSize = 0;//要是不赋值0的话音频播放结束也会继续更新时间
    while (playStatus != NULL && playStatus->getPlayStatus()) {
//        if(playStatus->getSeekStatus()== true){//seek状态
//            av_usleep(1000*100);
//            continue;
//        }
        if (audioPktQueue->getQueueSize() == 0) {//加载中
            if (playStatus->getLoadStatus() == false) {
                playStatus->setLoadStatus(true);
                //callBack->onCallLoad(CHILD_THREAD,true);
            }
            av_usleep(1000 * 100);
            continue;
        } else {
            if (playStatus->getLoadStatus() == true) {
                playStatus->setLoadStatus(false);
                //callBack->onCallLoad(CHILD_THREAD, false);
            }
        }
        avPacket = av_packet_alloc();
        if (audioPktQueue->getAVPacket(avPacket) != 0) {//获取失败
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        pthread_mutex_lock(&codecMutex);
        ret = avcodec_send_packet(avCodecCtx, avPacket);//返回0表示成功
        if (ret != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            pthread_mutex_unlock(&codecMutex);
            continue;
        }
        avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecCtx, avFrame);//返回0表示成功
        if (ret == 0) {
            //处理异常情况 声道数和声道布局
            if (avFrame->channels > 0 && avFrame->channel_layout == 0) {
                avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
            } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
                avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
            }
            SwrContext *swr_ctx;
            swr_ctx = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO,
                    AV_SAMPLE_FMT_S16,
                    avFrame->sample_rate,
                    avFrame->channel_layout,
                    (AVSampleFormat) avFrame->format,
                    avFrame->sample_rate,
                    NULL, NULL
            );
            if (!swr_ctx || swr_init(swr_ctx) < 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;
                if (swr_ctx != NULL) {
                    swr_free(&swr_ctx);
                    swr_ctx = NULL;
                }
                pthread_mutex_unlock(&codecMutex);
                continue;
            }
            //返回值是转换后的PCM的采样个数
            int nb = swr_convert(
                    swr_ctx,
                    &buffer,
                    avFrame->nb_samples,
                    (const uint8_t **) avFrame->data,
                    avFrame->nb_samples
            );
            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            dataSize = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            // ALOGD("zw_data_size:%d",dataSize);

            nowTime = avFrame->pts * av_q2d(timeBase);//帧数乘以每一帧的时间
            if (nowTime < clock) {
                nowTime = clock;
            }
            clock = nowTime;

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            if (swr_ctx != NULL) {
                swr_free(&swr_ctx);
                swr_ctx = NULL;
            }
            pthread_mutex_unlock(&codecMutex);
            break;//如果读成功就直接退出
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            pthread_mutex_unlock(&codecMutex);
            continue;
        }
    }
    return dataSize;
}

int AudioPlayer::getCurrentSampleRateForOpenSLES(int samplerate) {
    int rate = 0;
    switch (samplerate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}


