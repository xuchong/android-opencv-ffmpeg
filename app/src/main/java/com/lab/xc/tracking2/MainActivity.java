package com.lab.xc.tracking2;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    String TAG="XC_OPENCV_FFMPEG";

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //your video path in phone, my example: /sdcard/DCIM/Camera/1.mp4
        String result=readVideo("/sdcard/DCIM/Camera/1.mp4");
        TextView tv= (TextView) findViewById(R.id.textView);
        tv.setText(result);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String readVideo(String filePath);
}
