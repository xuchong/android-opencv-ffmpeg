/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：trackerBoostingModel.HPP
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
#ifndef __OPENCV_TRACKER_BOOSTING_MODEL_HPP__
#define __OPENCV_TRACKER_BOOSTING_MODEL_HPP__

#include "precomp.hpp"
#include "opencv2/core.hpp"

namespace cv
{

/**
 * \brief Implementation of TrackerModel for BOOSTING algorithm
 */
class TrackerBoostingModel : public TrackerModel
{
 public:
  enum
  {
    MODE_POSITIVE = 1,    // mode for positive features
    MODE_NEGATIVE = 2,    // mode for negative features
    MODE_CLASSIFY = 3    // mode for classify step
  };
  /**
   * \brief Constructor
   * \param boundingBox The first boundingBox
   */
  TrackerBoostingModel( const Rect& boundingBox );

  /**
   * \brief Destructor
   */
  ~TrackerBoostingModel()
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

  /**
   * \brief return the selected weak classifiers for the detect
   * @return the selected weak classifiers
   */
  std::vector<int> getSelectedWeakClassifier();

 protected:
  void modelEstimationImpl( const std::vector<Mat>& responses );
  void modelUpdateImpl();

 private:

  std::vector<Mat> currentSample;

  int mode;
};

} /* namespace cv */

#endif
