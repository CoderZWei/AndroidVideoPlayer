package com.example.zw.videoplayer;

import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Toast;

import com.example.zw.videoplayer.listener.OnCompleteListener;
import com.example.zw.videoplayer.listener.OnStopListener;
import com.example.zw.videoplayer.listener.OnTimeUpdateListener;
import com.example.zw.videoplayer.opengl.MyGLSurfaceView;
import com.example.zw.videoplayer.util.PermissionUtils;
import com.example.zw.videoplayer.util.TimeInfoBean;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private static final String VIDEOPATH = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "demo1.mp4";
    //private static final String VIDEOPATH1 = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "3.avi";
    //private static final String VIDEOPATH1 = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "jiaoxue.avi";
    private static final String VIDEOPATH1 = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "img2video.mp4";
    int id=0;
    private MediaPlayer mMediaPlayer;
    private MyGLSurfaceView mSurfaceView;
    private ImageView mImgStatus;
    private SeekBar mSeekbar;
    private boolean isPlaying = true;
    private int mPosition;
    Button btnStart, btnStop;
    ImageView nextBtn;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        PermissionUtils.requestPermissionsIfNeed(this);
        mSurfaceView = (MyGLSurfaceView) findViewById(R.id.surface_view);
        mImgStatus = (ImageView) findViewById(R.id.img_status);
        mSeekbar = (SeekBar) findViewById(R.id.seekbar);
        nextBtn=(ImageView)findViewById(R.id.next_img);
        nextBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mMediaPlayer.stopPlay();
            }
        });

        mMediaPlayer = new MediaPlayer();
        mMediaPlayer.initWrapper();
        //initView();
        mMediaPlayer.initPlayer(VIDEOPATH);
        mMediaPlayer.setGLSurfaceView(mSurfaceView);
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        mMediaPlayer.startPlay();


        mMediaPlayer.setOnTimeUpdateListener(new OnTimeUpdateListener() {
            @Override
            public void onTimeUpdate(TimeInfoBean timeInfoBean) {
                Message message = Message.obtain();
                message.what = 1;
                message.obj = timeInfoBean;
                mHandler.sendMessage(message);
            }
        });
        mMediaPlayer.setOnCompleteListener(new OnCompleteListener() {
            @Override
            public void onComplete() {
                Log.d("zw_debug", "onComplete");
//                new Thread(new Runnable() {
//                    @Override
//                    public void run() {
//                        mMediaPlayer.stopPlay();
//                    }
//                }).start();

                //mMediaPlayer.initPlayer(VIDEOPATH);
//                try {
//                    Thread.sleep(1000);
//                    //mMediaPlayer.setGLSurfaceView(mSurfaceView);
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
//                mMediaPlayer.startPlay();
            }
        });

        mMediaPlayer.setOnStopListener(new OnStopListener() {
            @Override
            public void onStop() {
                Log.d("zw_debug", "onStop");
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        mMediaPlayer.initPlayer(VIDEOPATH1);
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        mMediaPlayer.startPlay();
                    }
                }).start();

            }
        });

//        mSurfaceView.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Toast.makeText(MainActivity.this, "click", Toast.LENGTH_SHORT).show();
//                if (isPlaying) {
//                    mImgStatus.setBackground(getResources().getDrawable(R.mipmap.pause));
//                    mMediaPlayer.pausePlay();
//                    isPlaying = false;
//                } else {
//                    mImgStatus.setBackground(getResources().getDrawable(R.mipmap.play));
//                    mMediaPlayer.resumePlay();
//                    isPlaying = true;
//                }
//            }
//        });

        mSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                mPosition = progress * mMediaPlayer.getDuration() / 100;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                mMediaPlayer.seekPlay(mPosition);
            }
        });
    }

    private void initView() {
        btnStart = (Button) findViewById(R.id.btn_start);
        btnStop = (Button) findViewById(R.id.btn_stop);
        btnStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(id%2==0){
                    mMediaPlayer.initPlayer(VIDEOPATH1);
                }else {
                    mMediaPlayer.initPlayer(VIDEOPATH);
                }
                id++;
                //mMediaPlayer.setGLSurfaceView(mSurfaceView);
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                mMediaPlayer.startPlay();
            }
        });
        btnStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mMediaPlayer.stopPlay();
            }
        });
    }

    Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == 1) {
                TimeInfoBean infoBean = (TimeInfoBean) msg.obj;
                mSeekbar.setProgress(infoBean.getCurrentTime() * 100 / infoBean.getTotalTime());
            }
        }
    };
}
