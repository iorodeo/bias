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

    // Set video mode
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

    // Set trigger 
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

    // Set frame rate
    PropertyInfo frameRateInfo;
    Property frameRateProp;
    
    try
    {
        frameRateInfo = cameraPtr_ -> getPropertyInfo(PROPERTY_TYPE_FRAME_RATE);
        frameRateProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_FRAME_RATE);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to get framerate property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    if (param_.frameRate < frameRateInfo.minAbsoluteValue)
    {
        QString errorMsg = QString("framerate less than minimum allowed %1").arg(frameRateInfo.minAbsoluteValue);
        emit cameraSetupError(errorMsg);
        return false;

    }
    if (param_.frameRate > frameRateInfo.maxAbsoluteValue)
    {
        QString errorMsg = QString("framerate greater than maximum allowed %1").arg(frameRateInfo.maxAbsoluteValue);
        emit cameraSetupError(errorMsg);
        return false;
    }

    frameRateProp.absoluteControl = true;
    frameRateProp.absoluteValue = param_.frameRate;
    frameRateProp.autoActive = false;

    try
    {
        cameraPtr_ -> setProperty(frameRateProp);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to set framerate property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }



    // Set gain
    PropertyInfo gainInfo;
    Property gainProp;
    try
    {
        gainInfo = cameraPtr_ -> getPropertyInfo(PROPERTY_TYPE_GAIN);
        gainProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_GAIN);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to get gain property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    if (param_.gain < gainInfo.minAbsoluteValue)
    {
        QString errorMsg = QString("gain less than minimum allowed %1").arg(gainInfo.minAbsoluteValue);
        emit cameraSetupError(errorMsg);
        return false;
    }
    if (param_.gain > gainInfo.maxAbsoluteValue)
    {
        QString errorMsg = QString("gain greater than maximum allowed %1").arg(gainInfo.minAbsoluteValue);
        emit cameraSetupError(errorMsg);
        return false;
    }

    gainProp.absoluteControl = true;
    gainProp.absoluteValue = param_.gain;
    gainProp.autoActive = false;

    try
    {
        cameraPtr_ -> setProperty(gainProp);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to set gain property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }
    

    // Set shutter
    PropertyInfo shutterInfo;
    Property shutterProp;
    try
    {
        shutterInfo = cameraPtr_ -> getPropertyInfo(PROPERTY_TYPE_SHUTTER);
        shutterProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_SHUTTER);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to get shutter property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    if (param_.shutter < shutterInfo.minAbsoluteValue)
    {
        QString errorMsg = QString("shutter less than minimum allowed %1").arg(shutterInfo.minAbsoluteValue);
        emit cameraSetupError(errorMsg);
        return false;
    }
    if (param_.shutter > shutterInfo.maxAbsoluteValue)
    {
        QString errorMsg = QString("shutter greater than maximum allowed %1").arg(shutterInfo.minAbsoluteValue);
        emit cameraSetupError(errorMsg);
        return false;
    }

    shutterProp.absoluteControl = true;
    shutterProp.absoluteValue = param_.shutter;
    shutterProp.autoActive = false;

    try
    {
        cameraPtr_ -> setProperty(shutterProp);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to set shutter property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }
   


    // Set brightness
    PropertyInfo brightnessInfo;
    Property brightnessProp;
    try
    {
        brightnessInfo = cameraPtr_ -> getPropertyInfo(PROPERTY_TYPE_BRIGHTNESS);
        brightnessProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_BRIGHTNESS);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to get brightness property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    if (param_.brightness < brightnessInfo.minValue)
    {
        QString errorMsg = QString("brightness less than minimum allowed %1").arg(brightnessInfo.minValue);
        emit cameraSetupError(errorMsg);
        return false;
    }
    if (param_.brightness > brightnessInfo.maxValue)
    {
        QString errorMsg = QString("brightness greater than maximum allowed %1").arg(brightnessInfo.minValue);
        emit cameraSetupError(errorMsg);
        return false;
    }

    brightnessProp.absoluteControl = false;
    brightnessProp.absoluteValue = param_.brightness;
    brightnessProp.autoActive = false;

    try
    {
        cameraPtr_ -> setProperty(brightnessProp);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to set brightness property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    return true;
}

