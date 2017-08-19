/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：trackerMILModel.CPP
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
#include "trackerMILModel.hpp"

/**
 * TrackerMILModel
 */

namespace cv
{

TrackerMILModel::TrackerMILModel( const Rect& boundingBox )
{
  currentSample.clear();
  mode = MODE_POSITIVE;
  width = boundingBox.width;
  height = boundingBox.height;

  Ptr<TrackerStateEstimatorMILBoosting::TrackerMILTargetState> initState = Ptr<TrackerStateEstimatorMILBoosting::TrackerMILTargetState>(
      new TrackerStateEstimatorMILBoosting::TrackerMILTargetState( Point2f( (float)boundingBox.x, (float)boundingBox.y ), boundingBox.width, boundingBox.height,
                                                                   true, Mat() ) );
  trajectory.push_back( initState );
}

void TrackerMILModel::responseToConfidenceMap( const std::vector<Mat>& responses, ConfidenceMap& confidenceMap )
{
  if( currentSample.empty() )
  {
    CV_Error( -1, "The samples in Model estimation are empty" );
    return;
  }

  for ( size_t i = 0; i < responses.size(); i++ )
  {
    //for each column (one sample) there are #num_feature
    //get informations from currentSample
    for ( int j = 0; j < responses.at( i ).cols; j++ )
    {

      Size currentSize;
      Point currentOfs;
      currentSample.at( j ).locateROI( currentSize, currentOfs );
      bool foreground = false;
      if( mode == MODE_POSITIVE || mode == MODE_ESTIMATON )
      {
        foreground = true;
      }
      else if( mode == MODE_NEGATIVE )
      {
        foreground = false;
      }

      //get the column of the HAAR responses
      Mat singleResponse = responses.at( i ).col( j );

      //create the state
      Ptr<TrackerStateEstimatorMILBoosting::TrackerMILTargetState> currentState = Ptr<TrackerStateEstimatorMILBoosting::TrackerMILTargetState>(
          new TrackerStateEstimatorMILBoosting::TrackerMILTargetState( currentOfs, width, height, foreground, singleResponse ) );

      confidenceMap.push_back( std::make_pair( currentState, 0.0f ) );

    }

  }
}

void TrackerMILModel::modelEstimationImpl( const std::vector<Mat>& responses )
{
  responseToConfidenceMap( responses, currentConfidenceMap );

}

void TrackerMILModel::modelUpdateImpl()
{

}

void TrackerMILModel::setMode( int trainingMode, const std::vector<Mat>& samples )
{
  currentSample.clear();
  currentSample = samples;

  mode = trainingMode;
}

}
