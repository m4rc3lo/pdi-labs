/* 
 * File:   Subtraction.hpp
 * Author: marcelo
 *
 * Created on 14 de Agosto de 2016, 21:38
 */

#ifndef SUBTRACTION_HPP
#define	SUBTRACTION_HPP

#include "Vision.hpp"

/**
 * @inheritance Vision.hpp
 * 
 * Class for subtract two images
 * In color mode
 */
class Subtraction : public Vision
{
    
    public:
        
        Subtraction ();
        
        /**
         * Implement Vision run()
         * Retrun a smart pointer for a new image.
         * BGR image, 3 channel (8bits)
         */
        cv::Ptr<cv::Mat> run();       
        
};

#endif	/* SUBTRACTION_HPP */

