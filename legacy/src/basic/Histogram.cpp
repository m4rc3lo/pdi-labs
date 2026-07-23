/* 
 * File:   Histogram.cpp
 * Author: marcelo
 * 
 * Created on 29 de Agosto de 2016, 12:59
 */

//#include <opencv2/core/cvstd.hpp>
#include <opencv2/opencv.hpp>

#include "Histogram.hpp"
#include "Vision.hpp"

Histogram::Histogram() {}

Histogram::Histogram(const Histogram& orig) {}

Histogram::~Histogram() {}


/**Implements run() in Vision.hpp*/

cv::Ptr<cv::Mat> Histogram::run()
{
    
    
    this->canvas__ = std::vector<cv::Ptr<cv::Mat> > ();
    this->channels = std::vector<cv::Ptr<cv::Mat> > ();
    
    cv::Ptr<cv::Mat> image = this->source_01;
            //cv::Ptr<cv::Mat> ( new cv::Mat(this->source_01->rows, this->source_01->cols, this->source_01->type()) );
    
    cv::namedWindow("entrada", cv::WINDOW_KEEPRATIO);
    cv::imshow("entrada", *image);
    int bins = 256;
    int channels = image->channels();
    
    std::vector<cv::Ptr<cv::Mat> > split_histo(channels);
    std::vector<cv::Ptr<cv::Mat> > canvas_hist(channels);
    
    int h_max[3] = {0, 0, 0};
    
    
    //----------------------------------------------------------------------------------
    // init vectors's
    
    for (int c = 0 ; c < split_histo.size() ; c++) 
    {
        split_histo[c] = cv::Ptr<cv::Mat>( new cv::Mat(1, bins, CV_32SC1));
    }
    //----------------------------------------------------------------------------------
    
      //----------------------------------------------------------------------------------
    // calcula histogramas para cada canal
    
    for (int altura = 0 ; altura < image->rows ; altura++) 
    {
        for (int largura = 0 ; largura < image->cols ; largura ++ ) 
        {
            for (int nc = 0 ; nc < channels ; nc++) 
            {
                
                uchar val = channels == 1 ? 
                    image->at<uchar>(altura, largura) : 
                    image->at<cv::Vec3b>(altura, largura)[nc];
                
                split_histo[nc]->at<int>(val) += 1; 
                
            }
        }
    }
    //----------------------------------------------------------------------------------
    
    //----------------------------------------------------------------------------------
    //calcula o valor de pico de cada canal
    
    for (int nc = 0 ; nc < channels ; nc++) 
    {
        for (int b = 0 ; b < bins-1 ; b++ )
        {
            h_max[nc] = split_histo[nc]->at<int>(b) > h_max[nc] ? split_histo[nc]->at<int>(b) : h_max[nc];
                    
        }
    }
    
    const char* wname[3] = {"blue", "green", "red"};
    cv::Scalar colors[3] = {cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255)};
    
    for (int nc = 0 ; nc < channels ; nc++) 
    {
        canvas_hist[nc] = cv::Ptr<cv::Mat> ( new cv::Mat (cv::Mat::ones(125, bins, CV_8UC3)) );
        
        for (int j = 0, linhas = canvas_hist[nc]->rows ; j < bins-1 ; j++) 
        {
            cv::line
            (
            
              *canvas_hist[nc],
              cv::Point ( j, linhas ), 
              cv::Point ( j, linhas - (split_histo[nc]->at<int>(j) * linhas/h_max[nc]) ),
              channels == 1 ? cv::Scalar(200, 200, 200) : colors[nc],
              1, 8, 0
            
             );
        }
        
        cv::namedWindow(channels == 1 ? "value" : wname[nc], CV_WINDOW_KEEPRATIO);
        
        cv::imshow(channels == 1 ? "value" : wname[nc], *canvas_hist[nc]);
    }
    
    cv::waitKey(0);
}

