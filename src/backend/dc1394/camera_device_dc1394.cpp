#ifdef WITH_DC1394
#include "camera_device_dc1394.hpp"
#include "utils_dc1394.hpp"
#include "exception.hpp"
#include <sstream>
#include <algorithm>
#ifdef WIN32
#include<Windows.h>
#endif

namespace bias {

    CameraDevice_dc1394::CameraDevice_dc1394() : CameraDevice()
    {
        context_dc1394_ = NULL;
        camera_dc1394_ = NULL;
        numDMABuffer_ = DEFAULT_NUM_DMA_BUFFER; 

        timeStamp_ = {0,0};
        startTime_ = 0;
        timerFreq_ = 1;
        isFirst_ = true;
    }

    CameraDevice_dc1394::CameraDevice_dc1394(Guid guid) : CameraDevice(guid)
    {
        context_dc1394_ = NULL;
        camera_dc1394_ = NULL;
        numDMABuffer_ = DEFAULT_NUM_DMA_BUFFER; 

        context_dc1394_ = dc1394_new();
        if (!context_dc1394_) 
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": error creating dc1394_t" << std::endl;
            throw RuntimeError(ERROR_DC1394_CREATE_CONTEXT, ssError.str());
        }
    }

    CameraDevice_dc1394::~CameraDevice_dc1394()
    {
        if (capturing_) { stopCapture(); } 
        if (connected_) { disconnect(); }
        dc1394_free(context_dc1394_);
    }

    CameraLib CameraDevice_dc1394::getCameraLib()
    {
        return guid_.getCameraLib();
    }

    void CameraDevice_dc1394::connect() 
    {
        if (!connected_) 
        {
            // Create new dc1394 camera object
            camera_dc1394_ = dc1394_camera_new(context_dc1394_, guid_.getValue_dc1394());

            if ( ! camera_dc1394_ ) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to create dc1394 camera";
                throw RuntimeError(ERROR_DC1394_NEW_CAMERA, ssError.str());
            }
            connected_ = true;


            // DEVEL 
            // -------------------------------------------------------------------
            // Print supported video modes
            dc1394video_modes_t supportedModes;
            dc1394_video_get_supported_modes(camera_dc1394_, &supportedModes);
            std::cout << std::endl << "supported video modes" << std::endl;
            printVideoModes_dc1394(supportedModes);
            std::cout << std::endl;
            // --------------------------------------------------------------------


        }
    }

    void CameraDevice_dc1394::disconnect()
    {
        if (capturing_) { stopCapture(); }
        if (connected_) 
        {
            dc1394_camera_free(camera_dc1394_);
            connected_ = false;
        }
    }

    void CameraDevice_dc1394::startCapture()
    {
        dc1394error_t error;
        if (!connected_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to start dc1394 capture - not connected";
            throw RuntimeError(ERROR_DC1394_START_CAPTURE, ssError.str());
        }

        if (!capturing_) {

            // ------------------------------------------------------------------
            // WBD DEVEL
            //
            // Need to modify so that we pick a resonable initial video mode
            // ------------------------------------------------------------------

            // Temporary - just pick a video mode which works.
            error = dc1394_video_set_mode(
                    camera_dc1394_, 
                    DC1394_VIDEO_MODE_FORMAT7_0
                    );

            if (error != DC1394_SUCCESS) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to set dc1394 video mode, error code ";
                ssError << error  << std::endl;
                throw RuntimeError(ERROR_DC1394_SET_VIDEO_MODE, ssError.str());
            }
        
            // Temporary - set color coding to mono8
            error = dc1394_format7_set_color_coding(
                    camera_dc1394_,
                    DC1394_VIDEO_MODE_FORMAT7_0,
                    DC1394_COLOR_CODING_MONO8
                    );
            if (error != DC1394_SUCCESS)
            { 
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to set dc1394 color_coding, error code ";
                ssError << error  << std::endl;
                throw RuntimeError(ERROR_DC1394_SET_VIDEO_MODE, ssError.str());
            }

            // Set number of DMA buffers and capture flags
            error = dc1394_capture_setup(
                    camera_dc1394_,
                    numDMABuffer_, 
                    DC1394_CAPTURE_FLAGS_DEFAULT
                    );
            if (error != DC1394_SUCCESS) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to setup dc1394 capture, error code ";
                ssError << error << std::endl;
                throw RuntimeError(ERROR_DC1394_CAPTURE_SETUP, ssError.str());
            }

            // Start video transmission
            error = dc1394_video_set_transmission(camera_dc1394_, DC1394_ON);
            if (error != DC1394_SUCCESS) 
            {
                std::stringstream ssError;
                ssError << __PRETTY_FUNCTION__;
                ssError << ": unable to turn on dc1394 video transmission, error code ";
                ssError << error << std::endl;
                throw RuntimeError(ERROR_DC1394_SET_VIDEO_TRANSMISSION, ssError.str());
            }
            isFirst_ = true;
            capturing_ = true;
        }
    }

    void CameraDevice_dc1394::stopCapture()
    {
        if ( capturing_ ) {
            dc1394_video_set_transmission(camera_dc1394_, DC1394_OFF);
            dc1394_capture_stop(camera_dc1394_);
            capturing_ = false;
        }
    }

    void CameraDevice_dc1394::grabImage(cv::Mat &image)
    {
        if (!capturing_)
        {
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to grab dc1394 image - not capturing";
            throw RuntimeError(ERROR_DC1394_GRAB_IMAGE, ssError.str());
        }

        dc1394error_t error;
        error = dc1394_capture_dequeue(
                camera_dc1394_, 
                DC1394_CAPTURE_POLICY_WAIT, 
                &frame_dc1394_
                );

        // Get frame  
        if (error != DC1394_SUCCESS) 
        { 
            std::stringstream ssError;
            ssError << __PRETTY_FUNCTION__;
            ssError << ": unable to dequeue dc1394 frame, error code ";
            ssError << error << std::endl;
            throw RuntimeError(ERROR_DC1394_CAPTURE_DEQUEUE, ssError.str());
        }

        // update time stamp
        updateTimeStamp();
        isFirst_ = false;

        // Copy to cv image  (Temporary) - assume mono8 format
        // Need to modify to handle color images.
        // --------------------------------------------------------------------------
        if ((image.rows != frame_dc1394_ -> size[1]) || (image.cols != frame_dc1394_ -> size[0]))
        {
            image = cv::Mat(frame_dc1394_-> size[1], frame_dc1394_-> size[0], CV_8UC1); 
        }

        unsigned int frameSize = (frame_dc1394_ -> size[0])*(frame_dc1394_ -> size[1]);
        unsigned char *pData0 = frame_dc1394_ -> image;
        unsigned char *pData1 = pData0 +  frameSize - 1;
        std::copy(pData0, pData1, image.data);


        // Put frame back 
        error = dc1394_capture_enqueue(camera_dc1394_, frame_dc1394_);

        //std::cout << "color coding: " << getColorCodingString_dc1394(frame_dc1394_ -> color_coding) << std::endl;
        //std::cout << "size:         " << frame_dc1394_ -> size[0] << ", " << frame_dc1394_ -> size[1] << std::endl;
        //std::cout << "total bytes:  " << frame_dc1394_ -> total_bytes << std::endl;
        //std::cout << "frameSize:    " << frameSize << std::endl;
    }

    cv::Mat CameraDevice_dc1394::grabImage()
    {
        cv::Mat image;
        grabImage(image);
        return image;
    }

    VideoModeList CameraDevice_dc1394::getAllowedVideoModes()
    {
        return VideoModeList();
    }

    FrameRateList CameraDevice_dc1394::getAllowedFrameRates(VideoMode vidMode) 
    { 
        return FrameRateList(); 
    } 

    ImageModeList CameraDevice_dc1394::getAllowedImageModes()
    {
        return ImageModeList();
    }

    TimeStamp CameraDevice_dc1394::getImageTimeStamp()
    {
        return timeStamp_;
    }

    Format7Settings CameraDevice_dc1394::getFormat7Settings()
    {
        return Format7Settings();
    }


    Format7Info CameraDevice_dc1394::getFormat7Info(ImageMode imgMode)
    {
        return Format7Info(imgMode);
    }


    bool CameraDevice_dc1394::validateFormat7Settings(Format7Settings settings)
    {
        return false;
    }

   
    void CameraDevice_dc1394::setFormat7Configuration(Format7Settings settings, float percentSpeed)
    {
    }


    PixelFormatList CameraDevice_dc1394::getListOfSupportedPixelFormats(ImageMode imgMode)
    {
        return PixelFormatList();
    }


    std::string CameraDevice_dc1394::toString()
    {
        std::stringstream ss;
        ss << std::endl;
        ss << " ------------------ " << std::endl;
        ss << " Camera Information " << std::endl;
        ss << " ------------------ " << std::endl;
        ss << std::endl;
        if (connected_)
        {

            ss << " guid:        " << guid_ << std::endl;
            ss << " vendor Name: " << camera_dc1394_ -> vendor << std::endl;
            ss << " vendor Id:   " << std::hex << camera_dc1394_ -> vendor_id  << std::dec << std::endl;
            ss << " model Name:  " << std::hex << camera_dc1394_ -> model << std::dec << std::endl;
            ss << " model Id:    " << camera_dc1394_ -> model_id << std::endl;
        }
        else 
        {
            ss << " camera not connected" << std::endl;
        }
        ss << std::endl;
        return ss.str();
    }

    void CameraDevice_dc1394::printGuid()
    {
        guid_.printValue();
    }

    void CameraDevice_dc1394::printInfo()
    {
        std::cout << toString();
    }

    // Private methods
    // ------------------------------------------------------------------------
    

    void CameraDevice_dc1394::updateTimeStamp()
    {
        double elapsedTime = 0.0;
#ifdef WIN32
        if (isFirst_) 
        {
            // Get Start time and timer frequency
            LARGE_INTEGER startTimeLI;
            QueryPerformanceCounter(&startTimeLI);
            startTime_ = uint64_t(startTimeLI.QuadPart);

            LARGE_INTEGER timerFreqLI;
            QueryPerformanceFrequency(&timerFreqLI);
            timerFreq_ = uint64_t(timerFreqLI.QuadPart);
        }

        LARGE_INTEGER currentTimeLI;
        QueryPerformanceCounter(&currentTimeLI);
        uint64_t currentTime = uint64_t(currentTimeLI.QuadPart);
        elapsedTime = (currentTime - startTime_)/double(timerFreq_);

#else
        if (isFirst_)
        {
            startTime_ = frame_dc1394_  -> timestamp; 
            timerFreq_ = 1;  // Not used
        }
        elapsedTime = double(frame_dc1394_ -> timestamp - startTime_);
#endif
        timeStamp_.seconds = (long long)(elapsedTime);
        timeStamp_.microSeconds = (unsigned int)(1.0e6*(elapsedTime - double(timeStamp_.seconds)));
    }

} // namespace bias

#endif  // #ifdef WITH_DC1394
