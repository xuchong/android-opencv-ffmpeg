/*
* Copyright (c) BUAA
* All rights reserved.
*
* 文件名称：kalman_filters.HPP
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
#ifndef __OPENCV_TRACKING_KALMAN_HPP_
#define __OPENCV_TRACKING_KALMAN_HPP_

#include "opencv2/core.hpp"
#include <limits>

namespace cv
{
namespace tracking
{

/** @brief The interface for Unscented Kalman filter and Augmented Unscented Kalman filter.
*/
class  UnscentedKalmanFilter//CV_EXPORTS
{
public:

    virtual ~UnscentedKalmanFilter(){}

    /** The function performs prediction step of the algorithm
    * @param control - the current control vector,
    * @return the predicted estimate of the state.
    */
    virtual Mat predict( const Mat& control = Mat() ) = 0;

    /** The function performs correction step of the algorithm
    * @param measurement - the current measurement vector,
    * @return the corrected estimate of the state.
    */
    virtual Mat correct( const Mat& measurement ) = 0;

    /**
    * @return the process noise cross-covariance matrix.
    */
    virtual Mat getProcessNoiseCov() const = 0;

    /**
    * @return the measurement noise cross-covariance matrix.
    */
    virtual Mat getMeasurementNoiseCov() const = 0;

    /**
    * @return the error cross-covariance matrix.
    */
    virtual Mat getErrorCov() const = 0;

    /**
    * @return the current estimate of the state.
    */
    virtual Mat getState() const = 0;
};

/** @brief Model of dynamical system for Unscented Kalman filter.
* The interface for dynamical system model. It contains functions for computing the next state and the measurement.
* It must be inherited for using UKF.
*/
class  UkfSystemModel//CV_EXPORTS
{
public:

    virtual ~UkfSystemModel(){}

    /** The function for computing the next state from the previous state
    * @param x_k - previous state vector,
    * @param u_k - control vector,
    * @param v_k - noise vector,
    * @param x_kplus1 - next state vector.
    */
    virtual void stateConversionFunction( const Mat& x_k, const Mat& u_k, const Mat& v_k, Mat& x_kplus1 ) = 0;
    /** The function for computing the measurement from the state
    * @param x_k - state vector,
    * @param n_k - noise vector,
    * @param z_k - measurement vector.
    */
    virtual void measurementFunction( const Mat& x_k, const Mat& n_k, Mat& z_k ) = 0;
};


/** @brief Unscented Kalman filter parameters.
* The class for initialization parameters of Unscented Kalman filter
*/
class UnscentedKalmanFilterParams//CV_EXPORTS 
{
public:

     int DP;                                     //!< Dimensionality of the state vector.
     int MP;                                     //!< Dimensionality of the measurement vector.
     int CP;                                     //!< Dimensionality of the control vector.
     int dataType;                               //!< Type of elements of vectors and matrices, default is CV_64F.

     Mat stateInit;                              //!< Initial state, DP x 1, default is zero.
     Mat errorCovInit;                           //!< State estimate cross-covariance matrix, DP x DP, default is identity.

     Mat processNoiseCov;                        //!< Process noise cross-covariance matrix, DP x DP.
     Mat measurementNoiseCov;                    //!< Measurement noise cross-covariance matrix, MP x MP.

     // Parameters of algorithm
     double alpha;                               //!< Default is 1e-3.
     double k;                                   //!< Default is 0.
     double beta;                                //!< Default is 2.0.

     //Dynamical system model
     Ptr<UkfSystemModel> model;                  //!< Object of the class containing functions for computing the next state and the measurement.

    /** The constructors.
    */
    UnscentedKalmanFilterParams(){}

    /**
    * @param dp - dimensionality of the state vector,
    * @param mp - dimensionality of the measurement vector,
    * @param cp - dimensionality of the control vector,
    * @param processNoiseCovDiag - value of elements on main diagonal process noise cross-covariance matrix,
    * @param measurementNoiseCovDiag - value of elements on main diagonal measurement noise cross-covariance matrix,
    * @param dynamicalSystem - ptr to object of the class containing functions for computing the next state and the measurement,
    * @param type - type of the created matrices that should be CV_32F or CV_64F.
    */
    UnscentedKalmanFilterParams( int dp, int mp, int cp, double processNoiseCovDiag, double measurementNoiseCovDiag,
                                Ptr<UkfSystemModel> dynamicalSystem, int type = CV_64F );

