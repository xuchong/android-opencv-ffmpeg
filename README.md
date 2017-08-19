# android-opencv-ffmpeg
Android NDK JNI, use opencv compiled with ffmpeg, so you can use videocapture to decode videos, such as mp4,avi and so on.

> This project shows you how to use opencv compiled with ffmpeg to read video, include mp4 ,avi and so on.
> The most important things about how to read video using opencv in android device is compiling opencv with ffmpeg.

__My environments: Windows 10, android studio 2.3.3, android sdk 23, ndk 14r, opencv 310, ffmpeg 3.3(latest, 2017/8/20).__

## Tutorials (3 Steps)

Prepare:
Download related source project,including ndk, opencv source(from github), ffmpeg (from github).

Learning step by step.

1. Compile opencv for android.

> There has many tutorials to tell you how to compile both in Windows and Linux.Just google it by youself. :earth_asia:

> I used this one. [Compile opencv310 for android](https://zami0xzami.wordpress.com/2016/03/17/building-opencv-for-android-from-source/)

2. Import Opencv310 as your module in android project. [Test your compiled opencv library](http://blog.csdn.net/yu544324974/article/details/51848037)

3. Compile ffmpeg for android.
> I offer my uncleaned bash in the project.It can build dynamic libraries for android arch x86 and arm, you can modify for other versions.
Put them in ffmpeg source project. Just change its path carefully. Be patient, guys!

4. Last but not least,  opencv with ffmpeg.

> You need re-compiling your opencv with ffmpeg. Are you kidding me? I'm not. Seriously, above steps are just preparations,
> you need cd to your opencv source dir. For offical opencv doesn't support android with ffmpeg, you can just get opencv without 
> ffmpeg from that. 
First, modify root CMakeLists.txt in opencv source project to let it support android with ffmpeg.
Second, modify OpenCVFindLibsVideo.cmake in __cmake__ directory.Samely, for support android with ffmpeg.
Finally, recompile your opencv. It supports ffmepg now. Enjoy yourself.

## My Test for using opencv with ffmpeg
If videocapture is opened. Then screen gets success info. Otherwise, screen gets the path of the video. After this, you can process your
other task  just as simple opencv(without ffmpeg) for android.

## After All, Libraries you need to get.
[Libraries]()



