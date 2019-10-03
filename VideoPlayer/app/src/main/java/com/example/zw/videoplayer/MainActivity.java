package com.example.zw.videoplayer;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import com.example.zw.videoplayer.opengl.MyGLSurfaceView;
import com.example.zw.videoplayer.util.PermissionUtils;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private static final String VIDEOPATH = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "01.avi";
    private MediaPlayer mMediaPlayer;
    private MyGLSurfaceView mSurfaceView;
    private ImageView mImgStatus;
    boolean isPlaying=true;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        PermissionUtils.requestPermissionsIfNeed(this);
        mSurfaceView=(MyGLSurfaceView)findViewById(R.id.surface_view);
        mImgStatus=(ImageView)findViewById(R.id.img_status);
        mMediaPlayer = new MediaPlayer();
        mMediaPlayer.initPlayer(VIDEOPATH);
        mMediaPlayer.setGLSurfaceView(mSurfaceView);
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        mMediaPlayer.startPlay();

        mSurfaceView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(MainActivity.this,"click",Toast.LENGTH_SHORT).show();
                if(isPlaying){
                    mImgStatus.setBackground(getResources().getDrawable(R.mipmap.pause));
                    mMediaPlayer.pausePlay();
                    isPlaying=false;
                }else {
                    mImgStatus.setBackground(getResources().getDrawable(R.mipmap.play));
                    mMediaPlayer.resumePlay();
                    isPlaying=true;
                }


            }
        });
    }

}
