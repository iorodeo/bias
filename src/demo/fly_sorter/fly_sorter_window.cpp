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
#include <QDir>
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

        // Setup sorting and tracking
        blobFinder_ = BlobFinder(param_.blobFinder);
        identityTracker_ = IdentityTracker(param_.identityTracker);
        flySegmenter_ = FlySegmenter(param_.flySegmenter);
        hogPositionFitter_ = HogPositionFitter(param_.hogPositionFitter);
        genderSorter_ = GenderSorter(param_.genderSorter);

        // Create training data
        setupTrainingDataWrite(param_.imageGrabber.captureInputFile);


        startImageCapture();

        // DEBUG - open debug output data file
        // --------------------------------------------------------------------
        //debugStream.open("debug_data.txt");
    }
    else
    {
        stopImageCapture();

        // DEBUG - close debug output data file
        // --------------------------------------------------------------------
        //debugStream.close();
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

        running_ = true;
        threadPoolPtr_ -> start(imageGrabberPtr_);
        startPushButtonPtr_ -> setText("Stop");
        reloadPushButtonPtr_ -> setEnabled(false);
        trainingDataCheckBoxPtr_ -> setEnabled(false);
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
    //std::cout << "write training data ";
    //if (state == Qt::Checked)
    //{
    //    std::cout << "checked";
    //}
    //else
    //{
    //    std::cout << "unchecked";
    //}
    //std::cout << std::endl;
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

        //// DEBUG - display gender data
        //// -------------------------------------------------------------------------
        //if (0)
        //{
        //    GenderDataList genderDataList = genderSorterData_.genderDataList;
        //    GenderDataList::iterator it;

        //    for (it=genderDataList.begin(); it!=genderDataList.end(); it++)
        //    {
        //        GenderData data = *it;
        //        std::cout << data.toStdString(1) << std::endl;
        //    }
        //}
        //// -------------------------------------------------------------------------

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
        if (param_.imageGrabber.captureMode == QString("file"))
        {
            trainingDataCheckBoxPtr_ -> setEnabled(true);
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
    updateWidgetsOnLoad();

    // Temporary
    // ----------------------------------------------------------------------------
    //distribution_ = std::uniform_int_distribution<unsigned int>(0,1);
    //QString appDirPath = QCoreApplication::applicationDirPath();
    //std::cout << "applicationDirPath = " << appDirPath.toStdString() << std::endl;
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

    int numOkBlobs = getNumberOkItems(blobFinderData_.blobDataList);
    dataMap.insert("ndetections", numOkBlobs);
    
    GenderDataList genderDataList = genderSorterData_.genderDataList;
    GenderDataList::iterator it;
    QVariantList detectionList;

    for (it=genderDataList.begin(); it!=genderDataList.end(); it++)
    {
        GenderData genderData = *it;

        bool isOnBorder = genderData.positionData.segmentData.blobData.isOnBorder();
        bool success = genderData.positionData.success;

        if (isOnBorder ) 
        {
            // Throw away data on the border.
            continue;
        }

        QVariantMap detectionMap;

        QString genderString = QString::fromStdString( 
                GenderSorter::GenderToString(genderData.gender)
                );

        QVariant id = QVariant::fromValue<long>(
                genderData.positionData.segmentData.blobData.id
                );

        detectionMap.insert("fly_type", genderString); 
        detectionMap.insert("fly_id", id);
        detectionMap.insert("x", genderData.positionData.meanXAbs);
        detectionMap.insert("y", genderData.positionData.meanYAbs);
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


void FlySorterWindow::updateWidgetsOnLoad()
{
    if (param_.imageGrabber.captureMode == QString("file"))
    {
        trainingDataCheckBoxPtr_ -> setEnabled(true);
    }
    else
    {
        trainingDataCheckBoxPtr_ -> setCheckState(Qt::Unchecked);
        trainingDataCheckBoxPtr_ -> setEnabled(false);
    }
}


void FlySorterWindow::setupTrainingDataWrite(QString videoFileName)
{
    if (trainingDataCheckBoxPtr_ -> checkState() == Qt::Checked)
    {
        // Get application directory
        QString appPathString = QCoreApplication::applicationDirPath();
        QDir appDir = QDir(appPathString);

        // Create training data base directory if it doesn't exist
        QString baseDirString = QString("training_data");
        QDir baseDir = QDir(appDir.absolutePath() + "/" + baseDirString);
        if (!baseDir.exists())
        {
            appDir.mkdir(baseDirString);
        }

        // Create trianing data directory if it doesn't exist
        QString videoPrefix = videoFileName.split(".", QString::SkipEmptyParts).at(0);
        QDir dataDir = QDir(baseDir.absolutePath() + "/" + videoPrefix);
        if (!dataDir.exists())
        {
            baseDir.mkdir(videoPrefix);
        }

        // Create training data file prefix.
        QString dataPrefix = dataDir.absoluteFilePath("data");
        hogPositionFitter_.trainingDataWriteEnable(dataPrefix.toStdString());
    }
    else
    {
        hogPositionFitter_.trainingDataWriteDisable();
    }
}
