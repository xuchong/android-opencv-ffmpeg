package com.lab.xc.tracking2;

import android.app.AlertDialog;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Handler;
import android.provider.MediaStore;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.util.Log;

import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.videoio.VideoCapture;

import java.io.FileNotFoundException;
import java.io.InputStream;

import static org.opencv.imgproc.Imgproc.rectangle;

public class MainActivity extends AppCompatActivity {

    String TAG="xc_opencv_video";
    final int SELECT_VIDEO=100;
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    private Bitmap image;
    private ImageView iv;
    private android.graphics.Rect imageBounds;
    float heightRatio,widthRatio;
    private Thread bg=null;
    private Point leftTop=null,rightDown=null;
    private double[] roi=new double[4];
    private VideoCapture cap;
    private String filePath=null;
    private Mat im,outm;
    private Bitmap bm;
    private boolean stop=false;
    //声明一个AlertDialog构造器
    private AlertDialog.Builder builder;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //TextView tv = (TextView) findViewById(R.id.sample_text);
        //tv.setText(stringFromJNI("/sdcard/DCIM/Camera/1.mp4"));
        //Log.d(TAG,tv.getText().toString());
        iv = (ImageView) findViewById(R.id.imageView1);

        im=new Mat();

        iv.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                //convert coordinator
                final int index = motionEvent.getActionIndex();
                final float[] coords = new float[] { motionEvent.getX(index), motionEvent.getY(index) };
                Matrix matrix = new Matrix();
                iv.getImageMatrix().invert(matrix);
                matrix.postTranslate(iv.getScrollX(), iv.getScrollY());
                matrix.mapPoints(coords);
                if(leftTop==null)
                {
                    leftTop=new Point(coords[0],coords[1]);
                    return false;
                }
                if(rightDown==null)
                {
                    rightDown=new Point(coords[0],coords[1]);
                    roi[0]=leftTop.x;
                    roi[1]=leftTop.y;
                    roi[2]=rightDown.x-leftTop.x;
                    roi[3]=rightDown.y-leftTop.y;
                    initTracker(im.getNativeObjAddr(),roi);
                    rectangle(im,leftTop,rightDown,new Scalar(255,0,0),1);
                    Utils.matToBitmap(im, bm);
                    iv.setImageBitmap(bm);
                    return false;
                }
                return false;
            }
        });
        final Button button=(Button)findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(!stop) {
                    stop=true;
                    button.setText("Stop");
                    if(bg==null||!bg.isAlive()) {
                        bg = new Thread(new Runnable() {
                            @Override
                            public void run() {
                                while (stop) {
                                    boolean flag = getFrame();
                                    if (flag && !im.empty()) {
                                        roi = tracking(im.getNativeObjAddr(), roi);
                                        leftTop.x = roi[0];
                                        leftTop.y = roi[1];
                                        rightDown.x = roi[0] + roi[2];
                                        rightDown.y = roi[1] + roi[3];
                                        Log.d(TAG, String.format("%f %f %f %f, %d %d", leftTop.x, leftTop.y, rightDown.x, rightDown.y, im.cols(), im.rows()));
                                        rectangle(im, leftTop, rightDown, new Scalar(255, 0, 0), 1);
                                        Utils.matToBitmap(im, bm);
                                        MainActivity.this.runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                //UI
                                                iv.setImageBitmap(bm);
                                            }
                                        });
                                    }
                                }
                            }
                        });
                        bg.start();
                    }
                    else
                    {
                        stop=true;
//                        Log.e(TAG,"thread not")
//                        builder=new AlertDialog.Builder(MainActivity.this);
//                        builder.setCancelable(true);
//                        AlertDialog dialog=builder.create();
//                        dialog.show();
                    }
                }
                else {
                    stop = false;
                    button.setText("Start");
                }
            }
        });

        final Button selectbutton=(Button)findViewById(R.id.selectbutton);
        selectbutton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent photoPickerIntent = new Intent(Intent.ACTION_PICK, MediaStore.Video.Media.EXTERNAL_CONTENT_URI);
                photoPickerIntent.setType("*/*");
                startActivityForResult(photoPickerIntent, SELECT_VIDEO);
//                Intent videoPickerIntent = new Intent(Intent.ACTION_GET_CONTENT);
//                videoPickerIntent.setType("*/*");
//                videoPickerIntent.putExtra(Intent.EXTRA_MIME_TYPES,new String[]{"image/*", "video/*"});
//                startActivityForResult(videoPickerIntent,SELECT_VIDEO);
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent imageReturnedIntent) {
        super.onActivityResult(requestCode, resultCode, imageReturnedIntent);
        switch(requestCode) {
            case SELECT_VIDEO:
                if(resultCode == RESULT_OK){
                    Uri selectedVideo = imageReturnedIntent.getData();
                    if (selectedVideo.toString().contains("video")) {
                        //handle video
                        Log.d(TAG,selectedVideo.getPath());

                        filePath=getPath(selectedVideo);
                        Log.d(TAG,filePath);
                        if(cap!=null&&cap.isOpened())
                        {
                            cap.release();
                        }
                        getFrame();
                        outm=new Mat(im.rows(),im.cols(),im.type());
                        bm = Bitmap.createBitmap(im.cols(), im.rows(), Bitmap.Config.ARGB_8888);

                        Utils.matToBitmap(im, bm);
                        iv.setImageBitmap(bm);
                    }
                }
        }
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        stop=true;
        if(cap!=null&&cap.isOpened())
        {
            cap.release();
            cap=null;
        }
        leftTop=null;
        rightDown=null;
    }

    private String getPath(Uri uri) {
        String[] projection = { MediaStore.Video.Media.DATA };
        Cursor cursor = getContentResolver().query(uri, projection, null, null, null);
        if (cursor != null) {
            // HERE YOU WILL GET A NULLPOINTER IF CURSOR IS NULL
            // THIS CAN BE, IF YOU USED OI FILE MANAGER FOR PICKING THE MEDIA
            int column_index = cursor
                    .getColumnIndexOrThrow(MediaStore.Video.Media.DATA);
            cursor.moveToFirst();
            return cursor.getString(column_index);
        } else
            return null;
    }
    private boolean getFrame()
    {
        if(cap==null||!cap.isOpened())
        {
            cap=new VideoCapture(filePath);
        }
        if(cap.isOpened())
        {
            if(cap.read(im))
                return true;
            else{
                cap.release();
                Log.d(TAG,"video end ");
                return false;
            }
        }else
        {
            Log.d(TAG,"can't open! ");
            return false;
        }

    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String readVideo(String filePath);
    public native void convertGray(long matAddressInput,long matAddressOutput);
    public native void initTracker(long matAddressInput,double[] rect);
    public native double[] tracking(long matAddressInput,double[] rect);

}
