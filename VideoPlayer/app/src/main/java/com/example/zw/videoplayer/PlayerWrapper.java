package com.example.zw.videoplayer;

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
    private ThreadPoolExecutor mThreadPoolExecutor;
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
        mThreadPoolExecutor=new ThreadPoolExecutor(3,3,2,TimeUnit.SECONDS,new LinkedBlockingQueue<Runnable>(10));
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

    private native void nativeInitPlayer(String path);
    private native void nativePlay();
}
