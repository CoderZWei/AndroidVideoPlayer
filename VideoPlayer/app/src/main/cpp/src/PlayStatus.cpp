//
// Created by zw on 2019/10/1.
//

#include "PlayStatus.h"

PlayStatus::PlayStatus() {}

PlayStatus::~PlayStatus() {

}

bool PlayStatus::getPauseStatus() {
    return pauseStatus;
}

void PlayStatus::setPauseStatus(bool pauseStatus) {
    this->pauseStatus=pauseStatus;
}

bool PlayStatus::getSeekStatus() {
    return seekStatus;
}

void PlayStatus::setSeekStatus(bool seekStatus) {
    this->seekStatus=seekStatus;
}

bool PlayStatus::getLoadStatus() {
    return loadStatus;
}

void PlayStatus::setLoadStatus(bool loadStatus) {
    this->loadStatus=loadStatus;
}

bool PlayStatus::getPlayStatus() {
    return playStatus;
}

void PlayStatus::setPlayStatus(bool playStatus) {
    this->playStatus=playStatus;
}
