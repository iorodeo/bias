#include "fly_sorter_window.hpp"
#include "mat_to_qimage.hpp"
#include "json.hpp"
#include "json_utils.hpp"
#include "rtn_status.hpp"
#include "ext_ctl_http_server.hpp"
#include <QMessageBox>
#include <QThreadPool>
#include <QTimer>
#include <QPainter>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDateTime>
#include <iostream>
#include <list>
#include <random>
#include <cmath>


// Constants
// ----------------------------------------------------------------------------
const unsigned int MAX_THREAD_COUNT=5;
const unsigned int MAX_HTTP_REQUEST_ERROR = 10;
const double DEFAULT_DISPLAY_FREQ = 15.0; // Hz
const QSize PREVIEW_DUMMY_IMAGE_SIZE = QSize(320,256);
const QString DEFAULT_PARAMETER_FILENAME = QString("fly_sorter_param.json");
const QString TRAINING_DATA_BASE_STRING = QString("training_data");
const QString TRAINING_VIDEO_BASE_STRING = QString("training_video");
const QString DEBUG_IMAGES_BASE_STRING = QString("debug_images");
const QString CLASSIFIER_DIRECTORY = QString("classifiers");
const unsigned int DEFAULT_HTTP_SERVER_PORT = 5010; 


// Public Methods
// ----------------------------------------------------------------------------

FlySorterWindow::FlySorterWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    connectWidgets();
    initialize();
}


bool FlySorterWindow::isRunning()
{
    return running_;
}


RtnStatus FlySorterWindow::startRunning()
{
    RtnStatus rtnStatus;

    if (!running_)
    {
        // Setup sorting and tracking
        blobFinder_ = BlobFinder(param_.blobFinder);
        identityTracker_ = IdentityTracker(param_.identityTracker);
        flySegmenter_ = FlySegmenter(param_.flySegmenter);
        hogPositionFitter_ = HogPositionFitter(param_.hogPositionFitter);
        genderSorter_ = GenderSorter(param_.genderSorter);

        // Create training data
        if (trainingDataCheckBoxPtr_ -> checkState() == Qt::Checked)
        {
            if (isTrainingDataModeSingle())
            {
                if (isCaptureModeFile())
                {
                    setupTrainingDataWrite(param_.imageGrabber.captureInputFile);
                }
                else
                {
                    setupTrainingDataWrite(param_.imageGrabber.captureInputDir);
                }
            }
            else
            {
                // Batch trainging mode

                if (isCaptureModeFile())
                {
                    // Update list of batch video files 
                    bool success = updateBatchVideoFileList();
                    if (!success)
                    {
                        rtnStatus.success = false;
                        rtnStatus.message = QString("unable to update batch video file list");
                        return rtnStatus;
                    }
                    batchVideoFileIndex_ = 0;
                }
                else 
                {
                    // Update list of batch video directories 
                    bool success = updateBatchVideoDirList();
                    if (!success)
                    {
                        rtnStatus.success = false;
                        rtnStatus.message = QString("unable to update batch video directory list");
                        return rtnStatus;
                    }
                    batchVideoDirIndex_ = 0;
                }
                setupBatchDataWrite();
            }
        }
        else
        {
            hogPositionFitter_.trainingDataWriteDisable();
        }

        // Create debug image log
        if (createDebugImages())
        {
            setupDebugImagesWrite();
        }

        // Create debug data log
        if (createDebugLog())
        {
            debugDataLogStream_.open("debug_data_log.txt");
        }

        stopRunningFlag_ = false;
        startImageCapture();

        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }
    else
    {
        rtnStatus.success = false;
        rtnStatus.message = QString("already running");
        return rtnStatus;
    }
}


RtnStatus FlySorterWindow::stopRunning()
{ 
    RtnStatus rtnStatus;
    stopRunningFlag_ = true;
    stopImageCapture();

    rtnStatus.success = true;
    rtnStatus.message = QString("");
    return rtnStatus;
}


RtnStatus FlySorterWindow::getStatus(QVariantMap &statusMap)
{
    RtnStatus rtnStatus;
    statusMap.insert("running", running_);
    statusMap.insert("createTrainingData", createTrainingData());
    if (isTrainingDataModeBatch())
    {
        statusMap.insert("trainingModeCheckbox", QString("batch"));
    }
    else
    {
        statusMap.insert("trainingModeCheckbox", QString("single"));
    }
    
    if ((httpOutputCheckBoxPtr_ -> checkState()) == Qt::Checked)
    {
        statusMap.insert("httpOutputCheckbox", true);
    }
    else
    {
        statusMap.insert("httpOutputCheckbox", false);
    }
    statusMap.insert("configuration", paramMap_);
    return rtnStatus;
}


