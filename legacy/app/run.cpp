//------------------------------------------------------------------------------
/**!
* Dornbusch - 12/08/2016
* opencv 3.x testing ...
*/
//------------------------------------------------------------------------------

//#include <memory> /** for std::stshare_ptr */
#include <cstdint>  // for int8_t a signed char (8bits)
#include <cstdlib>  // general purpose fucntion
#include <iostream> // input/output stream objects
//#include <utility>
//------------------------------------------------------------------------------
//#include <opencv2/cvstd.hpp>
#include <opencv2/core.hpp> // core functionalities
//#include <opencv2/highgui.hpp>
//------------------------------------------------------------------------------
#include "Show.hpp"
#include "Pyramid.hpp"
#include "Grayscale.hpp"
#include "Subtraction.hpp"
//------------------------------------------------------------------------------

/**
 * Global varial, strinf type in opencv.
 * A help message cecessary for cv::CommandLineParser.
 */
const cv::String keys =
        "{help h usage ? |      | print this message   }"
        "{img1           |      | original image1      }"
        "{img2           |      | original image2      }"
        "{op operation   |      | operation            }"
        "{value          | 1.0  | number               }"
        ;

/**
 * Enum class for operations
 */
enum class Operation : uint8_t {Grayscale = 1, Subtraction = 2, Undefined = 255};

//static void help (); /**Show the help message*/

/**This fucntion convert a string in Operation (enum class)
 *    @param cv::String
 *    @return Operation
 * 
 * The inline fuctions are replaced by your code in compiling time.
 */
inline Operation hashit (cv::String const& operation_);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**Main fuction */
int main (int argc, char** argv)
{
    /**Parser for command line*/
    cv::CommandLineParser parser (argc, argv, keys);
    parser.about("Vision Basic v0.0.2");
    
    /**Test for a valid command line*/
    if ( parser.has("help") || argc < 2)
    {
        std::cout << "\n\n";
        parser.printMessage();
        exit(EXIT_FAILURE);
    }
    
    system("clear"); // clear terminal
    
    /**
     * Choose opertation.
     * "operation" is a fild dfined in <keys>
     */
    switch (hashit(parser.get<cv::String>("op"))) 
    {
        case Operation::Grayscale: 
        {
            /** smart pointer for Grayscale class*/
            cv::Ptr<Grayscale> gray (new Grayscale());
            
            gray->load(parser.get<cv::String>("img1")); /**Load image*/
            
            /**Do grayscale conversion
            * image_dst is a new one
            */
            cv::Ptr<cv::Mat> image_dst = gray->run();
            
            /**Show  original and result*/
            dl::Show::show(gray->get_original_01(), image_dst, "Original", "Grayscale");
            break;
        }
        
        case Operation::Subtraction:
        {
            /**smart pointer for Subtraction class
             * load two images
             */ 
            cv::Ptr<Subtraction> sub (new Subtraction());
            sub->load(parser.get<cv::String>("img1"), parser.get<cv::String>("img2"));
            
            /*Do subtraction images
             * image_dst is a new one
             */
            cv::Ptr<cv::Mat> image_dst = sub->run();

            /**Show  originals and result*/
            dl::Show::show(sub->get_original_01(),
                           sub->get_original_02(),
                           image_dst,
                           "Image1",
                           "Image2",
                           "Result"
                           );
            break;
        }
        /** Do more cases ...*/
        default:
        {   
            /** cheks */
            std::cout << "something wrong!" << std::endl;
            if (hashit(parser.get<cv::String>("op")) == Operation::Undefined)
            {
                std::cout << "chek --operation" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
                
    }
    /**close the program*/
    exit(EXIT_SUCCESS) ;
    
    }  

/**
 * This fuction checks dor a know operation.
 * Return Operation::Undefined otherwise. 
 * @param cv::String operation_
 * @return Operation (enum class)
 */
inline Operation hashit (cv::String const& operation_)
{
    if (operation_.compare("grayscale") == 0)
    {
        std::cout << "grayscale" << std::endl;
        return Operation::Grayscale;
    }    
    else if (operation_.compare("subtraction") == 0)
    {
         std::cout << "subtraction" << std::endl;
        return Operation::Subtraction;
    }
    else
        return Operation::Undefined;
}