    /** The function for initialization of Unscented Kalman filter
    * @param dp - dimensionality of the state vector,
    * @param mp - dimensionality of the measurement vector,
    * @param cp - dimensionality of the control vector,
    * @param processNoiseCovDiag - value of elements on main diagonal process noise cross-covariance matrix,
    * @param measurementNoiseCovDiag - value of elements on main diagonal measurement noise cross-covariance matrix,
    * @param dynamicalSystem - ptr to object of the class containing functions for computing the next state and the measurement,
    * @param type - type of the created matrices that should be CV_32F or CV_64F.
    */
    void init( int dp, int mp, int cp, double processNoiseCovDiag, double measurementNoiseCovDiag,
                                Ptr<UkfSystemModel> dynamicalSystem, int type = CV_64F );
};

/** @brief Augmented Unscented Kalman filter parameters.
* The class for initialization parameters of Augmented Unscented Kalman filter
*/
class AugmentedUnscentedKalmanFilterParams: public UnscentedKalmanFilterParams// CV_EXPORTS
{
public:

    AugmentedUnscentedKalmanFilterParams(){}

    /**
    * @param dp - dimensionality of the state vector,
    * @param mp - dimensionality of the measurement vector,
    * @param cp - dimensionality of the control vector,
    * @param processNoiseCovDiag - value of elements on main diagonal process noise cross-covariance matrix,
    * @param measurementNoiseCovDiag - value of elements on main diagonal measurement noise cross-covariance matrix,
    * @param dynamicalSystem - ptr to object of the class containing functions for computing the next state and the measurement,
    * @param type - type of the created matrices that should be CV_32F or CV_64F.
    */
    AugmentedUnscentedKalmanFilterParams( int dp, int mp, int cp, double processNoiseCovDiag, double measurementNoiseCovDiag,
                                Ptr<UkfSystemModel> dynamicalSystem, int type = CV_64F );

    /** The function for initialization of Augmented Unscented Kalman filter
    * @param dp - dimensionality of the state vector,
    * @param mp - dimensionality of the measurement vector,
    * @param cp - dimensionality of the control vector,
    * @param processNoiseCovDiag - value of elements on main diagonal process noise cross-covariance matrix,
    * @param measurementNoiseCovDiag - value of elements on main diagonal measurement noise cross-covariance matrix,
    * @param dynamicalSystem - object of the class containing functions for computing the next state and the measurement,
    * @param type - type of the created matrices that should be CV_32F or CV_64F.
    */
    void init( int dp, int mp, int cp, double processNoiseCovDiag, double measurementNoiseCovDiag,
                                Ptr<UkfSystemModel> dynamicalSystem, int type = CV_64F );
};

/** @brief Unscented Kalman Filter factory method

* The class implements an Unscented Kalman filter <https://en.wikipedia.org/wiki/Kalman_filter#Unscented_Kalman_filter>.
* @param params - an object of the UnscentedKalmanFilterParams class containing UKF parameters.
* @return pointer to the object of the UnscentedKalmanFilterImpl class implementing UnscentedKalmanFilter.
*/
 Ptr<UnscentedKalmanFilter> createUnscentedKalmanFilter( const UnscentedKalmanFilterParams &params );//CV_EXPORTS
/** @brief Augmented Unscented Kalman Filter factory method

* The class implements an Augmented Unscented Kalman filter http://becs.aalto.fi/en/research/bayes/ekfukf/documentation.pdf, page 31-33.
* AUKF is more accurate than UKF but its computational complexity is larger.
* @param params - an object of the AugmentedUnscentedKalmanFilterParams class containing AUKF parameters.
* @return pointer to the object of the AugmentedUnscentedKalmanFilterImpl class implementing UnscentedKalmanFilter.
*/
 Ptr<UnscentedKalmanFilter> createAugmentedUnscentedKalmanFilter( const AugmentedUnscentedKalmanFilterParams &params );//CV_EXPORTS

} // tracking
} // cv

#endif
