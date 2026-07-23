#include <highgui.h>
#include <iostream>
//#include <qt5/QtWidgets/QApplication>
//#include <QApplication>

//#include "code/texture.hpp"
//#include "PointList.h"

#include "Histogram.hpp"
#include "Vision.hpp"

int main(int argc, char **argv)
{
    
    Histogram hist_;
    
    hist_.load("/home/marcelo/Dropbox/PPGCC/2016-02/disciplinas/visao/img/bd/12003.jpg");
    hist_.run();
  
    
    
    
}

/*if (argc < 2)
  //{
    //std::cerr << "test IMAGE-IN IMAGE-OUT" << std::endl;
    //return 1;
  //}
  //cv::imwrite(argv[2], ex::colorize(cv::imread(argv[1])));
    
  cv::Mat* original = new cv::Mat (cv::imread("../../img/14565_A_8_2-05X05.jpg"));   
  cv::Mat* original_gray;
    
  texture filter (original);  
  filter.texture_out();
  original_gray = new cv::Mat (filter.get_lbp());  
  
  
  cv::namedWindow("original", CV_WINDOW_KEEPRATIO);
  cv::imshow("original", *original);
  
  cv::namedWindow("lbp", CV_WINDOW_KEEPRATIO);
  cv::imshow("lbp", *original_gray);
  
  cv::waitKey(0);
  
  std::cout << "Done" << std::endl;
   */

//return 0;
  
 //}


/*
 * QApplication app (argc, argv);
    
    PointList list;
    list.show();
    
    return app.exec();
 */