// Protected methods
// ----------------------------------------------------------------------------

void FlySorterWindow::showEvent(QShowEvent *event)
{
    resizeAllImageLabels();
}


void FlySorterWindow::resizeEvent(QResizeEvent *event)
{
    resizeAllImageLabels();
}


void FlySorterWindow::closeEvent(QCloseEvent *event)
{
    if (running_)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Close Request");
        msgBox.setText("The application is currently capturing images:");
        msgBox.setInformativeText("Do you want to stop and close the window?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Cancel)
        {
            event -> ignore();
            return;
        }
        else 
        {
            emit stopCapture();
            threadPoolPtr_ -> waitForDone();
        }
    }
    event -> accept();
}


// Private Slots
// ----------------------------------------------------------------------------

void FlySorterWindow::startPushButtonClicked()
{
    if (!running_)
    { 
        RtnStatus rtnStatus = startRunning();
        if (!rtnStatus.success)
        {
            QString errMsgTitle("Start Error");
            QMessageBox::critical(this,errMsgTitle,rtnStatus.message);
        }
    }
    else
    {
        stopRunning();
    }
}



void FlySorterWindow::startImageCapture()
{
    if (!running_)
    {

        ImageGrabberParam imageGrabberParam = param_.imageGrabber;
        if (createTrainingData() && isTrainingDataModeBatch())
        {
            if (isCaptureModeFile())
            {
                imageGrabberParam.captureInputFile = batchVideoFileList_[batchVideoFileIndex_];
            }
            else
            {
                imageGrabberParam.captureInputDir = batchVideoDirList_[batchVideoDirIndex_];
            }
        }
        imageGrabberPtr_ = new ImageGrabber(imageGrabberParam); 

        connect(
                imageGrabberPtr_,
                SIGNAL(cameraSetupError(QString)),
                this,
                SLOT(cameraSetupError(QString))
               );

        connect(
                imageGrabberPtr_,
                SIGNAL(fileReadError(QString)),
                this,
                SLOT(imageGrabberFileReadError(QString))
               );

        connect(
                this,
                SIGNAL(stopCapture()),
                imageGrabberPtr_,
                SLOT(stopCapture())
                );

        connect(
                this,
                SIGNAL(dumpCameraProperties()),
                imageGrabberPtr_,
                SLOT(dumpCameraProperties())
               );

        connect(
                imageGrabberPtr_,
                SIGNAL(stopped()),
                this,
                SLOT(OnImageCaptureStopped())
               );


        qRegisterMetaType<ImageData>("ImageData");
        connect(
                imageGrabberPtr_,
                SIGNAL(newImage(ImageData)),
                this,
                SLOT(newImage(ImageData))
                );

        running_ = true;
        threadPoolPtr_ -> start(imageGrabberPtr_);

        startPushButtonPtr_ -> setText("Stop");
        reloadPushButtonPtr_ -> setEnabled(false);
        trainingDataCheckBoxPtr_ -> setEnabled(false);
        if (param_.imageGrabber.captureMode == QString("camera"))
        {
            actionDumpCameraPropsPtr_ -> setEnabled(true);
        }
    }
}

void FlySorterWindow::stopImageCapture()
{
    if (running_)
    {
        emit stopCapture();
        OnImageCaptureStopped();
    }
}


void FlySorterWindow::reloadPushButtonClicked()
{
    bool isRunning = running_;
    if (isRunning)
    { 
        stopImageCapture();
    }
    loadParamFromFile();
    updateParamText();
    updateWidgetsOnLoad();
    if (isRunning)
    {
        startImageCapture();
    }
}


void FlySorterWindow::httpOutputCheckBoxChanged(int state) 
{ 
    if (state == Qt::Checked)
    {
        httpRequestErrorCount_ = 0;
    }
}


void FlySorterWindow::trainingDataCheckBoxChanged(int state)
{
    if (state == Qt::Checked)
    {
        singleRadioButtonPtr_ -> setEnabled(true);
        batchRadioButtonPtr_ -> setEnabled(true);
    }
    else
    {
        singleRadioButtonPtr_ -> setChecked(true);
        singleRadioButtonPtr_ -> setEnabled(false);
        batchRadioButtonPtr_ -> setEnabled(false);
    }
}


