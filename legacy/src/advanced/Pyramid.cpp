

//#include <>
#include "Pyramid.hpp"
#include <cstdlib>
#include <iostream>


//2.x.x -> 3.x
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

Pyramid::Pyramid()
{
  //
}

Pyramid::Pyramid (std::string path)
{
  this->image = cv::imread(path.c_str());
  /// Test image - Make sure it s divisible by 2^{n}
  if (!image.data)
  {
    std::cerr << "/* failed to open image */" << std::endl;
    exit(EXIT_FAILURE);
  }

}
Pyramid::~Pyramid()
{
  //
}

void Pyramid::start()
{

  cv::Mat dst;
  cv::Mat tmp;

  tmp = this->image;
  dst = tmp;

  cv::namedWindow( "Pyramid", CV_WINDOW_AUTOSIZE );
  cv::imshow( "Pyramid", dst );


  while( true )
  {
    int c;
    c = cv::waitKey(10);

    if( (char)c == 27 )
    { break; }
    if( (char)c == 'u' )
    {
      cv::pyrUp( tmp, dst, cv::Size( tmp.cols*2, tmp.rows*2 ) );
      std::cout << "** Zoom In: Image x 2" << std::endl;
    }
    else if( (char)c == 'd' )
    {
      cv::pyrDown( tmp, dst, cv::Size( tmp.cols/2, tmp.rows/2 ) );
      std::cout << "** Zoom Out: Image / 2" << std::endl;
    }

    cv::imshow( "Pyramid", dst );
    tmp = dst;
  }


}

void message()
{
  /// General instructions
  std::cout << "Zoom In-Out demo" << std::endl;
  std::cout << "* [u] -> Zoom in" << std::endl;
  std::cout << "------------------" << std::endl;
  std::cout << "* [d] -> Zoom out" << std::endl;
  std::cout << "* [ESC] -> Close program" << std::endl;
}
