/* 
 * File:   Histogram.hpp
 * Author: marcelo
 *
 * Created on 29 de Agosto de 2016, 12:58
 */

#ifndef HISTOGRAM_HPP
#define	HISTOGRAM_HPP

#include "Vision.hpp"

class Histogram : public Vision
{
    public:
        
        Histogram();
        Histogram(const Histogram& orig);
        virtual ~Histogram();
        
        cv::Ptr<cv::Mat> run ();
        
    private:
        
        std::vector<cv::Ptr<cv::Mat> > channels;
        std::vector<cv::Ptr<cv::Mat> > canvas__;

};

#endif	/* HISTOGRAM_HPP */

