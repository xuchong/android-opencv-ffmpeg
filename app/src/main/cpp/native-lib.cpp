#include <jni.h>
#include <android/log.h>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include "tracking.hpp"
#include "utils.hpp"
using namespace cv;

Ptr<Tracker> trackerk = Tracker::create("BOOSTING");
Rect2d roi;

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
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lab_xc_tracking2_MainActivity_convertGray(
        JNIEnv *env,
        jobject object, jlong input, jlong output) {
    Mat &matInput = *(Mat *) input;
    Mat &matOutput = *(Mat *) output;

    matInput.copyTo(matOutput);
    putText(matOutput, "hello world", Point(50, 50), 1, 1, Scalar(200, 0, 10));

}

extern "C"
JNIEXPORT void JNICALL
Java_com_lab_xc_tracking2_MainActivity_initTracker(
        JNIEnv *env,
        jobject object, jlong input, jdoubleArray array) {
    Mat &matInput = *(Mat *) input;
    jdouble * rect=env->GetDoubleArrayElements(array,NULL);
    roi.x = rect[0];
    roi.y = rect[1];
    roi.width = rect[2];
    roi.height = rect[3];
    trackerk->init(matInput, roi);
}

extern "C"
JNIEXPORT jdoubleArray JNICALL
Java_com_lab_xc_tracking2_MainActivity_tracking(
        JNIEnv *env,
        jobject object, jlong input, jdoubleArray array) {
    Mat &matInput = *(Mat *) input;
    jdouble * rect=env->GetDoubleArrayElements(array,NULL);
    roi.x = rect[0];
    roi.y = rect[1];
    roi.width = rect[2];
    roi.height = rect[3];
     __android_log_print(ANDROID_LOG_INFO, "JNIMsg", "before:%f %f %f %f",roi.x,roi.y,roi.width,roi.height);
    trackerk->update(matInput, roi);
    __android_log_print(ANDROID_LOG_INFO, "JNIMsg", "end:%f %f %f %f",roi.x,roi.y,roi.width,roi.height);
    rect[0] = roi.x;
    rect[1] = roi.y;
    rect[2] = roi.width;
    rect[3] = roi.height;
    env->SetDoubleArrayRegion(array,0,4,rect);
    return array;
}
