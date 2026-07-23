//------------------------------------------------------------------------------
/**
** Dornbusch - 13/08/0216
* Interface for basic algorithms
*/
//------------------------------------------------------------------------------

/**A guard to prevent the inclusion of a header file multiple times*/
#ifndef VISION_HPP
#define  VISION_HPP

//------------------------------------------------------------------------------
#include <iostream> // for derived classes
#include <opencv2/core.hpp> /** OpenCV core functonalitis*/
//------------------------------------------------------------------------------

/**
 *  This is a super class with two paths and two images
 *  The sub class must be implement the virtual fuction run()
 */  
class Vision
{
  public:

    /**
     * Overloaded
     * For one or two images
     */
    void load(cv::String path); 
    void load(cv::String path1, cv::String path2); //***/
    
    /**
    *  Ge a smart pointe for this images  
    */
    cv::Ptr<cv::Mat> get_original_01(){ return this->source_01; }     
    cv::Ptr<cv::Mat> get_original_02(){ return this->source_02; }
    
    /**
    * Must be implemented ...
    */
    virtual cv::Ptr<cv::Mat> run()=0;

  protected:

      /**
       * Instances variables
       * Two strings
       * Two pointers for images
       */
    cv::String path_01; 
    cv::String path_02; 
    cv::Ptr<cv::Mat> source_01;
    cv::Ptr<cv::Mat> source_02;
};
#endif