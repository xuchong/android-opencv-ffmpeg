#include "precomp.hpp"

#undef BOILERPLATE_CODE
#define BOILERPLATE_CODE(name,classname)\
  if(trackerType==name){\
      return classname::createTracker();\
  }

namespace cv
{

/*
 *  Tracker
 */

Tracker::~Tracker()
{
}

bool Tracker::init( const Mat& image, const Rect2d& boundingBox )
{

  if( isInit )
  {
    return false;
  }

  if( image.empty() )
    return false;

  sampler = Ptr<TrackerSampler>( new TrackerSampler() );
  featureSet = Ptr<TrackerFeatureSet>( new TrackerFeatureSet() );
  model = Ptr<TrackerModel>();

  bool initTracker = initImpl( image, boundingBox );

  //check if the model component is initialized
  if( model == 0 )
  {
    CV_Error( -1, "The model is not initialized" );
    return false;
  }

  if( initTracker )
  {
    isInit = true;
  }

  return initTracker;
}

bool Tracker::update( const Mat& image, Rect2d& boundingBox )
{

  if( !isInit )
  {
    return false;
  }

  if( image.empty() )
    return false;

  return updateImpl( image, boundingBox );
}

Ptr<Tracker> Tracker::create( const String& trackerType )
{
  BOILERPLATE_CODE("MIL",TrackerMIL);
  BOILERPLATE_CODE("BOOSTING",TrackerBoosting);
  BOILERPLATE_CODE("MEDIANFLOW",TrackerMedianFlow);
  BOILERPLATE_CODE("KCF",TrackerKCF);
  return Ptr<Tracker>();
}

} /* namespace cv */
