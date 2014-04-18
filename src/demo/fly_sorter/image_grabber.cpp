#include "image_grabber.hpp"
#include "camera_facade.hpp"
#include "exception.hpp"
#include <QPointer>
#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <iostream>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Constants
// ----------------------------------------------------------------------------
const QString DEBUG_DUMP_CAMERA_PROPS_FILE_NAME("fly_sorter_camera_props_dump.txt");
const int CAPTURE_FROM_DIR_FRAME_NUMBER_MUL = 1000;


// Function prototypes
// ----------------------------------------------------------------------------
bool getFrameNumberToFileInfoMap(
        QFileInfoList imageFileInfoList, 
        QMap<int,QFileInfo> &frameNumberToFileInfoMap,
        QString errorMsg
        );

bool getFrameNumberFilterMap(
        QDir captureInputDir,
        QMap<int,bool> &frameNumberFilterMap,
        QString errorMsg
        );

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
    dateTime = 0.0;
}


void ImageData::copy(ImageData imageData)
{
    frameCount = imageData.frameCount;
    dateTime = imageData.dateTime;
    imageData.mat.copyTo(mat);
}

// ImageGrabber
// ----------------------------------------------------------------------------

ImageGrabber::ImageGrabber(ImageGrabberParam param, QObject *parent) 
: QObject(parent) 
{ 
    param_ = param;
    stopped_ = false;
    dumpCamPropsFlag_ = false;
}


void ImageGrabber::stopCapture()
{
    stopped_ = true;
}


void ImageGrabber::dumpCameraProperties()
{
    dumpCamPropsFlag_ = true;
};


void ImageGrabber::run()
{ 

    if (param_.captureMode == QString("camera"))
    {
        runCaptureFromCamera();
    }
    else if (param_.captureMode == QString("file"))
    {
        runCaptureFromFile();
    }
    else if (param_.captureMode == QString("directory"))
    {
        runCaptureFromDir();
    }
    else
    {
        QString errorMsg = QString("Unable to start capture: unknown capture mode ");
        errorMsg += param_.captureMode;
        emit cameraSetupError(errorMsg);
    }

    CameraInfo emptyInfo;
    emit newCameraInfo(emptyInfo);
    emit stopped();
}

void ImageGrabber::runCaptureFromCamera()
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
        QDateTime currentDateTime = QDateTime::currentDateTime();
        imageData.dateTime = double(currentDateTime.toMSecsSinceEpoch())*(1.0e-3);
        emit newImage(imageData);

        // dump camera properties
        if (dumpCamPropsFlag_)
        {
            dumpCamPropsFlag_ = false;
            std::cout << "DEBUG: dumping camera properties" << std::endl;
            std::ofstream camPropsStream;
            camPropsStream.open(DEBUG_DUMP_CAMERA_PROPS_FILE_NAME.toStdString());
            camPropsStream << (cameraPtr_  -> getAllPropertiesString());
            camPropsStream.close();
        }
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

}

