/*
* Dornbusch - 12/08/2016
* opencv 3.0 testing ... pyramid
*/

#ifndef PYRAMID_HPP
#define PYRAMID_HPP

#include <string>
#include <opencv2/core.hpp>

class Pyramid
{

  public:

    Pyramid ();
    Pyramid (std::string path);

    void start ();

    virtual ~Pyramid();

  private:

    void message();
    cv::Mat image;

};

#endif
