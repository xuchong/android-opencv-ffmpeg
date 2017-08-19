/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：trackerBoostingModel.CPP
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
#include "trackerBoostingModel.hpp"

/**
 * TrackerBoostingModel
 */

namespace cv
{

TrackerBoostingModel::TrackerBoostingModel( const Rect& boundingBox )
{

  mode = MODE_POSITIVE;

  Ptr<TrackerStateEstimatorAdaBoosting::TrackerAdaBoostingTargetState> initState =
      Ptr<TrackerStateEstimatorAdaBoosting::TrackerAdaBoostingTargetState>(
          new TrackerStateEstimatorAdaBoosting::TrackerAdaBoostingTargetState( Point2f( (float)boundingBox.x, (float)boundingBox.y ), boundingBox.width,
                                                                               boundingBox.height, true, Mat() ) );
  trajectory.push_back( initState );
  maxCMLength = 10;
}

void TrackerBoostingModel::modelEstimationImpl( const std::vector<Mat>& responses )
{
  responseToConfidenceMap( responses, currentConfidenceMap );
}

void TrackerBoostingModel::modelUpdateImpl()
{

}

void TrackerBoostingModel::setMode( int trainingMode, const std::vector<Mat>& samples )
{
  currentSample.clear();
  currentSample = samples;

  mode = trainingMode;
}

std::vector<int> TrackerBoostingModel::getSelectedWeakClassifier()
{
  return stateEstimator.staticCast<TrackerStateEstimatorAdaBoosting>()->computeSelectedWeakClassifier();
}

void TrackerBoostingModel::responseToConfidenceMap( const std::vector<Mat>& responses, ConfidenceMap& confidenceMap )
{
  if( currentSample.empty() )
  {
    CV_Error( -1, "The samples in Model estimation are empty" );
    return;
  }

  for ( size_t i = 0; i < currentSample.size(); i++ )
  {

    Size currentSize;
    Point currentOfs;
    currentSample.at( i ).locateROI( currentSize, currentOfs );
    bool foreground = false;
    if( mode == MODE_POSITIVE || mode == MODE_CLASSIFY )
    {
      foreground = true;
    }
    else if( mode == MODE_NEGATIVE )
    {
      foreground = false;
    }
    const Mat resp = responses[0].col( (int)i );

    //create the state
    Ptr<TrackerStateEstimatorAdaBoosting::TrackerAdaBoostingTargetState> currentState = Ptr<
        TrackerStateEstimatorAdaBoosting::TrackerAdaBoostingTargetState>(
        new TrackerStateEstimatorAdaBoosting::TrackerAdaBoostingTargetState( currentOfs, currentSample.at( i ).cols, currentSample.at( i ).rows,
                                                                             foreground, resp ) );

    confidenceMap.push_back( std::make_pair( currentState, 0.0f ) );

  }
}

}
