package com.example.zw.videoplayer;

import com.example.zw.videoplayer.listener.OnTimeUpdateListener;
import com.example.zw.videoplayer.opengl.MyGLSurfaceView;

public class MediaPlayer {
    private PlayerWrapper mPlayerWrapper;
    private OnTimeUpdateListener mOnTimeUpdateListener;

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
        if(mPlayerWrapper!=null){
            mPlayerWrapper.pause();
        }
    }

    public void resumePlay(){
        if(mPlayerWrapper!=null){
            mPlayerWrapper.resume();
        }
    }

    public void seekPlay(int timeSec){
        if(mPlayerWrapper!=null){
            mPlayerWrapper.seek(timeSec);
        }
    }

    public void setOnTimeUpdateListener(OnTimeUpdateListener onTimeUpdateListener) {
        if(mPlayerWrapper!=null){
            mPlayerWrapper.setOnTimeUpdateListener(onTimeUpdateListener);
        }
    }

    public int getDuration(){
        if(mPlayerWrapper!=null){
            return mPlayerWrapper.getDurationTime();
        }
        return 0;
    }
}