void FlySorterWindow::newImage(ImageData imageData)
{
    if (running_)
    {
        imageData_.copy(imageData);

        // Find flies and sort by gender
        blobFinderData_ = blobFinder_.findBlobs(imageData_.mat);
        identityTracker_.update(blobFinderData_);
        flySegmenterData_ = flySegmenter_.segment(blobFinderData_);

        hogPositionFitterData_ = hogPositionFitter_.fit(
                flySegmenterData_,
                imageData.frameCount,
                imageData.mat
                );

        genderSorterData_ = genderSorter_.sort(hogPositionFitterData_);

        // Send position and gender data via http.
        if ((httpOutputCheckBoxPtr_ -> checkState()) == Qt::Checked) 
        {
            sendDataViaHttpRequest();
        }

        // Write debug data images
        if (actionDebugRawImagesPtr_ -> isChecked())
        {
            QString fileName = QString("raw_frm_%1.bmp").arg(imageData.frameCount);
            QString pathName = debugImagesDir_.absoluteFilePath(fileName);
            cv::imwrite(pathName.toStdString(),imageData.mat);
        }
        if (actionDebugBoundingImagesPtr_ -> isChecked())
        {
            int cnt;
            GenderDataList::iterator it;
            GenderDataList genderDataList = genderSorterData_.genderDataList;
            for (it=genderDataList.begin(), cnt=0; it!=genderDataList.end(); it++,cnt++)
            {
                GenderData data = *it;
                // Bounding image
                QString fileName = QString("bnd_frm_%1_cnt_%2.bmp").arg(imageData.frameCount).arg(cnt);
                QString pathName = debugImagesDir_.absoluteFilePath(fileName);
                cv::Mat boundingImage = data.positionData.segmentData.blobData.boundingImage;
                cv::imwrite(pathName.toStdString(),boundingImage);
            }
        }

        // Write debug data log
        if (actionDebugDataLogPtr_ -> isChecked())
        {
            int cnt;
            GenderDataList::iterator it;
            GenderDataList genderDataList = genderSorterData_.genderDataList;
            for (it=genderDataList.begin(), cnt=0; it!=genderDataList.end(); it++,cnt++)
            {
                GenderData data = *it;
                debugDataLogStream_ << "Frame: " << imageData.frameCount; 
                debugDataLogStream_ << ", count: " << cnt << std::endl;
                debugDataLogStream_ << data.toStdString(1) << std::endl;
            }
        }

        //// DEBUG - write images to file
        //// -------------------------------------------------------------------------
        //if (0)
        //{
        //    QString imgFileName = QString("image_%1.bmp").arg(imageData.frameCount);
        //    cv::imwrite(imgFileName.toStdString(),imageData.mat);
        //}
        //// -------------------------------------------------------------------------

    }
}

void FlySorterWindow::updateDisplayOnTimer()
{
    GenderDataList genderDataList = genderSorterData_.genderDataList;
    GenderDataList::iterator it;
    for (it=genderDataList.begin(); it!=genderDataList.end(); it++)
    {
        GenderData genderData = *it;

        if ((genderData.gender == MALE) || (genderData.gender == FEMALE))
        {
            int x = int(genderData.positionData.meanXAbs);
            int y = int(genderData.positionData.meanYAbs);
            std::string letter = GenderSorter::GenderToLetter(genderData.gender);
            cv::putText(
                    blobFinderData_.blobDataImage, 
                    letter, 
                    cv::Point(x,y),
                    cv::FONT_HERSHEY_SIMPLEX,
                    4.0,
                    cv::Scalar(0,0,255),
                    2,
                    CV_AA
                    );
        }
    }

    QImage previewImage = matToQImage(blobFinderData_.blobDataImage);
    if (!previewImage.isNull()) 
    {
        previewPixmapOrig_ = QPixmap::fromImage(previewImage);
    }

    QImage thresholdImage = matToQImage(blobFinderData_.thresholdImage);
    if (!thresholdImage.isNull())
    {
        thresholdPixmapOrig_ = QPixmap::fromImage(thresholdImage);
    }
    updateAllImageLabels();
}


void FlySorterWindow::networkAccessManagerFinished(QNetworkReply *reply)
{ 
    if (reply->error() == QNetworkReply::NoError)
    {
        std::cout << "http request ok" << std::endl;
    }
    else
    {
        std::cout << "http request error" << std::endl;
        httpRequestErrorCount_++;
        if (httpRequestErrorCount_ == MAX_HTTP_REQUEST_ERROR)
        { 
            httpOutputCheckBoxPtr_ -> setCheckState(Qt::Unchecked);
            QString errMsgTitle("Http Request Error");
            QString errMsgText("Too many request errors - stopping http output");
            QMessageBox::critical(this, errMsgTitle, errMsgText);
        }
    }
    delete reply;
}


