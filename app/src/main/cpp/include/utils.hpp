#ifndef cv_utils_h
#define cv_utils_h

#include <opencv2/opencv.hpp>

namespace OT {
    namespace Utils {
        /**
         * Check if there's another frame in the video capture. We do this by first checking if the user has quit (i.e. pressed
         * the "Q" key) and then trying to retrieve the next frame of the video.
         */
        bool hasFrame(cv::VideoCapture& capture);
        /**
         * Resize the image so that neither # rows nor # cols exceed maxDimension.
         * Preserve the aspect ratio though.
         * Set maxDimension = -1 if you don't want to do any scaling.
         */
        void scale(cv::Mat& img, int maxDimension);
    }
}

#endif /* cv_utils_h */
