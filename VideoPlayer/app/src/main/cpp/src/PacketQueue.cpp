//
// Created by zw on 2019/10/1.
//

#include "PacketQueue.h"

PacketQueue::PacketQueue() {}

PacketQueue::PacketQueue(PlayStatus *playStatus) : playStatus(playStatus) {
    pthread_mutex_init(&packetMutex, NULL);
    pthread_cond_init(&packetCond, NULL);
    this->playStatus = playStatus;
}

PacketQueue::~PacketQueue() {

}

void PacketQueue::putAVPacket(AVPacket *packet) {
    pthread_mutex_lock(&packetMutex);
    pktQueue.push(packet);
    pthread_cond_signal(&packetCond);
    pthread_mutex_unlock(&packetMutex);
}

int PacketQueue::getAVPacket(AVPacket *packet) {
    pthread_mutex_lock(&packetMutex);
    while (playStatus != NULL && playStatus->getPlayStatus() != false) {
        if (pktQueue.size() > 0) {
            AVPacket *avPacket = pktQueue.front();
            //产生一个新的引用，第一个参数是dst,第二个参数是src,返回0表示成功
            if (av_packet_ref(packet, avPacket) == 0) {
                this->pktQueue.pop();
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            break;
        } else {
            pthread_cond_wait(&packetCond, &packetMutex);
        }
    }
    pthread_mutex_unlock(&packetMutex);
    return 0;
}

int PacketQueue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&packetMutex);
    size = pktQueue.size();
    pthread_mutex_unlock(&packetMutex);
    return size;
}

void PacketQueue::clearQueue() {
    pthread_cond_signal(&packetCond);
    pthread_mutex_lock(&packetMutex);
    while (!pktQueue.empty()) {
        AVPacket *packet = pktQueue.front();
        pktQueue.pop();
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
    pthread_mutex_unlock(&packetMutex);
}

void PacketQueue::noticeQueue() {
    pthread_cond_signal(&packetCond);
}