void FlySorterWindow::cameraSetupError(QString message)
{ 
    QString errMsgTitle("Camera Setup Error");
    QMessageBox::critical(this, errMsgTitle, message);
}


void FlySorterWindow::imageGrabberFileReadError(QString message)
{
    stopRunning();
    QString errMsgTitle("Image Grabber File Read Error");
    QMessageBox::critical(this, errMsgTitle, message);
}


void FlySorterWindow::OnImageCaptureStopped()
{
        threadPoolPtr_ -> waitForDone();
        running_ = false;

        if (createTrainingData() && isTrainingDataModeBatch() && !stopRunningFlag_)
        {
            bool batchComplete = false;

            if (isCaptureModeFile())
            {
                batchVideoFileIndex_++;
                if (batchVideoFileIndex_ >= batchVideoFileList_.size())
                {
                    batchComplete = true;
                }
            }
            else
            {
                batchVideoDirIndex_++;
                if (batchVideoDirIndex_ >= batchVideoDirList_.size())
                {
                    batchComplete = true;
                }
            }

            if (!batchComplete)
            { 
                setupBatchDataWrite();
                running_ = false;
                startImageCapture();
                return;
            }
            else
            {
                statusbar -> clearMessage();
            }
        }

        if (actionDebugDataLogPtr_ -> isChecked())
        {
            debugDataLogStream_.close();
        }

        startPushButtonPtr_ -> setText("Start");
        reloadPushButtonPtr_ -> setEnabled(true);
        if (param_.imageGrabber.captureMode == QString("file"))
        {
            trainingDataCheckBoxPtr_ -> setEnabled(true);
        }
        actionDumpCameraPropsPtr_ -> setEnabled(false);
}


void FlySorterWindow::actionDumpCameraPropsTriggered()
{
    if (running_)
    {
        emit dumpCameraProperties();
    }
}



// Private Methods
// ----------------------------------------------------------------------------

void FlySorterWindow::connectWidgets()
{

    connect(
            startPushButtonPtr_,
            SIGNAL(clicked()),
            this,
            SLOT(startPushButtonClicked())
           );

    connect(
            reloadPushButtonPtr_,
            SIGNAL(clicked()),
            this,
            SLOT(reloadPushButtonClicked())
           );

    connect(
            httpOutputCheckBoxPtr_,
            SIGNAL(stateChanged(int)),
            this,
            SLOT(httpOutputCheckBoxChanged(int))
           );

    connect(
            trainingDataCheckBoxPtr_,
            SIGNAL(stateChanged(int)),
            this,
            SLOT(trainingDataCheckBoxChanged(int))
           );

    connect(
            actionDumpCameraPropsPtr_,
            SIGNAL(triggered()),
            this,
            SLOT(actionDumpCameraPropsTriggered())
           );
}



void FlySorterWindow::initialize()
{
    running_ = false;
    stopRunningFlag_ = false;
    httpRequestErrorCount_ = 0;
    displayFreq_ = DEFAULT_DISPLAY_FREQ;
    parameterFileName_ = DEFAULT_PARAMETER_FILENAME;
    threadPoolPtr_ = new QThreadPool(this);
    threadPoolPtr_ -> setMaxThreadCount(MAX_THREAD_COUNT);
    httpServerPort_ = DEFAULT_HTTP_SERVER_PORT;

    // Set up lists for batch running in file input mode
    batchVideoFileList_.clear();
    batchVideoFileIndex_ = 0; 

    // Set up list for batch running in directory input mode
    batchVideoDirList_.clear();
    batchVideoDirIndex_ = 0;

    setupImageLabels();
    setupDisplayTimer();
    setupNetworkAccessManager();
    loadParamFromFile();
    updateParamText();
    updateWidgetsOnLoad();
    startHttpServer();


}


void FlySorterWindow::updateAllImageLabels()
{
    updateImageLabel(previewLabelPtr_,  previewPixmapOrig_);
    updateImageLabel(thresholdLabelPtr_, thresholdPixmapOrig_);
}