void ImageGrabber::runCaptureFromFile()
{
    cv::VideoCapture fileCapture;

    // Open the capture input file
    try
    {
        fileCapture.open(param_.captureInputFile.toStdString());
    }
    catch(cv::Exception& exception)
    {
        QString errorMsg = QString("Error opening captureInputFile, ");
        errorMsg += param_.captureInputFile + QString(", - ");
        errorMsg += QString::fromStdString(exception.what());
        emit fileReadError(errorMsg);
        return;
    }
    if (!fileCapture.isOpened())
    {
        QString errorMsg = QString("Unable to open captureInputFile, ");
        errorMsg += param_.captureInputFile; 
        emit fileReadError(errorMsg);
        return;
    }

    // Get number of frames and fourcc
    unsigned int numFrames;
    int fourcc;
    try
    {
        numFrames = (unsigned int)(fileCapture.get(CV_CAP_PROP_FRAME_COUNT));
        fourcc = int(fileCapture.get(CV_CAP_PROP_FOURCC));
    }
    catch(cv::Exception& exception)
    {
        QString errorMsg = QString("Unable to get properties from captureInputFile, ");
        errorMsg += param_.captureInputFile + QString(", "); 
        errorMsg += QString::fromStdString(exception.what());
        emit fileReadError(errorMsg);
        return;
    }
    //std::cout << "fourcc: " << fourcc << std::endl;
    //std::cout << "numFrames: " << numFrames << std::endl;
    if ((fourcc == 0) || (fourcc == 808466521))
    {
        // --------------------------------------------------------------------
        // TEMPORARY - currently having problems with DIB/raw formats need to 
        // fix this
        // --------------------------------------------------------------------
        QString errorMsg = QString("Fourcc code is equal to 0 - this is currently not supported");
        emit fileReadError(errorMsg);
        return;
    }

    // Read frame from input file at frameRate.
    unsigned long frameCount = 0;
    //float sleepDt = 1.0e3/param_.frameRate;
    //float sleepDt = 0.5*1.0e3/param_.frameRate;
    float sleepDt = 0.2*1.0e3/param_.frameRate;
    

    std::cout << param_.captureInputFile.toStdString() << std::endl;
    std::cout << "begin play back" << std::endl;
    ImageData imageData;

    while ((!stopped_) && (frameCount < numFrames))
    {
        std::cout << (frameCount+1) << "/" << numFrames << std::endl;

        cv::Mat mat;

        try
        {
            fileCapture >> mat;
        }
        catch (cv::Exception &exception)
        {
            QString errorMsg = QString("Unable to read frame %1: ").arg(frameCount);
            errorMsg += QString::fromStdString(exception.what());
            emit fileReadError(errorMsg);
            stopped_ = true;
            continue;
        }
        catch (...)
        {
            QString errorMsg = QString("Unable to read frame %1: ").arg(frameCount);
            errorMsg += QString("an uknown exception occured");
            emit fileReadError(errorMsg);
            stopped_ = true;
            continue;

        }
        if (mat.empty())
        {
            // This shouldn't happen, but just in case 
            // skip any frames that come back empty.
            frameCount++;
            //std::cout << "empty frame" << std::endl;
            continue;
        }
        else
        {
            //std::cout << std::endl;
        }

        imageData.mat = mat.clone(); // Get deep copy of image mat (required)
        imageData.frameCount = frameCount; 
        QDateTime currentDateTime = QDateTime::currentDateTime();
        imageData.dateTime = double(currentDateTime.toMSecsSinceEpoch())*(1.0e-3);
        emit newImage(imageData);
        frameCount++;

        ThreadHelper::msleep(sleepDt);
        // DEBUG -  slow down
        // ------------------------------------------------------------------------
        //if (frameCount > 100)
        //{
        //    ThreadHelper::msleep(1000);
        //}
        //-------------------------------------------------------------------------
    }
    std::cout << "play back done" << std::endl;

    // Release file
    try
    {
        fileCapture.release();
    }
    catch(cv::Exception& exception)
    {
        QString errorMsg = QString("Error releasing captureInputFile, ");
        errorMsg += param_.captureInputFile + QString(", - ");
        errorMsg += QString::fromStdString(exception.what());
        emit fileReadError(errorMsg);
        return;
    }
    //std::cout << "end" << std::endl;
    return;
}


