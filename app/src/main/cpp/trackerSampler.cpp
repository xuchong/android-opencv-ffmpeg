/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：trackerSampler.CPP
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
 *  TrackerSampler
 */

/*
 * Constructor
 */
TrackerSampler::TrackerSampler()
{
  blockAddTrackerSampler = false;
}

/*
 * Destructor
 */
TrackerSampler::~TrackerSampler()
{

}

void TrackerSampler::sampling( const Mat& image, Rect boundingBox )
{

  clearSamples();

  for ( size_t i = 0; i < samplers.size(); i++ )
  {
    std::vector<Mat> current_samples;
    samplers[i].second->sampling( image, boundingBox, current_samples );

    //push in samples all current_samples
    for ( size_t j = 0; j < current_samples.size(); j++ )
    {
      std::vector<Mat>::iterator it = samples.end();
      samples.insert( it, current_samples.at( j ) );
    }
  }

  if( !blockAddTrackerSampler )
  {
    blockAddTrackerSampler = true;
  }
}

bool TrackerSampler::addTrackerSamplerAlgorithm( String trackerSamplerAlgorithmType )
{
  if( blockAddTrackerSampler )
  {
    return false;
  }
  Ptr<TrackerSamplerAlgorithm> sampler = TrackerSamplerAlgorithm::create( trackerSamplerAlgorithmType );

  if( sampler == 0 )
  {
    return false;
  }

  samplers.push_back( std::make_pair( trackerSamplerAlgorithmType, sampler ) );

  return true;
}

bool TrackerSampler::addTrackerSamplerAlgorithm( Ptr<TrackerSamplerAlgorithm>& sampler )
{
  if( blockAddTrackerSampler )
  {
    return false;
  }

  if( sampler == 0 )
  {
    return false;
  }

  String trackerSamplerAlgorithmType = sampler->getClassName();
  samplers.push_back( std::make_pair( trackerSamplerAlgorithmType, sampler ) );

  return true;
}

const std::vector<std::pair<String, Ptr<TrackerSamplerAlgorithm> > >& TrackerSampler::getSamplers() const
{
  return samplers;
}

const std::vector<Mat>& TrackerSampler::getSamples() const
{
  return samples;
}

void TrackerSampler::clearSamples()
{
  samples.clear();
}

} /* namespace cv */