void FlySorterWindow::updateImageLabel(QLabel *labelPtr,QPixmap &pixmap) 
{ 
    QPixmap pixmapScaled =  pixmap.scaled(
            labelPtr -> size(),
            Qt::KeepAspectRatio, 
            Qt::SmoothTransformation
            );

    if (blobFinderData_.success)
    {
        QPainter painter(&pixmapScaled);
        QString msg;  
        msg.sprintf("# Blobs: %d", blobFinderData_.blobDataList.size());
        painter.setPen(QColor(0,255,0));
        painter.drawText(5,12, msg);
    }
   
    labelPtr -> setPixmap(pixmapScaled);
}


void FlySorterWindow::resizeAllImageLabels()
{
    resizeImageLabel(previewLabelPtr_, previewPixmapOrig_);
    resizeImageLabel(thresholdLabelPtr_, thresholdPixmapOrig_);
}


void FlySorterWindow::resizeImageLabel(QLabel *labelPtr, QPixmap &pixmap)
{ 
    if (pixmap.isNull() || ((labelPtr -> pixmap()) == 0))  
    {
        return;
    }
    QSize imageLabelSize = labelPtr -> size();
    QSize adjustedSize = pixmap.size();
    adjustedSize.scale(imageLabelSize, Qt::KeepAspectRatio);
    QSize pixmapSize = labelPtr -> pixmap() -> size();
    if (pixmapSize != adjustedSize) 
    {
        updateImageLabel(labelPtr, pixmap);
    }
}


void FlySorterWindow::setupImageLabels()
{
    QImage dummyImage;
    dummyImage = QImage(PREVIEW_DUMMY_IMAGE_SIZE,QImage::Format_RGB888);
    dummyImage.fill(QColor(Qt::gray).rgb());
    previewPixmapOrig_ = QPixmap::fromImage(dummyImage);
    thresholdPixmapOrig_ = QPixmap::fromImage(dummyImage);
    updateAllImageLabels();
}


void FlySorterWindow::setupDisplayTimer()
{
        displayTimerPtr_ = new QTimer(this);
        connect(
                displayTimerPtr_, 
                SIGNAL(timeout()), 
                this, 
                SLOT(updateDisplayOnTimer())
                );

        unsigned int displayDt = int(1000.0/displayFreq_);
        displayTimerPtr_ -> start(displayDt);
}


void FlySorterWindow::setupNetworkAccessManager()
{
    networkAccessManagerPtr_ = new QNetworkAccessManager(this);
    connect(
            networkAccessManagerPtr_, 
            SIGNAL(finished(QNetworkReply*)),
            this, 
            SLOT(networkAccessManagerFinished(QNetworkReply*))
            );
}


void FlySorterWindow::sendDataViaHttpRequest()
{ 
    // Only send data if we have data
    if (genderSorterData_.genderDataList.size() > 0) 
    {
        QString reqString = QString("http://%1").arg(param_.server.address);
        reqString += QString(":%1").arg(param_.server.port); 
        QString jsonString = QString(dataToJson());
        jsonString.replace(" ", "");
        reqString += QString("/sendCmdGetRsp/sendData/") + jsonString;
        QUrl reqUrl = QUrl(reqString);
        //std::cout << "http request: " << reqUrl.toString().toStdString() << std::endl;
        QNetworkRequest req(reqUrl);
        QNetworkReply *reply = networkAccessManagerPtr_ -> get(req);
    }
}


void FlySorterWindow::startHttpServer()
{ 
    httpServerPtr_ = new ExtCtlHttpServer(this,this);
    httpServerPtr_ -> listen(QHostAddress::Any, httpServerPort_);
}


