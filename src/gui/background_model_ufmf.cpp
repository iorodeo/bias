#include "background_model_ufmf.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <cmath>
#include <iostream>

namespace bias
{

    const unsigned int BackgroundModel_ufmf::DEFAULT_BIN_SIZE = 1;

    BackgroundModel_ufmf::BackgroundModel_ufmf()
    {
        isFirst_ = true;
        binSize_ = DEFAULT_BIN_SIZE;
    }

    void BackgroundModel_ufmf::addFrame(StampedImage stampedImg)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        if (isFirst_ == true)
        {
            initialize(stampedImg);
            isFirst_ = false;
        }
    }

    void BackgroundModel_ufmf::initialize(StampedImage stampedImg)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        imageSize_ = stampedImg.image.size();
        imageDepth_ = stampedImg.image.depth();

        switch (imageDepth_)
        {
            case CV_8U:
                numberOfBins_ = int(ceil(256.0/float(binSize_)));
                break;

            case CV_16U:
                numberOfBins_ = int(ceil(65536.0/float(binSize_)));
                break;

            default:

                unsigned int errorId = ERROR_VIDEO_WRITER_INITIALIZE;
                std::string errorMsg("ufmf background model setup failed:\n\n"); 
                errorMsg += "image depth must be CV_8U or CV_16U";
                throw RuntimeError(errorId,errorMsg);

                break;

        }
         
        

    }

} // namespace bias
