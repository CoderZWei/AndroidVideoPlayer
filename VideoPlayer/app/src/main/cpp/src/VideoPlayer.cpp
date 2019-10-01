//
// Created by zw on 2019/10/1.
//

#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(PlayStatus *pStatus) {
    this->playStatus=pStatus;
    this->videoPktQueue=new PacketQueue(playStatus);
}


VideoPlayer::~VideoPlayer() {

}

void VideoPlayer::play() {

}

void VideoPlayer::pause() {

}

void VideoPlayer::resume() {

}

void VideoPlayer::release() {

}