QVariantMap FlySorterWindow::dataToMap()
{
    QVariantMap dataMap;
    MotionDirection direction = param_.identityTracker.motionDirection;
    GenderDataList genderDataList = genderSorterData_.genderDataList;
    GenderDataList::iterator it;
    QVariantList detectionList;

    // Add number of detections - note certain detections on the border are
    // ignored based motion direction.
    int numBlobs = 0;
    if (direction == MOTION_DIRECTION_Y)
    {
        numBlobs = getNumBlobsExcludeYBorder(blobFinderData_.blobDataList);
    }
    else
    {
        numBlobs = getNumBlobsExcludeXBorder(blobFinderData_.blobDataList);
    } 
    std::cout << "numBlobs: " << numBlobs << std::endl;
    dataMap.insert("ndetections", numBlobs);

    for (it=genderDataList.begin(); it!=genderDataList.end(); it++)
    {
        GenderData genderData = *it;
        BlobData blobData = genderData.positionData.segmentData.blobData;
        QVariant id = QVariant::fromValue<long>(
                genderData.positionData.segmentData.blobData.id
                );
        QVariantMap detectionMap;

        // Skip certain on the border cases depending on motion direction
        bool skipData = false;
        skipData |= blobData.onBorderX && (direction == MOTION_DIRECTION_X);
        skipData |= blobData.onBorderY && (direction == MOTION_DIRECTION_Y);
        if (skipData)
        {
            continue;
        }

        // Check to see if fly is out of bounds - how this is determined depends 
        // on motion direction
        bool isOutOfBounds = false; 
        isOutOfBounds |= blobData.onBorderX && (direction == MOTION_DIRECTION_Y);
        isOutOfBounds |= blobData.onBorderY && (direction == MOTION_DIRECTION_X);
        if (isOutOfBounds)
        { 
            detectionMap.insert("fly_type", "outofbounds"); 
            int x = blobData.boundingRect.x + blobData.boundingRect.width/2;
            int y = blobData.boundingRect.y + blobData.boundingRect.height/2;
            detectionMap.insert("x", genderData.positionData.meanXAbs);
            detectionMap.insert("y", genderData.positionData.meanYAbs);
        } 
        else   
        {
            // Normal detection - report classifiaction data
            if (blobData.old)
            {
                detectionMap.insert("fly_type", "old"); 
            }
            else
            {
                if (genderData.positionData.isMultipleFlies)
                {
                    detectionMap.insert("fly_type", "multiple");
                }
                else
                {
                    QString genderString = QString::fromStdString( 
                            GenderSorter::GenderToString(genderData.gender)
                            );
                    detectionMap.insert("fly_type", genderString); 
                }
            }
            detectionMap.insert("x", genderData.positionData.meanXAbs);
            detectionMap.insert("y", genderData.positionData.meanYAbs);
        }
        detectionMap.insert("fly_id", id);
        detectionList.push_back(detectionMap);
    }
    dataMap.insert("detections", detectionList);
    dataMap.insert("time_acquired", imageData_.dateTime);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    double sendDateTime = double(currentDateTime.toMSecsSinceEpoch())*(1.0e-3);
    dataMap.insert("time_sent", sendDateTime);
    return dataMap;
}


QByteArray FlySorterWindow::dataToJson()
{
    QVariantMap map = dataToMap();
    bool ok;
    QByteArray json = QtJson::serialize(map,ok);
    if (!ok)
    {
        QByteArray emptyByteArray = QByteArray();
        return emptyByteArray;
    }
    return json;

}


void FlySorterWindow::loadParamFromFile()
{ 
    RtnStatus rtnStatus;
    QString errMsgTitle("Load Parameter Error");

    QFile parameterFile(parameterFileName_);
    if (!parameterFile.exists())
    {
        QString errMsgText = QString("Parameter file, %1").arg(parameterFileName_);
        errMsgText += QString(", does not exist - using default values");
        QMessageBox::critical(this, errMsgTitle, errMsgText);
        param_ = FlySorterParam(); 
        return;
    }

    bool ok = parameterFile.open(QIODevice::ReadOnly);
    if (!ok)
    {
        QString errMsgText = QString("Unable to open parameter file %1").arg(parameterFileName_);
        errMsgText += QString(" - using default values");
        QMessageBox::critical(this, errMsgTitle, errMsgText);
        param_ = FlySorterParam();
        return;
    }

    QByteArray paramJson = parameterFile.readAll();
    parameterFile.close();

    QVariantMap paramMap = QtJson::parse(QString(paramJson), ok).toMap();
    if (!ok)
    {
        QString errMsgText = QString("Unable to parse configuration in %1").arg(parameterFileName_); 
        errMsgText += " - using default values";
        QMessageBox::critical(this, errMsgTitle, errMsgText);
        param_ = FlySorterParam();
        return;
    }
    FlySorterParam paramNew;
    rtnStatus = paramNew.fromMap(paramMap);
    if (!rtnStatus.success)
    {
        QString errMsgText = QString("%1: ").arg(parameterFileName_);
        errMsgText += rtnStatus.message + QString(" - using default values");
        QMessageBox::critical(this, errMsgTitle, errMsgText);
        param_ = FlySorterParam();
        return;
    }

    // DEVELOP
    // ------------------------------------------------------------------------
    rtnStatus = param_.flySegmenter.classifier.loadFromFile(CLASSIFIER_DIRECTORY);
    if (!rtnStatus.success)
    {
        QString errMsgText = QString("%1: ").arg(parameterFileName_);
        errMsgText += rtnStatus.message + QString(" - using default values");
        QMessageBox::critical(this, errMsgTitle, errMsgText);
        param_ = FlySorterParam();
        return;
    }

    
    // ------------------------------------------------------------------------
    param_ = paramNew;
    paramMap_ = paramMap;

}


