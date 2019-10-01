//
// Created by zw on 2019/10/1.
//

#ifndef VIDEOPLAYER_PLAYSTATUS_H
#define VIDEOPLAYER_PLAYSTATUS_H

class PlayStatus {
public:
    PlayStatus();

    ~PlayStatus();

    bool getPauseStatus();

    void setPauseStatus(bool pauseStatus);

    bool getSeekStatus();

    void setSeekStatus(bool seekStatus);

    bool getLoadStatus();

    void setLoadStatus(bool loadStatus);

    bool getPlayStatus();

    void setPlayStatus(bool playStatus);

    bool playStatus = true;
    bool loadStatus = true;
    bool seekStatus = true;
    bool pauseStatus = true;

};

#endif //VIDEOPLAYER_PLAYSTATUS_H
