/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：roiSelector.HPP
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
#include "precomp.hpp"
#include "opencv2/highgui.hpp"
using namespace std;
namespace cv {
  void ROISelector::mouseHandler(int event, int x, int y, int flags, void *param){
      ROISelector *self =static_cast<ROISelector*>(param);
      self->opencv_mouse_callback(event,x,y,flags,param);
  }

  void ROISelector::opencv_mouse_callback( int event, int x, int y, int , void *param ){
    handlerT * data = (handlerT*)param;
    switch( event ){
      // update the selected bounding box
      case EVENT_MOUSEMOVE:
        if( data->isDrawing ){
          if(data->drawFromCenter){
            data->box.width = 2*(x-data->center.x)/*data->box.x*/;
            data->box.height = 2*(y-data->center.y)/*data->box.y*/;
            data->box.x = data->center.x-data->box.width/2.0;
            data->box.y = data->center.y-data->box.height/2.0;
          }else{
            data->box.width = x-data->box.x;
            data->box.height = y-data->box.y;
          }
        }
      break;

      // start to select the bounding box
      case EVENT_LBUTTONDOWN:
        data->isDrawing = true;
        data->box = cvRect( x, y, 0, 0 );
        data->center = Point2f((float)x,(float)y);
      break;

      // cleaning up the selected bounding box
      case EVENT_LBUTTONUP:
        data->isDrawing = false;
        if( data->box.width < 0 ){
          data->box.x += data->box.width;
          data->box.width *= -1;
        }
        if( data->box.height < 0 ){
          data->box.y += data->box.height;
          data->box.height *= -1;
        }
      break;
    }
  }

  Rect2d ROISelector::select(Mat img, bool fromCenter){
    return select("ROI selector", img, fromCenter);
  }

  Rect2d ROISelector::select(const cv::String& windowName, Mat img, bool showCrossair, bool fromCenter){

    key=0;

    // set the drawing mode
    selectorParams.drawFromCenter = fromCenter;

    // show the image and give feedback to user
    imshow(windowName,img);

    // copy the data, rectangle should be drawn in the fresh image
    selectorParams.image=img.clone();

    // select the object
    setMouseCallback( windowName, mouseHandler, (void *)&selectorParams );

    // end selection process on SPACE (32) BACKSPACE (27) or ENTER (13)
    while(!(key==32 || key==27 || key==13)){
      // draw the selected object
      rectangle(
        selectorParams.image,
        selectorParams.box,
        Scalar(255,0,0),2,1
      );

      // draw cross air in the middle of bounding box
      if(showCrossair){
        // horizontal line
        line(
          selectorParams.image,
          Point((int)selectorParams.box.x,(int)(selectorParams.box.y+selectorParams.box.height/2)),
          Point((int)(selectorParams.box.x+selectorParams.box.width),(int)(selectorParams.box.y+selectorParams.box.height/2)),
          Scalar(255,0,0),2,1
        );

        // vertical line
        line(
          selectorParams.image,
          Point((int)(selectorParams.box.x+selectorParams.box.width/2),(int)selectorParams.box.y),
          Point((int)(selectorParams.box.x+selectorParams.box.width/2),(int)(selectorParams.box.y+selectorParams.box.height)),
          Scalar(255,0,0),2,1
        );
      }

      // show the image bouding box
      imshow(windowName,selectorParams.image);

      // reset the image
      selectorParams.image=img.clone();

      //get keyboard event
      key=waitKey(1);
    }


    return selectorParams.box;
  }

  void ROISelector::select(const cv::String& windowName, Mat img, std::vector<Rect2d> & boundingBox, bool fromCenter){
    std::vector<Rect2d> box;
    Rect2d temp;
    key=0;

    // show notice to user
    printf("Select an object to track and then press SPACE or ENTER button!\n" );
    printf("Finish the selection process by pressing BACKSPACE button!\n" );

    // while key is not Backspace
    while(key!=27){
      temp=select(windowName, img, true, fromCenter);
      if(temp.width>0 && temp.height>0)
        box.push_back(temp);
    }
    boundingBox=box;
  }

  ROISelector _selector;
  Rect2d selectROI(Mat img, bool fromCenter){
    return _selector.select("ROI selector", img, true, fromCenter);
  };

  Rect2d selectROI(const cv::String& windowName, Mat img, bool showCrossair, bool fromCenter){
    printf("Select an object to track and then press SPACE or ENTER button!\n" );
    return _selector.select(windowName,img, showCrossair, fromCenter);
  };

  void selectROI(const cv::String& windowName, Mat img, std::vector<Rect2d> & boundingBox, bool fromCenter){
    return _selector.select(windowName, img, boundingBox, fromCenter);
  }

} /* namespace cv */
