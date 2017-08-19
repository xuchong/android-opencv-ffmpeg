/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：utils.CPP
* 文件标识：见配置管理计划书
* 摘 要：简要描述本文件的内容
*
* 当前版本：1.01
* 作 者：Henry
* 完成日期：2017年08月12日
*
* 取代版本：1.00
* 原作者 ：HENRY
* 完成日期：2017年07月12日
*/
#include "utils.hpp"

#include <opencv2/opencv.hpp>

namespace OT {
    namespace Utils {
        bool hasFrame(cv::VideoCapture& capture) {
            bool hasNotQuit = ((char) cv::waitKey(1)) != 'q';
            bool hasAnotherFrame = capture.grab();
            return hasNotQuit && hasAnotherFrame;
        }

        void scale(cv::Mat& img, int maxDimension) {
            if (maxDimension == -1) {
                return;
            }
            if (maxDimension >= img.rows && maxDimension >= img.cols) {
                return;
            }

            double scale = (1.0 * maxDimension) / img.rows;
            if (img.cols > img.rows) {
                scale = (1.0 * maxDimension) / img.cols;
            }

            int newRows = img.rows * scale;
            int newCols = img.cols * scale;

            cv::resize(img, img, cv::Size(newCols, newRows));
        }
    }
}
