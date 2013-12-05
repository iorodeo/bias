#include "fly_sorter_window.hpp"
#include "mat_to_qimage.hpp"
#include "json.hpp"
#include "json_utils.hpp"
#include "rtn_status.hpp"
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

// Public Methods
// ----------------------------------------------------------------------------

FlySorterWindow::FlySorterWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    connectWidgets();
    initialize();
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
        debugStream.open("debug_data.txt");
        startImageCapture();
    }
    else
    {
        stopImageCapture();
        debugStream.close();
    }
}


void FlySorterWindow::startImageCapture()
{
    if (!running_)
    {
        imageGrabberPtr_ = new ImageGrabber(param_.imageGrabber); 

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

        threadPoolPtr_ -> start(imageGrabberPtr_);
        running_ = true;
        startPushButtonPtr_ -> setText("Stop");
        reloadPushButtonPtr_ -> setEnabled(false);
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


void FlySorterWindow::newImage(ImageData imageData)
{
    if (running_)
    {
        std::cout << imageData.frameCount << std::endl;

        imageData_.copy(imageData);

        BlobFinder blobFinder = BlobFinder(param_.blobFinder);
        blobFinderData_ = blobFinder.findBlobs(imageData_.mat);

        FlySegmenter flySegmenter = FlySegmenter(param_.flySegmenter);
        flySegmenterData_ = flySegmenter.segment(blobFinderData_);

        HogPositionFitter hogPositionFitter = HogPositionFitter(param_.hogPositionFitter);
        hogPositionFitterData_ = hogPositionFitter.fit(flySegmenterData_,imageData.frameCount,imageData.mat);

        GenderSorter genderSorter = GenderSorter(param_.genderSorter);
        genderSorterData_ = genderSorter.sort(hogPositionFitterData_);


        debugStream << "Frame Count: " << imageData.frameCount << std::endl;
        if (hogPositionFitterData_.positionDataList.empty())
        {
            debugStream << "  " << "PositionData: None"  << std::endl;
        }
        else
        {
            debugStream << "  " << "PositionData: "  << std::endl;

            PositionDataList::iterator it;
            unsigned int count;
            for (
                    it  = hogPositionFitterData_.positionDataList.begin(), count=0;
                    it != hogPositionFitterData_.positionDataList.end();
                    it++, count++
                )
            {
                PositionData posData = *it;
                debugStream << std::endl;
                debugStream << "    " << "count:            " << count << std::endl;
                debugStream << "    " << "success:          " << posData.success << std::endl;
                debugStream << "    " << "isFly:            " << posData.isFly << std::endl; 
                debugStream << "    " << "isMultipleFlies:  " << posData.isMultipleFlies << std::endl;
                debugStream << "    " << "bodyArea:         " << posData.bodyArea << std::endl;
                debugStream << std::endl;
            }
        }

        // Write images to file
        // ----------------------------------------------------------------------
        //QString imgFileName = QString("image_%1.bmp").arg(imageData.frameCount);
        //cv::imwrite(imgFileName.toStdString(),imageData.mat);
        // ----------------------------------------------------------------------


        if ((httpOutputCheckBoxPtr_ -> checkState()) == Qt::Checked)
        {
            sendDataViaHttpRequest();
        }
    }
}

void FlySorterWindow::updateDisplayOnTimer()
{
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
    QString errMsgTitle("Image Grabber File Read Error");
    QMessageBox::critical(this, errMsgTitle, message);
}

void FlySorterWindow::OnImageCaptureStopped()
{
        threadPoolPtr_ -> waitForDone();
        running_ = false;
        startPushButtonPtr_ -> setText("Start");
        reloadPushButtonPtr_ -> setEnabled(true);
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
}


void FlySorterWindow::initialize()
{
    running_ = false;
    httpRequestErrorCount_ = 0;
    displayFreq_ = DEFAULT_DISPLAY_FREQ;
    parameterFileName_ = DEFAULT_PARAMETER_FILENAME;
    threadPoolPtr_ = new QThreadPool(this);
    threadPoolPtr_ -> setMaxThreadCount(MAX_THREAD_COUNT);

    setupImageLabels();
    setupDisplayTimer();
    setupNetworkAccessManager();
    loadParamFromFile();
    updateParamText();

    // Temporary
    // --------------------------------------------------------------------------
    QString appDirPath = QCoreApplication::applicationDirPath();
    std::cout << "applicationDirPath = " << appDirPath.toStdString() << std::endl;
    distribution_ = std::uniform_int_distribution<unsigned int>(0,1);

    //
    //int n = 15;
    //int m = 2*n+1;
    //cv::Mat se = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(m,m));
    //for (int i=0; i<m; i++)
    //{
    //    for (int j=0; j<m; j++)
    //    {
    //        std::cout << int(se.at<uchar>(i,j)) << " ";
    //    }
    //    std::cout << std::endl;
    //}
    //

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
    QString reqString = QString("http://%1").arg(param_.server.address);
    reqString += QString(":%1").arg(param_.server.port); 
    QString jsonString = QString(dataToJson());
    jsonString.replace(" ", "");
    reqString += QString("/sendCmdGetRsp/sendData/") + jsonString;
    QUrl reqUrl = QUrl(reqString);
    std::cout << "http request: " << reqUrl.toString().toStdString() << std::endl;
    QNetworkRequest req(reqUrl);
    QNetworkReply *reply = networkAccessManagerPtr_ -> get(req);
}


QVariantMap FlySorterWindow::dataToMap()
{
    QVariantMap dataMap;
    dataMap.insert("ndetections", blobFinderData_.blobDataList.size());
    
    QVariantList detectionList;
    BlobDataList::iterator it;
    unsigned int cnt = 0;

    // Development
    // ------------------------------------------------------------------------
    static bool isNextFlyFemale = true;
    // -------------------------------------------------------------------------


    for (it=blobFinderData_.blobDataList.begin(); it!=blobFinderData_.blobDataList.end(); it++)
    {
        BlobData blobData = *it;
        QVariantMap detectionMap;

        // Devel - select fly gender based on genderMode setting
        // ----------------------------------------------------------------------
        switch (param_.genderMode)
        {
            case GenderModeMaleOnly:
                detectionMap.insert("fly_type", "male");
                break;

            case GenderModeEveryOther:
                if (isNextFlyFemale)
                {
                    detectionMap.insert("fly_type", "female");
                    isNextFlyFemale = false;
                }
                else
                {
                    detectionMap.insert("fly_type", "male");
                    isNextFlyFemale = true;
                }
                break;

            default:
                unsigned int coinFlip = distribution_(generator_);
                if (coinFlip == 0)
                {
                    detectionMap.insert("fly_type", "male");
                }
                else
                {
                    detectionMap.insert("fly_type", "female");
                }
                break;
        }
        // --------------------------------------------------------------------
        
        detectionMap.insert("fly_id", cnt);
        detectionMap.insert("x",blobData.centroid.x);
        detectionMap.insert("y",blobData.centroid.y);
        detectionList.push_back(detectionMap);
        cnt++;
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
    RtnStatus rtnStatus = paramNew.fromMap(paramMap);
    if (!rtnStatus.success)
    {
        QString errMsgText = QString("%1: ").arg(parameterFileName_);
        errMsgText += rtnStatus.message + QString(" - using default values");
        QMessageBox::critical(this, errMsgTitle, errMsgText);
        param_ = FlySorterParam();
        return;
    }
    param_ = paramNew;
}


void FlySorterWindow::updateParamText()
{
    QByteArray paramJson = param_.toJson();
    QByteArray prettyParamJson = prettyIndentJson(paramJson);
    paramsTextEditPtr_ -> setPlainText(QString(prettyParamJson));
}
