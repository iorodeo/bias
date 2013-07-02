#include "fly_sorter_window.hpp"
#include "mat_to_qimage.hpp"
#include "json.hpp"
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
#include <iostream>
#include <list>
#include <random>

// Constants
// ----------------------------------------------------------------------------
const unsigned int MAX_THREAD_COUNT=5;
const unsigned int MAX_HTTP_REQUEST_ERROR = 10;
const double DEFAULT_DISPLAY_FREQ = 15.0; // Hz
const QSize PREVIEW_DUMMY_IMAGE_SIZE = QSize(320,256);


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
    std::cout << "show" << std::endl;
    resizeAllImageLabels();
}


void FlySorterWindow::resizeEvent(QResizeEvent *event)
{
    std::cout << "resize" << std::endl;
    resizeAllImageLabels();
}


void FlySorterWindow::closeEvent(QCloseEvent *event)
{
    if (running_)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Close Request");
        msgBox.setText("The application is currently running:");
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
    if (running_ == false)
    {
        imageGrabberPtr_ = new ImageGrabber(param_.imageGrabber); 

        connect(
                imageGrabberPtr_,
                SIGNAL(cameraSetupError(QString)),
                this,
                SLOT(cameraSetupError(QString))
               );
                  

        connect(
                this,
                SIGNAL(stopCapture()),
                imageGrabberPtr_,
                SLOT(stopCapture())
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
    }
    else
    {
        emit stopCapture();
        threadPoolPtr_ -> waitForDone();
        running_ = false;
        startPushButtonPtr_ -> setText("Start");
    }
}


void FlySorterWindow::httpOutputCheckBoxChanged(int state) 
{ 
    if (state == Qt::Checked)
    {
        httpRequestErrorCount_=0;
    }
}


void FlySorterWindow::newImage(ImageData imageData)
{
    imageData_ = imageData;
    BlobFinder blobFinder = BlobFinder(param_.blobFinder);
    blobFinderData_ = blobFinder.findBlobs(imageData.mat);

    if ((httpOutputCheckBoxPtr_ -> checkState()) == Qt::Checked)
    {
        sendDataViaHttpRequest();
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
    threadPoolPtr_ = new QThreadPool(this);
    threadPoolPtr_ -> setMaxThreadCount(MAX_THREAD_COUNT);

    setupImageLabels();
    setupDisplayTimer();
    setupNetworkAccessManager();

    // Temp
    // -----------------------------------
    param_ = FlySorterParam();

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
        painter.setPen(QColor(0,0,255));
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
    reqString += QString("/sendData/") + jsonString;
    QUrl reqUrl = QUrl(reqString);
    //reqUrl.addQueryItem("sendData", jsonString);
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

    std::default_random_engine generator;
    std::uniform_int_distribution<unsigned int> distribution(0,1);


    for (it=blobFinderData_.blobDataList.begin(); it!=blobFinderData_.blobDataList.end(); it++)
    {
        BlobData blobData = *it;
        QVariantMap detectionMap;
        unsigned int coinFlip = distribution(generator);
        if (coinFlip == 0)
        {
            detectionMap.insert("fly_type", "male");
        }
        else
        {
            detectionMap.insert("fly_type", "female");
        }
        detectionMap.insert("fly_id", cnt);
        detectionMap.insert("x",blobData.centroid.x);
        detectionMap.insert("y",blobData.centroid.y);
        detectionList.push_back(detectionMap);
        cnt++;
    }
    dataMap.insert("detections", detectionList);
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
