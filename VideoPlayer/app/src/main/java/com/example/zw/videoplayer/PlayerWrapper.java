package com.example.zw.videoplayer;

import android.util.Log;

import com.example.zw.videoplayer.opengl.MyGLSurfaceView;

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

    public void setGLSurfaceView(MyGLSurfaceView glSurfaceView){
        this.mGLSurfaceView=glSurfaceView;
    }

    public void onCallRenderYUV(int width,int height,byte[]y,byte[]u,byte[]v){
        Log.d("zw_debug","获取到yuv");
        if(mGLSurfaceView!=null){
            mGLSurfaceView.setYUVData(width,height,y,u,v);
        }
    }

    private native void nativeInitPlayer(String path);
    private native void nativePlay();
}
