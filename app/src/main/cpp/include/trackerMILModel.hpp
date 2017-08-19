/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：TrackerMILMODEL.HPP
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
#ifndef __OPENCV_TRACKER_MIL_MODEL_HPP__
#define __OPENCV_TRACKER_MIL_MODEL_HPP__

#include "opencv2/core.hpp"

namespace cv
{

/**
 * \brief Implementation of TrackerModel for MIL algorithm
 */
class TrackerMILModel : public TrackerModel
{
 public:
  enum
  {
    MODE_POSITIVE = 1,  	// mode for positive features
    MODE_NEGATIVE = 2,  	// mode for negative features
    MODE_ESTIMATON = 3	// mode for estimation step
  };

  /**
   * \brief Constructor
   * \param boundingBox The first boundingBox
   */
  TrackerMILModel( const Rect& boundingBox );

  /**
   * \brief Destructor
   */
  ~TrackerMILModel()
  {
  }
  ;

  /**
   * \brief Set the mode
   */
  void setMode( int trainingMode, const std::vector<Mat>& samples );

  /**
   * \brief Create the ConfidenceMap from a list of responses
   * \param responses The list of the responses
   * \param confidenceMap The output
   */
  void responseToConfidenceMap( const std::vector<Mat>& responses, ConfidenceMap& confidenceMap );

 protected:
  void modelEstimationImpl( const std::vector<Mat>& responses );
  void modelUpdateImpl();

 private:
  int mode;
  std::vector<Mat> currentSample;

  int width;	//initial width of the boundingBox
  int height;  //initial height of the boundingBox
};

} /* namespace cv */

#endif
