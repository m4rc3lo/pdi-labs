
#include "Vision.hpp"

//#include <cstdlib>
#include <iostream>
#include <opencv2/highgui.hpp>  


/**
 * @Overrid load
 * @param path
 * 
 * if cannot load, exit the program
 */
void Vision::load(cv::String path) 
{
    /**Take the path for a image file*/
    this->path_01 = std::string (path);
    
    /*
     * @see OpenCV Documentation http://goo.gl/mkK6H6 (cv::imread)
     * 
     * cv::imread load a image by a file, if cannot return a cv::Mat::data==NULL
     * 
     * "The function determines the type of an image by the content, not by the file extension."
     * "In the case of color images, the decoded images will have the channels stored in B G R order." 
     */
    this->source_01 = cv::Ptr<cv::Mat> (new cv::Mat(cv::imread(this->path_01.c_str())));
    
    /**check for good load*/
    if( !this->source_01->data )    
    { 
        std::cerr << "No data! -- Exiting the program" << std::endl;
        exit (EXIT_FAILURE);
    }
}

/**
 * @Overrid load
 * @param path1
 * @param path2
 * 
 * if cannot load, exit the program
 */
void Vision::load(cv::String path_01, cv::String path_02) 
{
    this->path_01 = cv::String (path_01);
    this->path_02 = cv::String (path_02);
    
    this->source_01 = cv::Ptr<cv::Mat> (new cv::Mat(cv::imread(this->path_01.c_str())));
    this->source_02 = cv::Ptr<cv::Mat> (new cv::Mat(cv::imread(this->path_02.c_str())));
    //cv::imread(this->path_02.c_str());
    
    if( !this->source_01->data || !this->source_02->data)
    
    { 
        std::cerr << "No data! -- Exiting the program" << std::endl;
        exit (EXIT_FAILURE);
    }
}