void ImageGrabber::runCaptureFromDir()
{
    float sleepDt = 1.0e3/param_.frameRate;
    unsigned long fileCount = 0;

    // Check that capture directory exists
    QDir captureInputDir = QDir(param_.captureInputDir);
    if (!captureInputDir.exists())
    {
        QString dirStr = captureInputDir.absolutePath();
        QString errorMsg = QString("Error: captureInputDir %1 does not exist.").arg(dirStr);
        emit fileReadError(errorMsg);
        return;
    }

    // Check that debug images sub-directory exists
    QDir debugImagesDir = QDir(captureInputDir.absolutePath() + "/debug_images");
    if (!debugImagesDir.exists())
    {
        QString dirStr = debugImagesDir.absolutePath();
        QString errorMsg = QString("Error: debug_images dir %1 does not exist.").arg(dirStr);
        emit fileReadError(errorMsg);
        return;
    }

    // Get names of image files in capture directory
    QStringList bmpFilter("*.bmp");
    debugImagesDir.setSorting(QDir::Name);
    QFileInfoList imageFileInfoList = debugImagesDir.entryInfoList(bmpFilter);

    // Create map from frame number in filename and sort images files by frame number
    QMap<int, QFileInfo> frameNumberToFileInfoMap;
    QString mapErrorMsg;
    bool mapOk = getFrameNumberToFileInfoMap(imageFileInfoList,frameNumberToFileInfoMap,mapErrorMsg);
    if (!mapOk)
    {
        emit fileReadError(mapErrorMsg);
        return;
    }
    QList<int> frameNumberList = frameNumberToFileInfoMap.keys();
    qSort(frameNumberList.begin(), frameNumberList.end());
    QListIterator<int> frameNumberIt(frameNumberList);

    // Read debug data log and create filter map
    QMap<int, bool> frameNumberFilterMap;  
    mapOk = getFrameNumberFilterMap(captureInputDir,frameNumberFilterMap, mapErrorMsg);
    if (!mapOk)
    {
        emit fileReadError(mapErrorMsg);
        return;
    }


    while ((!stopped_)  && frameNumberIt.hasNext())
    {
        ImageData imageData;

        int frameNumber = frameNumberIt.next();
        QFileInfo fileInfo = frameNumberToFileInfoMap[frameNumber];
        QString fileName = fileInfo.absoluteFilePath();
        //std::cout  << "count: " << fileCount << ", fileName " << fileName.toStdString(); 

        // Filter out undesired values
        if (!frameNumberFilterMap.contains(frameNumber))
        {
            QString errorMsg = QString("frame number not found in filter map for dir %1").arg(
                    captureInputDir.absolutePath()
                    );
            emit fileReadError(errorMsg);
            return;
        }
        bool filterValue = frameNumberFilterMap[frameNumber];
        if (!filterValue)
        {
            //std::cout << " - skipped (filtered)" << std::endl;
            continue;
        }

        // Make sure file exists
        if (!fileInfo.exists())
        {
            //std::cout <<" - skipped (doesn't exist)" << std::endl;
            continue;
        }
        //std::cout << std::endl;


        // Read image from file
        cv::Mat subImageMat;
        try
        { 
            subImageMat = cv::imread(fileName.toStdString());
        }
        catch (cv::Exception &exception)
        {
            QString errorMsg = QString("Unable to read image %1: ").arg(fileName);
            errorMsg += QString::fromStdString(exception.what());
            emit fileReadError(errorMsg);
            stopped_ = true;
            continue;
        }

        // Place mat in larger padded image
        unsigned int pad = 500;
        cv::Scalar padColor = cv::Scalar(255,255,255);
        cv::Size subImageSize = subImageMat.size();
        cv::Size fullImageSize = cv::Size(subImageSize.width+2*pad,subImageSize.height+2*pad);
        cv::Mat fullImageMat = cv::Mat(fullImageSize, CV_8UC3, padColor);
        cv::copyMakeBorder(subImageMat,fullImageMat, pad, pad, pad, pad, cv::BORDER_CONSTANT, padColor);

        imageData.mat = fullImageMat.clone(); // Get deep copy of image mat (required)
        imageData.frameCount = frameNumber; 
        QDateTime currentDateTime = QDateTime::currentDateTime();
        imageData.dateTime = double(currentDateTime.toMSecsSinceEpoch())*(1.0e-3);
        emit newImage(imageData);

        ThreadHelper::msleep(sleepDt);
        fileCount++;
    }

    return;
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
        QString errorMsg = QString(
                "Unable to get framerate property: "
                );
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    if (param_.frameRate < frameRateInfo.minAbsoluteValue)
    {
        QString errorMsg = QString(
                "framerate less than minimum allowed %1"
                ).arg(frameRateInfo.minAbsoluteValue);
        emit cameraSetupError(errorMsg);
        return false;

    }
    if (param_.frameRate > frameRateInfo.maxAbsoluteValue)
    {
        QString errorMsg = QString(
                "framerate greater than maximum allowed %1"
                ).arg(frameRateInfo.maxAbsoluteValue);
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
        QString errorMsg = QString(
                "gain less than minimum allowed %1"
                ).arg(gainInfo.minAbsoluteValue);
        emit cameraSetupError(errorMsg);
        return false;
    }
    if (param_.gain > gainInfo.maxAbsoluteValue)
    {
        QString errorMsg = QString(
                "gain greater than maximum allowed %1"
                ).arg(gainInfo.minAbsoluteValue);
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
        QString errorMsg = QString(
                "shutter less than minimum allowed %1"
                ).arg(shutterInfo.minAbsoluteValue);
        emit cameraSetupError(errorMsg);
        return false;
    }
    if (param_.shutter > shutterInfo.maxAbsoluteValue)
    {
        QString errorMsg = QString(
                "shutter greater than maximum allowed %1"
                ).arg(shutterInfo.minAbsoluteValue);
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
        QString errorMsg = QString(
                "brightness less than minimum allowed %1"
                ).arg(brightnessInfo.minValue);
        emit cameraSetupError(errorMsg);
        return false;
    }
    if (param_.brightness > brightnessInfo.maxValue)
    {
        QString errorMsg = QString(
                "brightness greater than maximum allowed %1"
                ).arg(brightnessInfo.minValue);
        emit cameraSetupError(errorMsg);
        return false;
    }

    brightnessProp.absoluteControl = false;
    brightnessProp.value = param_.brightness;
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

    // Set Gamma - if present
    PropertyInfo gammaInfo;
    Property gammaProp;
    try
    {
        gammaInfo = cameraPtr_ -> getPropertyInfo(PROPERTY_TYPE_GAMMA);
        gammaProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_GAMMA);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to get gamma property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }
    // Color camera specific - for development dont' set if can't 
    if ((gammaInfo.present) && (gammaInfo.manualCapable) && (gammaInfo.absoluteCapable))
    {
        //std::cout << "setting gamma" << std::endl; 

        if (param_.gamma < gammaInfo.minAbsoluteValue)
        {
            QString errorMsg = QString(
                    "gamma less than minimum allowed %1"
                    ).arg(gammaInfo.minAbsoluteValue);
            emit cameraSetupError(errorMsg);
            return false;
        }
        if (param_.gamma > gammaInfo.maxAbsoluteValue)
        {
            QString errorMsg = QString(
                    "gamma greater than maximum allowed %1"
                    ).arg(gammaInfo.maxAbsoluteValue);
            emit cameraSetupError(errorMsg);
            return false;
        }

        gammaProp.absoluteControl = true;
        gammaProp.absoluteValue = param_.gamma;
        gammaProp.autoActive = false;

        try
        {
            cameraPtr_ -> setProperty(gammaProp);
        }
        catch (RuntimeError &runtimeError)
        {
            QString errorMsg = QString("Unable to set gamma property: ");
            errorMsg += QString::fromStdString(runtimeError.what());
            emit cameraSetupError(errorMsg);
            return false;
        }
    }
    else
    {
        //std::cout << "not setting gamma" << std::endl; 
        // -------------------------------------------
        // TO DO ... emit warning if not present??
        // -------------------------------------------
    }

    // Set Saturation - if present
    PropertyInfo saturationInfo;
    Property saturationProp;
    try
    {
        saturationInfo = cameraPtr_ -> getPropertyInfo(PROPERTY_TYPE_SATURATION);
        saturationProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_SATURATION);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to get saturation property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    if ((saturationInfo.present) && (saturationInfo.manualCapable) && (saturationInfo.absoluteCapable))
    {
        //std::cout << "setting saturation" << std::endl; 

        if (param_.saturation < saturationInfo.minAbsoluteValue)
        {
            QString errorMsg = QString(
                    "saturation less than minimum allowed %1"
                    ).arg(saturationInfo.minAbsoluteValue);
            emit cameraSetupError(errorMsg);
            return false;
        }
        if (param_.saturation > saturationInfo.maxAbsoluteValue)
        {
            QString errorMsg = QString(
                    "saturation greater than maximum allowed %1"
                    ).arg(saturationInfo.maxAbsoluteValue);
            emit cameraSetupError(errorMsg);
            return false;
        }

        saturationProp.absoluteControl = true;
        saturationProp.absoluteValue = param_.saturation;
        saturationProp.autoActive = false;

        try
        {
            cameraPtr_ -> setProperty(saturationProp);
        }
        catch (RuntimeError &runtimeError)
        {
            QString errorMsg = QString("Unable to set saturation property: ");
            errorMsg += QString::fromStdString(runtimeError.what());
            emit cameraSetupError(errorMsg);
            return false;
        }
    }
    else
    {
        //std::cout << "not setting saturation" << std::endl;
        // ------------------------------------------------
        // TO DO ... emit waring if not present??
        // ------------------------------------------------
    }


    // Set whiteBalance (Red and Blue) - if present
    PropertyInfo whiteBalanceInfo;
    Property whiteBalanceProp;
    try
    {
        whiteBalanceInfo = cameraPtr_ -> getPropertyInfo(PROPERTY_TYPE_SATURATION);
        whiteBalanceProp = cameraPtr_ -> getProperty(PROPERTY_TYPE_SATURATION);
    }
    catch (RuntimeError &runtimeError)
    {
        QString errorMsg = QString("Unable to get whiteBalance property: ");
        errorMsg += QString::fromStdString(runtimeError.what());
        emit cameraSetupError(errorMsg);
        return false;
    }

    if ((whiteBalanceInfo.present) && (whiteBalanceInfo.manualCapable))
    {
        //std::cout << "setting whiteBalance" << std::endl; 

        if (param_.whiteBalanceRed < whiteBalanceInfo.minValue)
        {
            QString errorMsg = QString(
                    "whiteBalanceRed less than minimum allowed %1"
                    ).arg(whiteBalanceInfo.minValue);
            emit cameraSetupError(errorMsg);
            return false;
        }
        if (param_.whiteBalanceRed > whiteBalanceInfo.maxValue)
        {
            QString errorMsg = QString(
                    "whiteBalanceRed greater than maximum allowed %1"
                    ).arg(whiteBalanceInfo.maxValue);
            emit cameraSetupError(errorMsg);
            return false;
        }
        if (param_.whiteBalanceBlue < whiteBalanceInfo.minValue)
        {
            QString errorMsg = QString(
                    "whiteBalanceBlue less than minimum allowed %1"
                    ).arg(whiteBalanceInfo.minValue);
            emit cameraSetupError(errorMsg);
            return false;
        }
        if (param_.whiteBalanceBlue > whiteBalanceInfo.maxValue)
        {
            QString errorMsg = QString(
                    "whiteBalanceBlue greater than maximum allowed %1"
                    ).arg(whiteBalanceInfo.maxValue);
            emit cameraSetupError(errorMsg);
            return false;
        }
        whiteBalanceProp.absoluteControl = false;
        whiteBalanceProp.valueA = param_.whiteBalanceRed;
        whiteBalanceProp.valueB = param_.whiteBalanceBlue;
        whiteBalanceProp.autoActive = false;

        try
        {
            cameraPtr_ -> setProperty(whiteBalanceProp);
        }
        catch (RuntimeError &runtimeError)
        {
            QString errorMsg = QString("Unable to set whiteBalance property: ");
            errorMsg += QString::fromStdString(runtimeError.what());
            emit cameraSetupError(errorMsg);
            return false;
        }
    }
    else
    {
        //std::cout << "not setting whiteBalance" << std::endl;
        // ------------------------------------------------
        // TO DO ... emit waring if not present??
        // ------------------------------------------------
    }
    return true;
}


