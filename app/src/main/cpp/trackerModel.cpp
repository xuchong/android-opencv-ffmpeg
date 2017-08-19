/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：trackerModel.CPP
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

namespace cv
{

/*
 *  TrackerModel
 */

TrackerModel::TrackerModel()
{
  stateEstimator = Ptr<TrackerStateEstimator>();
  maxCMLength = 1;
}

TrackerModel::~TrackerModel()
{

}

bool TrackerModel::setTrackerStateEstimator( Ptr<TrackerStateEstimator> trackerStateEstimator )
{
  if( stateEstimator != 0 )
  {
    return false;
  }

  stateEstimator = trackerStateEstimator;
  return true;
}

Ptr<TrackerStateEstimator> TrackerModel::getTrackerStateEstimator() const
{
  return stateEstimator;
}

void TrackerModel::modelEstimation( const std::vector<Mat>& responses )
{
  modelEstimationImpl( responses );

}

void TrackerModel::clearCurrentConfidenceMap()
{
  currentConfidenceMap.clear();
}

void TrackerModel::modelUpdate()
{
  modelUpdateImpl();

  if( maxCMLength != -1 && (int) confidenceMaps.size() >= maxCMLength - 1 )
  {
    int l = maxCMLength / 2;
    confidenceMaps.erase( confidenceMaps.begin(), confidenceMaps.begin() + l );
  }
  if( maxCMLength != -1 && (int) trajectory.size() >= maxCMLength - 1 )
  {
    int l = maxCMLength / 2;
    trajectory.erase( trajectory.begin(), trajectory.begin() + l );
  }
  confidenceMaps.push_back( currentConfidenceMap );
  stateEstimator->update( confidenceMaps );

  clearCurrentConfidenceMap();

}

bool TrackerModel::runStateEstimator()
{
  if( stateEstimator == 0 )
  {
    CV_Error( -1, "Tracker state estimator is not setted" );
    return false;
  }
  Ptr<TrackerTargetState> targetState = stateEstimator->estimate( confidenceMaps );
  if( targetState == 0 )
    return false;

  setLastTargetState( targetState );
  return true;
}

void TrackerModel::setLastTargetState( const Ptr<TrackerTargetState>& lastTargetState )
{
  trajectory.push_back( lastTargetState );
}

Ptr<TrackerTargetState> TrackerModel::getLastTargetState() const
{
  return trajectory.back();
}

const std::vector<ConfidenceMap>& TrackerModel::getConfidenceMaps() const
{
  return confidenceMaps;
}

const ConfidenceMap& TrackerModel::getLastConfidenceMap() const
{
  return confidenceMaps.back();
}

/*
 *  TrackerTargetState
 */

Point2f TrackerTargetState::getTargetPosition() const
{
  return targetPosition;
}

void TrackerTargetState::setTargetPosition( const Point2f& position )
{
  targetPosition = position;
}

int TrackerTargetState::getTargetWidth() const
{
  return targetWidth;
}

void TrackerTargetState::setTargetWidth( int width )
{
  targetWidth = width;
}
int TrackerTargetState::getTargetHeight() const
{
  return targetHeight;
}

void TrackerTargetState::setTargetHeight( int height )
{
  targetHeight = height;
}

} /* namespace cv */