void FlySorterWindow::updateParamText()
{
    QByteArray paramJson = param_.toJson();
    QByteArray prettyParamJson = prettyIndentJson(paramJson);
    paramsTextEditPtr_ -> setPlainText(QString(prettyParamJson));
}


void FlySorterWindow::updateWidgetsOnLoad()
{
    if (isCaptureModeFile() || isCaptureModeDir())
    {
        trainingDataCheckBoxPtr_ -> setEnabled(true);
        if (trainingDataCheckBoxPtr_ -> checkState() == Qt::Checked)
        {
            singleRadioButtonPtr_ -> setEnabled(true);
            batchRadioButtonPtr_ -> setEnabled(true);
        }
        else
        {
            singleRadioButtonPtr_ -> setEnabled(false);
            batchRadioButtonPtr_ -> setEnabled(false);
        }
    }
    else
    {
        trainingDataCheckBoxPtr_ -> setCheckState(Qt::Unchecked);
        trainingDataCheckBoxPtr_ -> setEnabled(false);
        singleRadioButtonPtr_ -> setEnabled(false);
        batchRadioButtonPtr_ -> setEnabled(false);
    }
    actionDumpCameraPropsPtr_ -> setEnabled(false);
}


void FlySorterWindow::setupTrainingDataWrite(QString name)
{
    // Get application directory
    QString appPathString = QCoreApplication::applicationDirPath();
    QDir appDir = QDir(appPathString);

    // Create training data base directory if it doesn't exist
    QString baseDirString = TRAINING_DATA_BASE_STRING;
    QDir baseDir = QDir(appDir.absolutePath() + "/" + baseDirString);
    if (!baseDir.exists())
    {
        appDir.mkdir(baseDirString);
    }

    // Create training data directory if it doesn't exist
    QString videoPrefix; 
    if (isCaptureModeFile())
    {
        videoPrefix = name.split(".", QString::SkipEmptyParts).at(0);
    }
    else
    {
        videoPrefix = QDir(name).dirName(); 
    }

    QDir dataDir = QDir(baseDir.absolutePath() + "/" + videoPrefix);
    if (!dataDir.exists())
    {
        baseDir.mkdir(videoPrefix);
    }

    // Create training data file prefix.
    QString dataPrefix = dataDir.absoluteFilePath("data");
    hogPositionFitter_.trainingDataWriteEnable(dataPrefix.toStdString());
}


void FlySorterWindow::setupBatchDataWrite()
{
    QString videoName;
    QStringList statusMsg; 
    statusMsg << QString("Batch "); 

    if (isCaptureModeFile())
    {
        QFileInfo videoFileInfo(batchVideoFileList_[batchVideoFileIndex_]);
        videoName = videoFileInfo.fileName();
        statusMsg << QString::number(batchVideoFileIndex_+1);
        statusMsg << QString("/");  
        statusMsg << QString::number(batchVideoFileList_.size());
    }
    else
    {
        videoName = batchVideoDirList_[batchVideoDirIndex_];
        statusMsg << QString::number(batchVideoDirIndex_+1);
        statusMsg << QString("/");  
        statusMsg << QString::number(batchVideoDirList_.size());
    }
    statusMsg << QString("  "); 
    statusMsg << videoName;
    statusbar -> showMessage(statusMsg.join(QString("")));
    setupTrainingDataWrite(videoName);
}


void FlySorterWindow::setupDebugImagesWrite()
{
    // Get application directory
    QString appPathString = QCoreApplication::applicationDirPath();
    QDir appDir = QDir(appPathString);

    // Create images base directory if it doesn't exist
    QString imagesDirString = DEBUG_IMAGES_BASE_STRING;
    debugImagesDir_ = QDir(appDir.absolutePath() + "/" + imagesDirString);
    if (!debugImagesDir_.exists())
    {
        appDir.mkdir(imagesDirString);
    }

    // Get list of '.bmp' files in images directory
    debugImagesDir_.setNameFilters(QStringList()<<"*.bmp");
    QList<QString> imageNameList  = debugImagesDir_.entryList();
    if (!imageNameList.empty())
    {
        for (int i=0; i<imageNameList.size(); i++)
        {
            debugImagesDir_.remove(imageNameList[i]); 
        }
    }
};


TrainingDataMode FlySorterWindow::getTrainingDataMode()
{
    if (singleRadioButtonPtr_ -> isChecked())
    {
        return TRAINING_DATA_MODE_SINGLE;
    }
    else
    {
        return TRAINING_DATA_MODE_BATCH;
    }
}


