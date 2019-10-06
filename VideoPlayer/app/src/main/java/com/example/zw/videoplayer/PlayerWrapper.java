package com.example.zw.videoplayer;

import android.util.Log;

import com.example.zw.videoplayer.listener.OnCompleteListener;
import com.example.zw.videoplayer.listener.OnStopListener;
import com.example.zw.videoplayer.listener.OnTimeUpdateListener;
import com.example.zw.videoplayer.opengl.MyGLSurfaceView;
import com.example.zw.videoplayer.util.TimeInfoBean;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;

public class PlayerWrapper {
    static {
        System.loadLibrary("native-lib");
    }
    private static PlayerWrapper mPlayerWrapper;
    private MyGLSurfaceView mGLSurfaceView;
    private ThreadPoolExecutor mThreadPoolExecutor;
    private OnTimeUpdateListener mOnTimeUpdateListener;
    private OnCompleteListener mOnCompleteListener;
    private OnStopListener mOnStopListener;
    private TimeInfoBean mTimeInfoBean;

    public static PlayerWrapper getInstance(){
        if(mPlayerWrapper==null){
            synchronized (PlayerWrapper.class){
                if(mPlayerWrapper==null){
                    mPlayerWrapper=new PlayerWrapper();
                }
            }
        }
        return mPlayerWrapper;
    }

    public void init(final String path) {
        if(mThreadPoolExecutor==null){
            mThreadPoolExecutor=new ThreadPoolExecutor(5,5,2,TimeUnit.SECONDS,new LinkedBlockingQueue<Runnable>(10));
        }
        mThreadPoolExecutor.execute(new Runnable() {
            @Override
            public void run() {
                nativeInitPlayer(path);
            }
        });
    }

    public void play(){
        mThreadPoolExecutor.execute(new Runnable() {
            @Override
            public void run() {
                nativePlay();
            }
        });
    }

    public void pause(){
        mThreadPoolExecutor.execute(new Runnable() {
            @Override
            public void run() {
                nativePause();
            }
        });
    }

    public void resume() {
        mThreadPoolExecutor.execute(new Runnable() {
            @Override
            public void run() {
                nativeResume();
            }
        });
    }

    public void seek(final int timeSec){
        mThreadPoolExecutor.execute(new Runnable() {
            @Override
            public void run() {
                nativeSeek(timeSec);
            }
        });
    }

    public void stop(){
//        mThreadPoolExecutor.execute(new Runnable() {
//            @Override
//            public void run() {
//                nativeStop();
//            }
//        });
        mTimeInfoBean=null;

        nativeStop();
    }

    public void setGLSurfaceView(MyGLSurfaceView glSurfaceView){
        this.mGLSurfaceView=glSurfaceView;
    }

    public void onCallRenderYUV(int width,int height,byte[]y,byte[]u,byte[]v){
        //Log.d("zw_debug","获取到yuv");
        if(mGLSurfaceView!=null){
            mGLSurfaceView.setYUVData(width,height,y,u,v);
        }
    }

    public void setOnTimeUpdateListener(OnTimeUpdateListener onTimeUpdateListener) {
        this.mOnTimeUpdateListener = onTimeUpdateListener;
    }

    public void onCallTimeUpdate(int currentTime, int totalTime){
        if(this.mOnTimeUpdateListener!=null){
            if(mTimeInfoBean==null){
                mTimeInfoBean=new TimeInfoBean();
            }
            mTimeInfoBean.setCurrentTime(currentTime);
            mTimeInfoBean.setTotalTime(totalTime);
            mOnTimeUpdateListener.onTimeUpdate(mTimeInfoBean);
        }
    }

    public int getDurationTime(){
        return mTimeInfoBean.getTotalTime();
    }

    private native void nativeInitPlayer(String path);
    private native void nativePlay();
    private native void nativePause();
    private native void nativeResume();
    private native void nativeSeek(int timeSec);
    private native void nativeStop();

    public void setOnCompleteListener(OnCompleteListener onCompleteListener) {
        this.mOnCompleteListener = onCompleteListener;
    }

    public void onCallComplete(){
        if(mOnCompleteListener!=null){
            mOnCompleteListener.onComplete();
        }
    }

    public void setOnStopListener(OnStopListener onStopListener) {
        this.mOnStopListener = onStopListener;
    }

    public void onCallStop(){
        if(mOnStopListener!=null){
            mOnStopListener.onStop();
        }
    }
}
