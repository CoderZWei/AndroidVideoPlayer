package com.example.zw.videoplayer;

public class MediaPlayer {
    private PlayerWrapper mPlayerWrapper;

    public void initPlayer(String path){
        mPlayerWrapper=PlayerWrapper.getInstance();
        mPlayerWrapper.init(path);
    }


    public void startPlay(){
        if(mPlayerWrapper!=null){
            mPlayerWrapper.play();
        }
    }
    public void pausePlay(){

    }

    public void resumePlay(){

    }
}
