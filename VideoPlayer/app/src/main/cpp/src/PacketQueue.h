//
// Created by zw on 2019/10/1.
//

#ifndef VIDEOPLAYER_PACKETQUEUE_H
#define VIDEOPLAYER_PACKETQUEUE_H

#include "queue"
#include "pthread.h"
#include "PlayStatus.h"
extern "C"{
#include "libavcodec/avcodec.h"
};
class PacketQueue{
public:
    PacketQueue();

    PacketQueue(PlayStatus *playStatus);

    ~PacketQueue();
    std::queue<AVPacket*>pktQueue;
    pthread_mutex_t packetMutex;
    pthread_cond_t packetCond;
    PlayStatus *playStatus=NULL;

    void putAVPacket(AVPacket *packet);
    int getAVPacket(AVPacket *packet);
    int getQueueSize();
    void clearQueue();
    void noticeQueue();

};
#endif //VIDEOPLAYER_PACKETQUEUE_H
