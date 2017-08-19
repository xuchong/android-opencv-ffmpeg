/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：trackBoosting.CPP
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
#include "trackerBoostingModel.hpp"

namespace cv
{

class TrackerBoostingImpl : public TrackerBoosting
{
 public:
  TrackerBoostingImpl( const TrackerBoosting::Params &parameters = TrackerBoosting::Params() );
  void read( const FileNode& fn );
  void write( FileStorage& fs ) const;

 protected:

  bool initImpl( const Mat& image, const Rect2d& boundingBox );
  bool updateImpl( const Mat& image, Rect2d& boundingBox );

  TrackerBoosting::Params params;
};

/*
 *  TrackerBoosting
 */

/*
 * Parameters
 */
TrackerBoosting::Params::Params()
{
  numClassifiers = 100;
  samplerOverlap = 0.99f;
  samplerSearchFactor = 1.8f;
  iterationInit = 50;
  featureSetNumFeatures = ( numClassifiers * 10 ) + iterationInit;
}

void TrackerBoosting::Params::read( const cv::FileNode& fn )
{
  numClassifiers = fn["numClassifiers"];
  samplerOverlap = fn["overlap"];
  samplerSearchFactor = fn["samplerSearchFactor"];
  iterationInit = fn["iterationInit"];
  samplerSearchFactor = fn["searchFactor"];
}

void TrackerBoosting::Params::write( cv::FileStorage& fs ) const
{
  fs << "numClassifiers" << numClassifiers;
  fs << "overlap" << samplerOverlap;
  fs << "searchFactor" << samplerSearchFactor;
  fs << "iterationInit" << iterationInit;
  fs << "samplerSearchFactor" << samplerSearchFactor;
}

/*
 * Constructor
 */
Ptr<TrackerBoosting> TrackerBoosting::createTracker(const TrackerBoosting::Params &parameters){
    return Ptr<TrackerBoostingImpl>(new TrackerBoostingImpl(parameters));
}
TrackerBoostingImpl::TrackerBoostingImpl( const TrackerBoostingImpl::Params &parameters ) :
    params( parameters )
{
  isInit = false;
}

void TrackerBoostingImpl::read( const cv::FileNode& fn )
{
  params.read( fn );
}

void TrackerBoostingImpl::write( cv::FileStorage& fs ) const
{
  params.write( fs );
}

bool TrackerBoostingImpl::initImpl( const Mat& image, const Rect2d& boundingBox )
{
  srand (1);
  //sampling
  Mat_<int> intImage;
  Mat_<double> intSqImage;
  Mat image_;
  cvtColor( image, image_, CV_RGB2GRAY );
  integral( image_, intImage, intSqImage, CV_32S );
  TrackerSamplerCS::Params CSparameters;
  CSparameters.overlap = params.samplerOverlap;
  CSparameters.searchFactor = params.samplerSearchFactor;

  Ptr<TrackerSamplerAlgorithm> CSSampler = Ptr<TrackerSamplerCS>( new TrackerSamplerCS( CSparameters ) );

  if( !sampler->addTrackerSamplerAlgorithm( CSSampler ) )
    return false;

  CSSampler.staticCast<TrackerSamplerCS>()->setMode( TrackerSamplerCS::MODE_POSITIVE );
  sampler->sampling( intImage, boundingBox );
  const std::vector<Mat> posSamples = sampler->getSamples();

  CSSampler.staticCast<TrackerSamplerCS>()->setMode( TrackerSamplerCS::MODE_NEGATIVE );
  sampler->sampling( intImage, boundingBox );
  const std::vector<Mat> negSamples = sampler->getSamples();

  if( posSamples.empty() || negSamples.empty() )
    return false;

  Rect ROI = CSSampler.staticCast<TrackerSamplerCS>()->getROI();

  //compute HAAR features
  TrackerFeatureHAAR::Params HAARparameters;
  HAARparameters.numFeatures = params.featureSetNumFeatures;
  HAARparameters.isIntegral = true;
  HAARparameters.rectSize = Size( static_cast<int>(boundingBox.width), static_cast<int>(boundingBox.height) );
  Ptr<TrackerFeature> trackerFeature = Ptr<TrackerFeatureHAAR>( new TrackerFeatureHAAR( HAARparameters ) );
  if( !featureSet->addTrackerFeature( trackerFeature ) )
    return false;

  featureSet->extraction( posSamples );
  const std::vector<Mat> posResponse = featureSet->getResponses();
  featureSet->extraction( negSamples );
  const std::vector<Mat> negResponse = featureSet->getResponses();

  //Model
  model = Ptr<TrackerBoostingModel>( new TrackerBoostingModel( boundingBox ) );
  Ptr<TrackerStateEstimatorAdaBoosting> stateEstimator = Ptr<TrackerStateEstimatorAdaBoosting>(
      new TrackerStateEstimatorAdaBoosting( params.numClassifiers, params.iterationInit, params.featureSetNumFeatures,
                                            Size( static_cast<int>(boundingBox.width), static_cast<int>(boundingBox.height) ), ROI ) );
  model->setTrackerStateEstimator( stateEstimator );

  //Run model estimation and update for iterationInit iterations
  for ( int i = 0; i < params.iterationInit; i++ )
  {
    //compute temp features
    TrackerFeatureHAAR::Params HAARparameters2;
    HAARparameters2.numFeatures = static_cast<int>( posSamples.size() + negSamples.size() );
    HAARparameters2.isIntegral = true;
    HAARparameters2.rectSize = Size( static_cast<int>(boundingBox.width), static_cast<int>(boundingBox.height) );
    Ptr<TrackerFeatureHAAR> trackerFeature2 = Ptr<TrackerFeatureHAAR>( new TrackerFeatureHAAR( HAARparameters2 ) );

    model.staticCast<TrackerBoostingModel>()->setMode( TrackerBoostingModel::MODE_NEGATIVE, negSamples );
    model->modelEstimation( negResponse );
    model.staticCast<TrackerBoostingModel>()->setMode( TrackerBoostingModel::MODE_POSITIVE, posSamples );
    model->modelEstimation( posResponse );
    model->modelUpdate();

    //get replaced classifier and change the features
    std::vector<int> replacedClassifier = stateEstimator->computeReplacedClassifier();
    std::vector<int> swappedClassified = stateEstimator->computeSwappedClassifier();
    for ( size_t j = 0; j < replacedClassifier.size(); j++ )
    {
      if( replacedClassifier[j] != -1 && swappedClassified[j] != -1 )
      {
        trackerFeature.staticCast<TrackerFeatureHAAR>()->swapFeature( replacedClassifier[j], swappedClassified[j] );
        trackerFeature.staticCast<TrackerFeatureHAAR>()->swapFeature( swappedClassified[j], trackerFeature2->getFeatureAt( (int)j ) );
      }
    }
  }

  return true;
}

bool TrackerBoostingImpl::updateImpl( const Mat& image, Rect2d& boundingBox )
{
  Mat_<int> intImage;
  Mat_<double> intSqImage;
  Mat image_;
  cvtColor( image, image_, CV_RGB2GRAY );
  integral( image_, intImage, intSqImage, CV_32S );
  //get the last location [AAM] X(k-1)
  Ptr<TrackerTargetState> lastLocation = model->getLastTargetState();
  Rect lastBoundingBox( (int)lastLocation->getTargetPosition().x, (int)lastLocation->getTargetPosition().y, lastLocation->getTargetWidth(),
                        lastLocation->getTargetHeight() );

  //sampling new frame based on last location
  ( sampler->getSamplers().at( 0 ).second ).staticCast<TrackerSamplerCS>()->setMode( TrackerSamplerCS::MODE_CLASSIFY );
  sampler->sampling( intImage, lastBoundingBox );
  const std::vector<Mat> detectSamples = sampler->getSamples();
  Rect ROI = ( sampler->getSamplers().at( 0 ).second ).staticCast<TrackerSamplerCS>()->getROI();

  if( detectSamples.empty() )
    return false;

  /*//TODO debug samples
   Mat f;
   image.copyTo( f );

   for ( size_t i = 0; i < detectSamples.size(); i = i + 10 )
   {
   Size sz;
   Point off;
   detectSamples.at( i ).locateROI( sz, off );
   rectangle( f, Rect( off.x, off.y, detectSamples.at( i ).cols, detectSamples.at( i ).rows ), Scalar( 255, 0, 0 ), 1 );
   }*/

  std::vector<Mat> responses;
  Mat response;

  std::vector<int> classifiers = model->getTrackerStateEstimator().staticCast<TrackerStateEstimatorAdaBoosting>()->computeSelectedWeakClassifier();
  Ptr<TrackerFeatureHAAR> extractor = featureSet->getTrackerFeature()[0].second.staticCast<TrackerFeatureHAAR>();
  extractor->extractSelected( classifiers, detectSamples, response );
  responses.push_back( response );

  //predict new location
  ConfidenceMap cmap;
  model.staticCast<TrackerBoostingModel>()->setMode( TrackerBoostingModel::MODE_CLASSIFY, detectSamples );
  model.staticCast<TrackerBoostingModel>()->responseToConfidenceMap( responses, cmap );
  model->getTrackerStateEstimator().staticCast<TrackerStateEstimatorAdaBoosting>()->setCurrentConfidenceMap( cmap );
  model->getTrackerStateEstimator().staticCast<TrackerStateEstimatorAdaBoosting>()->setSampleROI( ROI );

  if( !model->runStateEstimator() )
  {
    return false;
  }

  Ptr<TrackerTargetState> currentState = model->getLastTargetState();
  boundingBox = Rect( (int)currentState->getTargetPosition().x, (int)currentState->getTargetPosition().y, currentState->getTargetWidth(),
                      currentState->getTargetHeight() );

  /*//TODO debug
   rectangle( f, lastBoundingBox, Scalar( 0, 255, 0 ), 1 );
   rectangle( f, boundingBox, Scalar( 0, 0, 255 ), 1 );
   imshow( "f", f );
   //waitKey( 0 );*/

  //sampling new frame based on new location
  //Positive sampling
  ( sampler->getSamplers().at( 0 ).second ).staticCast<TrackerSamplerCS>()->setMode( TrackerSamplerCS::MODE_POSITIVE );
  sampler->sampling( intImage, boundingBox );
  const std::vector<Mat> posSamples = sampler->getSamples();

  //Negative sampling
  ( sampler->getSamplers().at( 0 ).second ).staticCast<TrackerSamplerCS>()->setMode( TrackerSamplerCS::MODE_NEGATIVE );
  sampler->sampling( intImage, boundingBox );
  const std::vector<Mat> negSamples = sampler->getSamples();

  if( posSamples.empty() || negSamples.empty() )
    return false;

  //extract features
  featureSet->extraction( posSamples );
  const std::vector<Mat> posResponse = featureSet->getResponses();

  featureSet->extraction( negSamples );
  const std::vector<Mat> negResponse = featureSet->getResponses();

  //compute temp features
  TrackerFeatureHAAR::Params HAARparameters2;
  HAARparameters2.numFeatures = static_cast<int>( posSamples.size() + negSamples.size() );
  HAARparameters2.isIntegral = true;
  HAARparameters2.rectSize = Size( static_cast<int>(boundingBox.width), static_cast<int>(boundingBox.height) );
  Ptr<TrackerFeatureHAAR> trackerFeature2 = Ptr<TrackerFeatureHAAR>( new TrackerFeatureHAAR( HAARparameters2 ) );

  //model estimate
  model.staticCast<TrackerBoostingModel>()->setMode( TrackerBoostingModel::MODE_NEGATIVE, negSamples );
  model->modelEstimation( negResponse );
  model.staticCast<TrackerBoostingModel>()->setMode( TrackerBoostingModel::MODE_POSITIVE, posSamples );
  model->modelEstimation( posResponse );

  //model update
  model->modelUpdate();

  //get replaced classifier and change the features
  std::vector<int> replacedClassifier = model->getTrackerStateEstimator().staticCast<TrackerStateEstimatorAdaBoosting>()->computeReplacedClassifier();
  std::vector<int> swappedClassified = model->getTrackerStateEstimator().staticCast<TrackerStateEstimatorAdaBoosting>()->computeSwappedClassifier();
  for ( size_t j = 0; j < replacedClassifier.size(); j++ )
  {
    if( replacedClassifier[j] != -1 && swappedClassified[j] != -1 )
    {
      featureSet->getTrackerFeature().at( 0 ).second.staticCast<TrackerFeatureHAAR>()->swapFeature( replacedClassifier[j], swappedClassified[j] );
      featureSet->getTrackerFeature().at( 0 ).second.staticCast<TrackerFeatureHAAR>()->swapFeature( swappedClassified[j],
                                                                                                    trackerFeature2->getFeatureAt( (int)j ) );
    }
  }

  return true;
}

} /* namespace cv */
