package com.example.zw.videoplayer;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import com.example.zw.videoplayer.util.PermissionUtils;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private static final String VIDEOPATH = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "demo.mov";
    private VideoPlayer mVideoPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        PermissionUtils.requestPermissionsIfNeed(this);
        mVideoPlayer = new VideoPlayer();
        mVideoPlayer.initPlayer(VIDEOPATH);

    }

}
