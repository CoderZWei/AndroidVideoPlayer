package com.example.zw.videoplayer;

import com.example.zw.videoplayer.opengl.MyGLSurfaceView;

public class MediaPlayer {
    private PlayerWrapper mPlayerWrapper;

    public void initPlayer(String path){
        mPlayerWrapper=PlayerWrapper.getInstance();
        mPlayerWrapper.init(path);
    }

    public void setGLSurfaceView(MyGLSurfaceView glSurfaceView){
        if(mPlayerWrapper!=null){
            mPlayerWrapper.setGLSurfaceView(glSurfaceView);
        }
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
