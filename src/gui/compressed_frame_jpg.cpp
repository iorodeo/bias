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
    const bool CompressedFrame_jpg::DEFAULT_MJPG_FLAG = false;

    // Public methods
    // -------------------------------------------------------------------------------------------------------

    CompressedFrame_jpg::CompressedFrame_jpg()
    {
        quality_ = DEFAULT_QUALITY;
        fileName_ = DEFAULT_FILENAME;
        mjpgFlag_ = DEFAULT_MJPG_FLAG;
        haveFileName_= false;
        haveStampedImg_ = false;
        haveEncoding_ = false;
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
        setQuality(quality);
    }

    CompressedFrame_jpg::CompressedFrame_jpg(QString fileName, StampedImage stampedImg, unsigned int quality, bool mjpgFlag)
        : CompressedFrame_jpg(fileName,stampedImg,quality)
    {
        setMjpgFlag(mjpgFlag);
    }


    bool CompressedFrame_jpg::haveFileName() const
    {
        return haveFileName_;
    }


    bool CompressedFrame_jpg::haveStampedImage() const
    {
        return haveStampedImg_;
    }


    void CompressedFrame_jpg::setStampedImage(StampedImage stampedImg)
    {
        stampedImg_= stampedImg;
        haveStampedImg_ = true;
        haveEncoding_ = false;
        encodedJpgBuffer_.clear();
    }

    StampedImage CompressedFrame_jpg::getStampedImage() const
    {
        return stampedImg_;
    }

    unsigned long CompressedFrame_jpg::getFrameCount() const
    {
        if (haveStampedImg_)
        {
            stampedImg_.frameCount;
        }
        else
        {
            return 0;
        }
    } 


    void CompressedFrame_jpg::setFileName(QString fileName)
    {
        fileName_ = fileName;
        haveFileName_ = true;
    }

    QString CompressedFrame_jpg::getFileName() const
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

    unsigned int CompressedFrame_jpg::getQuality() const
    {
        return quality_;
    }


    void CompressedFrame_jpg::setMjpgFlag(bool value)
    {
        mjpgFlag_ = value;
    }


    bool CompressedFrame_jpg::getMjpgFlag() const
    {
        return mjpgFlag_;
    }


    bool CompressedFrame_jpg::haveEncoding() const
    {
        return haveEncoding_;
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

    void CompressedFrame_jpg::encode()
    {
        std::vector<int> compressionParams;
        compressionParams.push_back(CV_IMWRITE_JPEG_QUALITY);
        compressionParams.push_back(int(quality_));
        cv::imencode(".jpg", stampedImg_.image, encodedJpgBuffer_, compressionParams);
    }

    // Compressed frame comparison operator
    // ----------------------------------------------------------------------------------------
    bool CompressedFrameCmp_jpg::operator() (
            const CompressedFrame_jpg &cmpFrame0,
            const CompressedFrame_jpg &cmpFrame1
            )
    {
        bool haveImage0 = cmpFrame0.haveStampedImage();
        bool haveImage1 = cmpFrame1.haveStampedImage();

        if ((haveImage0 == false) && (haveImage1 == false))
        {
            return false;
        }
        else if ((haveImage0 == false) && (haveImage1 == true))
        {
            return false;
        }
        else if ((haveImage0 == true) && (haveImage1 == false))
        {
            return true;
        }
        else
        {
            unsigned long frameCount0 = cmpFrame0.getFrameCount();
            unsigned long frameCount1 = cmpFrame1.getFrameCount();
            return (frameCount0 < frameCount1);
        }
    }

} // namespace bias
