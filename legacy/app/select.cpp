#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
//#include 
#include <iostream>
#include <string>
//#include <qt5/QtWidgets/QApplication>
#include <QApplication>

//
#include <thread>

//#include "code/texture.hpp"
#include "PointList.h"
#include "Grayscale.hpp"
#include "Show.hpp"

//------------------------------------------------------------------------------
std::vector<cv::Vec3b> samples; 

void run ();
void CallBack(int event, int x, int y, int flags, void* userdata);
cv::Ptr<cv::Mat> img_;
cv::Ptr<cv::Mat> read_xml (cv::Ptr<cv::Mat> file);

cv::Vec3b calc_average(cv::Ptr<cv::Mat> file_);

cv::Ptr<cv::Mat> class_l2_norm(cv::Ptr<cv::Mat> img_, cv::Vec3b average_pixel);

//------------------------------------------------------------------------------

//ew cv::Mat(cv::imread("../../img/original/deb8_800x600.jpg"));
//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  
    QApplication app (argc, argv);
    
    //PointList list;
    //list.show();
    
    //PointList* ptr_list = &list;
    
    //std::thread run_opencv(run);
    
    //run_opencv.join();
    
    //cv::CommandLineParser parser(argc, argv,"{input||}");
    //std::string filename = parser.get<std::string>("input");
    
    //std::string filename;
    //if (filename.empty())
    //{
    //    std::cout << "no path" << std::endl;
    //    exit(EXIT_FAILURE);      
    //}
    
    //std::cout << "path: " << filename << std::endl;
    
    samples = std::vector<cv::Vec3b>();
    img_ = new cv::Mat(cv::imread("/home/marcelo/Dropbox/PPGCC/2016-02/disciplinas/visao/img/bd/12003.jpg"));
            //filename.c_str()));
    
    cv::namedWindow("image", CV_WINDOW_KEEPRATIO);
    
    cv::setMouseCallback("image", CallBack, NULL);
    cv::imshow("image", *img_);
    
    cv::waitKey(0);
    
    cv::Ptr<cv::Mat> mat_samples (new cv::Mat(samples)); 
   
   std::cout << "size: " << std::to_string(samples.size()) << std::endl;
   std::cout << "\n Mat Samples: \n" << *mat_samples <<std::endl;
   
   cv::FileStorage fs("/home/marcelo/Dropbox/PPGCC/2016-02/disciplinas/visao/img/bd/test.xml", cv::FileStorage::WRITE);

    //cout << "writing images\n";
    //fs << "samples" << "[";

    //fs << "image1.jpg" << "myfi.png" << "../data/baboon.jpg";
    //cout << "image1.jpg" << " myfi.png" << " ../data/baboon.jpg" << endl;

    //fs << "]";

    //cout << "writing mats\n";
    //Mat R =Mat_<double>::eye(3, 3),T = Mat_<double>::zeros(3, 1);
    //cout << "R = " << R << "\n";
   // cout << "T = " << T << "\n";
    fs << "samples" << *mat_samples;
    fs.release();
    
    //return app.exec();
    cv::Ptr<cv::Mat> image_read;
    image_read = read_xml(image_read);
    
    calc_average(image_read);
    
    std::cout << "out function " << *image_read;
    
    return 0;
}

/**
 * Events <mouse move> + <hold alt key> in orignal image window!
 * in this function x is columns and y is lines (in cv::Mat)!
 */

//------------------------------------------------------------------------------

