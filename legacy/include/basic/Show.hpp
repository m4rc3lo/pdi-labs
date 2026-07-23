/* 
 * File:   Show.hpp
 * Author: marcelo
 *
 * Created on 19 de Agosto de 2016, 15:18
 */

#ifndef SHOW_HPP
#define	SHOW_HPP

#include <opencv2/highgui.hpp>

namespace dl 
{
    class Show
    {
        public:
       
            static void show (cv::Ptr<cv::Mat> image, cv::String name_);
            
            static void show 
                            ( cv::Ptr<cv::Mat> image_1, 
                              cv::Ptr<cv::Mat> image_2,
                              cv::String name_1,
                              cv::String name_2 
                            );
            
            static void show 
                            ( cv::Ptr<cv::Mat> image_1, 
                              cv::Ptr<cv::Mat> image_2,
                              cv::Ptr<cv::Mat> image_3,
                              cv::String name_1,
                              cv::String name_2, 
                              cv::String name_3
                            );            
        private:
            
            Show();
    };
}

#endif	/* SHOW_HPP */

