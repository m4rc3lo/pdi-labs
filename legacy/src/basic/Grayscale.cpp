#include "Grayscale.hpp"

Grayscale::Grayscale(){}

/**
 * @see Mat OpenCV Documentation http://goo.gl/0LuNaz* 
 * @return cv::Mat gray
 */

cv::Ptr<cv::Mat> Grayscale::run()
{
    /**
     * New image rows x cols single channel 8bits
     */
    cv::Ptr<cv::Mat> gray ( new cv::Mat(this->source_01->rows, this->source_01->cols, CV_8UC1) );
    
    float pixel_temp = 0.f;
    
    /**row by row*/
    for (int height = 0 ; height < this->source_01->rows ; height++) 
    {   
        /**
         * A poiter for cv::Vec3b type (array of 3 usigned char)
         * ptr function returm a pointer for row matrix 
         */
        cv::Vec3b* src_ptr = this->source_01->ptr<cv::Vec3b>(height);
        
        /**
         * A poiter for unsigned char type 
         * ptr function returm a pointer for row matrix 
         */
        uchar* dst_ptr = gray->ptr<uchar>(height);
        
        /**column by column*/
        for (int width = 0 ; width < this->source_01->cols ; width++ ) 
        {   
            /**Access pixel B,G,R values in [width] column*/
            pixel_temp =  src_ptr[width][0] * 0.114f;
            pixel_temp += src_ptr[width][1] * 0.587f;
            pixel_temp += src_ptr[width][2] * 0.299f;
            
            /**Access single channel pixel*/
            dst_ptr[width] = (uchar)pixel_temp;
        }
    }
    
    /**A smart pointer with result*/
    return gray;
}