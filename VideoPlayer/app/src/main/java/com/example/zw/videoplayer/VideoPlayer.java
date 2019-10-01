package com.example.zw.videoplayer;

public class VideoPlayer {
    private PlayerWrapper mPlayerWrapper;

    public void initPlayer(String path){
        mPlayerWrapper=PlayerWrapper.getInstance();
        mPlayerWrapper.init(path);
    }


    public void startPlay(){

    }
    public void pausePlay(){

    }

    public void resumePlay(){

    }
}
