package com.example.zw.videoplayer.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.example.zw.videoplayer.listener.OnRenderListener;

public class MyGLSurfaceView extends GLSurfaceView {
    private MyRender mRender;
    public MyGLSurfaceView(Context context) {
        this(context,null);
    }

    public MyGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        mRender=new MyRender(context);
        setRenderer(mRender);
        //主动刷新模式
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        mRender.setOnRenderListener(new OnRenderListener() {
            @Override
            public void onRender() {
                //requestRender();
            }
        });
    }

    public void setYUVData(int width,int height,byte[]y,byte[]u,byte[]v){
        if(mRender!=null){
            mRender.setYUVRenderData(width,height,y,u,v);
            requestRender();
        }
    }


}
