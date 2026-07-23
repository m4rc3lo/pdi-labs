
#ifndef GRAYSCALE_HPP
#define GRAYSCALE_HPP

#include "Vision.hpp"
/**
 * @inheritance Vision.hpp
 * 
 * Class for weighted grayscale conversoin
 * 0.299 * R
 * 0.587 * G
 * 0.114 * B
 */
class Grayscale : public Vision
{
    public:
        
        Grayscale(); /**Empty consctructor*/
        
        /**
         * Implement Vision run()
         * Retrun a smart pointer for a new image in grayscale.
         * A CV_8UC1 single channel image (8bits)
         */
        cv::Ptr<cv::Mat> run();
};
#endif
