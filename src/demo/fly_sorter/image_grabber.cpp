#include "image_grabber.hpp"
#include "camera_facade.hpp"
#include "exception.hpp"
#include <QPointer>
#include <iostream>

// CameraInfo
// ----------------------------------------------------------------------------

CameraInfo::CameraInfo()
{
    vendor = QString("");
    model = QString("");
    guid = QString("");
}

// ImageData
// ----------------------------------------------------------------------------
ImageData::ImageData()
{
    frameCount = 0;
}

// ImageGrabber
// ----------------------------------------------------------------------------

ImageGrabber::ImageGrabber(ImageGrabberParam param, QObject *parent) 
: QObject(parent) 
{ 
    param_ = param;
    stopped_ = false;
}


void ImageGrabber::stopCapture()
{
    stopped_ = true;
}


void ImageGrabber::run()
{ 

    // Setup camera and start capture
    if (!setupCamera())
    {
        return;
    }

    try
    {
        cameraPtr_ -> startCapture();
    }
    catch (RuntimeError &runtimeError)
    {
        unsigned int errorId = runtimeError.id();
        QString errorMsg = QString("Unable to start capture: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return;
    }


    // Grab images
    ImageData imageData;

    while (!stopped_)
    {
        try
        {
            imageData.mat = cameraPtr_ -> grabImage();
        }
        catch (RuntimeError &runtimeError)
        {
            continue;
        }
        imageData.frameCount++; 
        emit newImage(imageData);
    } 

    // Clean up
    try
    {
        cameraPtr_ -> stopCapture();
    }
    catch (RuntimeError &runtimeError)
    {
        unsigned int errorId = runtimeError.id();
        QString errorMsg = QString("Unable to start capture: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
    }

    try
    {
        cameraPtr_ -> disconnect();
    }
    catch (RuntimeError &runtimeError)
    {
        unsigned int errorId = runtimeError.id();
        QString errorMsg = QString("Unable to disconnect from camera: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
    }

    CameraInfo emptyInfo;
    emit newCameraInfo(emptyInfo);
    emit stopped();
}


bool ImageGrabber::setupCamera()
{
    CameraFinder cameraFinder;
    CameraPtrList cameraPtrList;  

    // Get list guids for all cameras found
    try
    { 
        cameraPtrList = cameraFinder.createCameraPtrList();
    }
    catch (bias::RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to enumerate cameras: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    // If no cameras found - error
    if (cameraPtrList.empty()) 
    {
        QString errorMsg = QString("No cameras found");
        emit cameraSetupError(errorMsg);
        return false;
    }

    // Connect to first camera
    cameraPtr_ = cameraPtrList.front();
    if (cameraPtr_ -> isConnected()) 
    {
        QString errorMsg = QString("Camera is already connected");
        emit cameraSetupError(errorMsg);
        return false;
    }
    try
    {
        cameraPtr_ -> connect();
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to connect to camera: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    CameraInfo cameraInfo;
    cameraInfo.vendor = QString::fromStdString(cameraPtr_ -> getVendorName());
    cameraInfo.model = QString::fromStdString(cameraPtr_ -> getModelName());
    cameraInfo.guid = QString::fromStdString((cameraPtr_ -> getGuid()).toString());
    emit newCameraInfo(cameraInfo);

    // Set video mode and frame rate
    try
    {
        cameraPtr_ -> setVideoMode(VIDEOMODE_FORMAT7);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to set Video Mode: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    try
    {
        cameraPtr_ -> setTriggerInternal();
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to set trigger: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    try
    {
        PropertyInfo  frameRateInfo = cameraPtr_ -> getPropertyInfo(PROPERTY_TYPE_FRAME_RATE);
        Property frameRateProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_FRAME_RATE);
        frameRateProp.absoluteControl = true;
        frameRateProp.absoluteValue = param_.frameRate;
        frameRateProp.autoActive = false;
        cameraPtr_ -> setProperty(frameRateProp);
        frameRateProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_FRAME_RATE);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to framerate: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    return true;
}

