//
// Created by zw on 2019/10/1.
//

#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(PlayStatus *pStatus) {
    this->playStatus=pStatus;
    this->audioPktQueue=new PacketQueue(playStatus);
    pthread_mutex_init(&codecMutex,NULL);
}


AudioPlayer::~AudioPlayer() {

}

void AudioPlayer::play() {

}

void AudioPlayer::pause() {

}

void AudioPlayer::resume() {

}

void AudioPlayer::release() {

}