bool FlySorterWindow::isTrainingDataModeSingle()
{
    if (getTrainingDataMode() == TRAINING_DATA_MODE_SINGLE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool FlySorterWindow::isTrainingDataModeBatch()
{
    if (getTrainingDataMode() == TRAINING_DATA_MODE_BATCH)
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool FlySorterWindow::isCaptureModeFile()
{ 
    return param_.imageGrabber.captureMode == QString("file");
}

bool FlySorterWindow::isCaptureModeDir()
{
    return param_.imageGrabber.captureMode == QString("directory");
}


bool FlySorterWindow::updateBatchVideoFileList()
{
    // Get application directory
    QString appPathString = QCoreApplication::applicationDirPath();
    QDir appDir = QDir(appPathString);

    // Check to see if video file base directory exists 
    QString videoDirString = TRAINING_VIDEO_BASE_STRING;
    QDir videoDir = QDir(appDir.absolutePath() + "/" + videoDirString);
    if (!videoDir.exists())
    {
        QString errMsgTitle("Batch Training Data Error");
        QString message = QString("%1 subdirectory does not exist").arg(videoDirString);
        QMessageBox::critical(this, errMsgTitle, message);
        return false;
    }

    // Get list of '.avi' files in video file directory
    videoDir.setNameFilters(QStringList()<<"*.avi");
    QStringList videoNameList  = videoDir.entryList();
    if (videoNameList.empty())
    {
        QString errMsgTitle("Batch Training Data Error");
        QString message = QString("no .avi video files found in %1 subdirectory").arg(videoDirString);
        QMessageBox::critical(this, errMsgTitle, message);
        return false;
    }

    std::cout << std::endl;
    std::cout << "batch video file list" << std::endl;
    for (int i=0; i< videoNameList.size(); i++)
    {
        std::cout << " " << i << " " << videoNameList[i].toStdString() << std::endl;
    }
    std::cout << std::endl;

    // Get absolute path of video files
    batchVideoFileList_.clear();
    QStringListIterator nameIt(videoNameList);
    while (nameIt.hasNext())
    {
        QString absoluteFilePath = videoDir.absoluteFilePath(nameIt.next());
        batchVideoFileList_.push_back(absoluteFilePath);
    }
    return true;
}


bool FlySorterWindow::updateBatchVideoDirList()
{
    // Get application directory
    QString appPathString = QCoreApplication::applicationDirPath();
    QDir appDir = QDir(appPathString);

    // Check to see if video file base directory exists 
    QString videoDirString = TRAINING_VIDEO_BASE_STRING;
    QDir videoDir = QDir(appDir.absolutePath() + "/" + videoDirString);
    if (!videoDir.exists())
    {
        QString errMsgTitle("Batch Training Data Error");
        QString message = QString("%1 subdirectory does not exist").arg(videoDirString);
        QMessageBox::critical(this, errMsgTitle, message);
        return false;
    }

    // Get list of sub-directories
    videoDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList  videoDirList = videoDir.entryList();

    if (videoDirList.empty())
    {
        QString errMsgTitle("Batch Training Data Error");
        QString message = QString("no cropped image directories found in %1 subdirectory").arg(videoDirString);
        QMessageBox::critical(this, errMsgTitle, message);
        return false;
    }

    // Get absolute path of cropped image "video" directories
    batchVideoDirList_.clear();
    QStringListIterator dirIt(videoDirList);
    while(dirIt.hasNext())
    {
        //QString absoluteImagesDirPath = videoDir.absoluteFilePath(dirIt.next() + "/debug_images");
        //batchVideoDirList_.push_back(absoluteImagesDirPath);
        QString absoluteDirPath = videoDir.absoluteFilePath(dirIt.next());
        batchVideoDirList_.push_back(absoluteDirPath);
    }
    return true;
}

bool FlySorterWindow::createTrainingData()
{ 
    // Returns true is trainging data should be created
    if (trainingDataCheckBoxPtr_ -> checkState() == Qt::Checked)
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool FlySorterWindow::createDebugImages()
{
    // Returns true is debug images should be created
    bool value = actionDebugBoundingImagesPtr_ -> isChecked(); 
    value |= actionDebugRawImagesPtr_ -> isChecked();
    return value;
}


bool FlySorterWindow::createDebugLog()
{
    // Returns true is debug log should be created
    return actionDebugDataLogPtr_ -> isChecked();
}