// Utility functions
// ----------------------------------------------------------------------------

bool getFrameNumberToFileInfoMap(
        QFileInfoList imageFileInfoList, 
        QMap<int,QFileInfo> &frameNumberToFileInfoMap,
        QString errorMsg
        )
{
    for (int i=0; i<imageFileInfoList.size(); i++)
    {
        bool intOk;

        QFileInfo fileInfo = imageFileInfoList[i];
        QString fileName = fileInfo.fileName();

        int frmInd = fileName.indexOf(QString("frm"));
        if (frmInd == -1)
        {
            errorMsg = QString("Error: unable to find frm in %1").arg(fileName);
            return false;
        }

        int frmScoreInd0 = fileName.indexOf(QString("_"),frmInd);
        if (frmScoreInd0 == -1)
        {
            errorMsg = QString("Error: unable to parse frame number in %1").arg(fileName);
            return false;
        }

        int frmScoreInd1 = fileName.indexOf(QString("_"),frmScoreInd0+1);
        if (frmScoreInd1 == -1)
        {
            errorMsg = QString("Error: unable to parse frame number in %1").arg(fileName);
            return false;
        }

        intOk = true;
        int frameNumber = fileName.mid(frmScoreInd0+1, frmScoreInd1-frmScoreInd0-1).toInt(&intOk);
        if (!intOk)
        {
            errorMsg = QString("Error: unable to convert frameNumber to int in file %1").arg(fileName);
            return false;
        }
        int cntInd = fileName.indexOf(QString("cnt"));
        if (cntInd == -1)
        {
            errorMsg = QString("Error: unable to find cnt in %1").arg(fileName);
            return false;
        }

        int cntScoreInd0 = fileName.indexOf(QString("_"), cntInd);
        if (cntScoreInd0 == -1)
        {
            errorMsg = QString("Error: unable to parse object count in %1").arg(fileName);
            return false;
        }

        int cntScoreInd1 = fileName.indexOf(QString("."), cntScoreInd0+1);
        if (cntScoreInd1 == -1)
        {
            errorMsg = QString("Error: unable to parse object count in %1").arg(fileName);
            return false;
        }

        intOk = true;
        int objectCount =  fileName.mid(cntScoreInd0+1,cntScoreInd1-cntScoreInd0-1).toInt(&intOk);
        if (!intOk)
        {
            errorMsg = QString("Error: unable to convert count to int in file %1").arg(fileName);
            return false;
        }
        int frameNumberWithCount = frameNumber*CAPTURE_FROM_DIR_FRAME_NUMBER_MUL + objectCount;
        if (!frameNumberToFileInfoMap.contains(frameNumberWithCount))
        { 
            frameNumberToFileInfoMap.insert(frameNumberWithCount, fileInfo);
        }
    }
    return true;
}