void CallBack(int event, int x, int y, int flags, void* userdata)
{
    
    int row = y;
    int col = x;
    
    //*((int*)&userdata);
    
    std::cout << "callback" << std::endl;
    cv::Vec3b* pixel_;
    
    if ( event == cv::EVENT_MOUSEMOVE && flags == cv::EVENT_FLAG_CTRLKEY)
    {
        pixel_ = img_->ptr<cv::Vec3b>(row);
        //BGR
        
        //cv::Vec3b temp = {pixel_[col][0], pixel_[col][1], pixel_[col][2]};
        
        if ( ! (pixel_[col][0] == 0 && pixel_[col][1] == 0 && pixel_[col][2] == 0) )
            samples.push_back(pixel_[col]);
        
        
        //samples.push_back(temp);
        
        int thickness = -1;
        int lineType = 8;

        cv::circle( *img_, cv::Point(x,y), 3.0, cv::Scalar( 0, 0, 0 ), thickness, lineType );
        cv::imshow("image", *img_);
    }
        
    
    /*
    if  ( event == cv::EVENT_LBUTTONDOWN )
    {
          std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
     else if  ( event == cv::EVENT_RBUTTONDOWN )
    {
          std::cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
     else if  ( event == cv::EVENT_MBUTTONDOWN )
    {
          std::cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
     else if ( event == cv::EVENT_MOUSEMOVE )
    {
          std::cout << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;

    }
    */
    //delete[] pixel_;
}

void run ()
{
    cv::Ptr<cv::Mat> img_ = new cv::Mat(cv::imread("../../img/original/deb8_800x600.jpg"));
    
    cv::namedWindow("image", CV_WINDOW_KEEPRATIO);
    
    
    cv::imshow("image", *img_);
}

cv::Ptr<cv::Mat> read_xml (cv::Ptr<cv::Mat> file)
{
   cv::FileStorage fs_read ("/home/marcelo/Dropbox/PPGCC/2016-02/disciplinas/visao/img/bd/test.xml", cv::FileStorage::READ); 
   cv::Mat img;
   
   if (fs_read.isOpened()) 
   {
       fs_read["samples"] >> img;
      // std::cout << "Image read:\n" << img;
       
       //cv::Ptr<cv::Mat> mat_samples (new cv::Mat(samples)); 
   
       //file.operator = (cv::Ptr<cv::Mat> (new cv::Mat(img)));
       // = cv::Ptr<cv::Mat> (new cv::Mat(img));
       std::cout << "Image read:\n" << img; 
   }
   else 
   {
       std::cout << "not open" << std::endl;
   }
   
   return cv::Ptr<cv::Mat> (new cv::Mat(img));
   
   std::cout << "read_xml - end" << std::endl;   
}

cv::Vec3b calc_average(cv::Ptr<cv::Mat> file_)
{
    float b = 0;
    float g = 0;
    float r = 0;
    
    
    //CV_Assert(file_->data);
    
    int cout = 0;
    
    for (auto row = 0 ; row < file_->rows ; row++ )
    {
        cv::Vec3b* pixel = file_->ptr<cv::Vec3b>(row);
                
        
        for (auto cols = 0 ; cols < file_->cols ; cols++)
        {
            
            cout++;
            std::cout << "interarion: " << cout << std::endl;
            
            b += pixel[cols][0];
            g += pixel[cols][1];
            r += pixel[cols][2];            
        }
    }
    
    b = b / (file_->rows * file_->cols);
    g = g / (file_->rows * file_->cols);
    r = r / (file_->rows * file_->cols);
    
    
    std::cout << "b: " << std::to_string(b) << std::endl;
    std::cout << "g: " << std::to_string(g) << std::endl;
    std::cout << "r: " << std::to_string(r) << std::endl;
    
    std::cout << "b: " << std::to_string((uchar)b) << std::endl;
    std::cout << "g: " << std::to_string((uchar)g) << std::endl;
    std::cout << "r: " << std::to_string((uchar)r) << std::endl;
    
    
    cv::Vec3b average;
    
    average[0] = (uchar)b;
    average[1] = (uchar)g;
    average[2] = (uchar)r;
    
    return average;
    
    
}

cv::Ptr<cv::Mat> class_l2_norm(cv::Ptr<cv::Mat> img_, cv::Vec3b average_pixel)
{
    
    for(auto row = 0 ; row < img_->rows ; row++)
    {
        cv::Vec3b* pixel = img_->ptr<cv::Vec3b>(row);
                
        for (auto col = 0 ; col < img_->cols ; col++)            
        {
            pixel[col][0];
            pixel[col][1];
            pixel[col][2];
            
        }
    }
    
}

