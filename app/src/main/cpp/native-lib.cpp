#include <jni.h>
#include <android/log.h>
#include <string>

#include <opencv2/opencv.hpp>
using namespace cv;

extern "C"
JNIEXPORT jstring JNICALL
Java_com_lab_xc_tracking2_MainActivity_readVideo(
        JNIEnv *env,
        jobject object, jstring pFileName) {

    char *videoFileName = (char *) (env)->GetStringUTFChars(pFileName, NULL);
    VideoCapture capture(videoFileName);
    if (!capture.isOpened()) {
        capture.release();
        return env->NewStringUTF(videoFileName);
    }
    capture.release();
    std::string hello = "Read Video Success!!!";
    return env->NewStringUTF(hello.c_str());
}
