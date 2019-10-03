package com.example.zw.videoplayer;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.example.zw.videoplayer.opengl.MyGLSurfaceView;
import com.example.zw.videoplayer.util.PermissionUtils;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private static final String VIDEOPATH = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "01.avi";
    private MediaPlayer mMediaPlayer;
    private MyGLSurfaceView mSurfaceView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        PermissionUtils.requestPermissionsIfNeed(this);
        mSurfaceView=(MyGLSurfaceView)findViewById(R.id.surface_view);
        mMediaPlayer = new MediaPlayer();
        mMediaPlayer.initPlayer(VIDEOPATH);
        mMediaPlayer.setGLSurfaceView(mSurfaceView);
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        mMediaPlayer.startPlay();

    }

}