bool getFrameNumberFilterMap(
        QDir captureInputDir,
        QMap<int,bool> &frameNumberFilterMap,
        QString errorMsg
        )
{
    // Check that file exists
    QString logFileName("debug_data_log.txt");
    if (!captureInputDir.exists(logFileName))
    {
        errorMsg = QString("Error: %1 not found in capture directory").arg(logFileName); 
        return false;
    }

    // Open file
    QFile logFile(captureInputDir.absoluteFilePath(logFileName));
    if (!logFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        errorMsg = QString("Error: unable to open debug data log %1").arg(logFileName);
        return false;
    }

    // Read contents and create map
    QTextStream logStream(&logFile);
    bool inFrameCount = false;
    bool filterFlag = true;
    int frameNumberWithCount = -1;
    while (!logStream.atEnd())
    {
        QString line = logStream.readLine();
        QStringList lineList = line.split(" ",QString::SkipEmptyParts);
        lineList = lineList.replaceInStrings(QString(","),QString(""));
        if (lineList.isEmpty())
        {
            // Skip empty lines
            continue;
        }

        if (lineList.contains("Frame:") && lineList.contains("count:"))
        {
            inFrameCount = true;
            bool intOk = true;

            int frameNumber = lineList[1].toInt(&intOk);
            if (frameNumber == -1)
            {
                errorMsg = QString("Error: unable to parse frame number in %1").arg(
                        captureInputDir.absoluteFilePath(logFileName)
                        );
                return false;
            }

            int objectCount = lineList[3].toInt(&intOk);
            if (objectCount == -1)
            {
                errorMsg = QString("Error: unable to parse object count in %1").arg(
                        captureInputDir.absoluteFilePath(logFileName)
                        );
                return false;
            }
            
            frameNumberWithCount = frameNumber*CAPTURE_FROM_DIR_FRAME_NUMBER_MUL + objectCount;
        }
        else if (lineList.contains("onBorderX:") || lineList.contains("onBorderY:"))
        {
            bool intOk = true;
            int onBorderVal = lineList[1].toInt(&intOk);
            if ( (!intOk) || (onBorderVal < 0) || (onBorderVal > 1))
            {
                errorMsg = QString("Error: unable to parse onBorder value in %1").arg(
                        captureInputDir.absoluteFilePath(logFileName)
                        );
                return false;
            }
            if (onBorderVal == 1)
            {
                filterFlag = false;
            }
        }
        else if (lineList.contains("contourVector:"))
        {
            if (inFrameCount)
            {
                frameNumberFilterMap.insert(frameNumberWithCount,filterFlag);
            }
            inFrameCount = false;
            filterFlag = true;
        }
    }
    logFile.close();
    return true;
}
