/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：trackerFeatureSet.CPP
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
 *  TrackerFeatureSet
 */

/*
 * Constructor
 */
TrackerFeatureSet::TrackerFeatureSet()
{
  blockAddTrackerFeature = false;
}

/*
 * Destructor
 */
TrackerFeatureSet::~TrackerFeatureSet()
{

}

void TrackerFeatureSet::extraction( const std::vector<Mat>& images )
{

  clearResponses();
  responses.resize( features.size() );

  for ( size_t i = 0; i < features.size(); i++ )
  {
    Mat response;
    features[i].second->compute( images, response );
    responses[i] = response;
  }

  if( !blockAddTrackerFeature )
  {
    blockAddTrackerFeature = true;
  }
}

void TrackerFeatureSet::selection()
{

}

void TrackerFeatureSet::removeOutliers()
{

}

bool TrackerFeatureSet::addTrackerFeature( String trackerFeatureType )
{
  if( blockAddTrackerFeature )
  {
    return false;
  }
  Ptr<TrackerFeature> feature = TrackerFeature::create( trackerFeatureType );

  if( feature == 0 )
  {
    return false;
  }

  features.push_back( std::make_pair( trackerFeatureType, feature ) );

  return true;
}

bool TrackerFeatureSet::addTrackerFeature( Ptr<TrackerFeature>& feature )
{
  if( blockAddTrackerFeature )
  {
    return false;
  }

  String trackerFeatureType = feature->getClassName();
  features.push_back( std::make_pair( trackerFeatureType, feature ) );

  return true;
}

const std::vector<std::pair<String, Ptr<TrackerFeature> > >& TrackerFeatureSet::getTrackerFeature() const
{
  return features;
}

const std::vector<Mat>& TrackerFeatureSet::getResponses() const
{
  return responses;
}

void TrackerFeatureSet::clearResponses()
{
  responses.clear();
}

} /* namespace cv */
