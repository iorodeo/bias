#include "compressed_frame_jpg.hpp"
#include "basic_types.hpp"
#include "exception.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

namespace bias
{
    // Constants
    // -------------------------------------------------------------------------------------------------------
    const QString CompressedFrame_jpg::DEFAULT_FILENAME = "default_filename.jpg";
    const unsigned int CompressedFrame_jpg::DEFAULT_QUALITY = 90;


    // Public methods
    // -------------------------------------------------------------------------------------------------------

    CompressedFrame_jpg::CompressedFrame_jpg()
    {
        quality_ = DEFAULT_QUALITY;
        fileName_ = DEFAULT_FILENAME;
        haveFileName_= false;
        haveStampedImg_ = false;
    }


    CompressedFrame_jpg::CompressedFrame_jpg(QString fileName)
        : CompressedFrame_jpg()
    {
        setFileName(fileName);
    }


    CompressedFrame_jpg::CompressedFrame_jpg(QString fileName, StampedImage stampedImg)
        : CompressedFrame_jpg(fileName)
    {
        setStampedImage(stampedImg);
    }


    CompressedFrame_jpg::CompressedFrame_jpg(QString fileName, StampedImage stampedImg, unsigned int quality)
        : CompressedFrame_jpg(fileName,stampedImg)
    {
        setFileName(fileName);
        setStampedImage(stampedImg);
        setQuality(quality);
    }


    bool CompressedFrame_jpg::haveFileName()
    {
        return haveFileName_;
    }


    bool CompressedFrame_jpg::haveStampedImage()
    {
        return haveStampedImg_;
    }


    void CompressedFrame_jpg::setStampedImage(StampedImage stampedImg)
    {
        stampedImg_= stampedImg;
        haveStampedImg_ = true;
    }

    StampedImage CompressedFrame_jpg::getStampedImage()
    {
        return stampedImg_;
    }


    void CompressedFrame_jpg::setFileName(QString fileName)
    {
        fileName_ = fileName;
        haveFileName_ = true;
    }

    QString CompressedFrame_jpg::getFileName()
    {
        return fileName_;
    }


    void CompressedFrame_jpg::setQuality(unsigned int quality)
    {
        if (quality <= 100)
        {
            quality_= quality;
        }
        else
        {
            quality_ = 100;
        }
    }

    unsigned int CompressedFrame_jpg::getQuality()
    {
        return quality_;
    }

    void CompressedFrame_jpg::write()
    {
        std::vector<int> compressionParams;
        compressionParams.push_back(CV_IMWRITE_JPEG_QUALITY);
        compressionParams.push_back(int(quality_));

        try
        {
            cv::imwrite(fileName_.toStdString(), stampedImg_.image, compressionParams);
        }
        catch (cv::Exception &exc)
        {
            unsigned int errorId = ERROR_VIDEO_WRITER_ADD_FRAME;
            std::string errorMsg("writing jpg frame failed - "); 
            errorMsg += exc.what();
            throw RuntimeError(errorId, errorMsg);
        }
    }

} // namespace bias
