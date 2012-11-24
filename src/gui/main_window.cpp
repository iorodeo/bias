#include "main_window.hpp"
#include <iostream>
#include <QTimer>
#include <QThread>
#include <opencv2/core/core.hpp>
#include "camera_finder.hpp"
#include "mat_to_qimage.hpp"
#include "image_grabber.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    initialize();
}

bool MainWindow::haveCamera()
{
    return haveCamera_;
}

// Private slots
// ----------------------------------------------------------------------------
void MainWindow::startButtonClicked()
{
    //counter_ = 0;
    //cameraPtr_ -> startCapture();
    //timer_ -> start(15);
    
}

void MainWindow::stopButtonClicked()
{
    //cameraPtr_ -> stopCapture();
    //timer_ -> stop();
}

void MainWindow::timerUpdate()
{
    std::cout << "timerUpdate" << std::endl;

    imagePool_.acquireOldImageLock();
    cv::Mat mat = imagePool_.dequeueOldImage();
    imagePool_.releaseOldImageLock();

    cameraPtr_ -> grabImage(mat);

    imagePool_.acquireNewImageLock();
    imagePool_.enqueueNewImage(mat);
    imagePool_.releaseNewImageLock();

    QImage img = matToQImage(mat);
    pixmapOriginal_ = QPixmap::fromImage(img);
    updateImageLabel();
    havePixmap_ = true;
    counter_++;
}

// Private methods
// ----------------------------------------------------------------------------

void MainWindow::initialize()
{
    timer_ = new QTimer(this);
    connectWidgets();
    createCamera();

    if (haveCamera_) 
    {
        havePixmap_ = false;
        testTimer_ = new QTimer(this);
        connect(testTimer_, SIGNAL(timeout()), this, SLOT(testTimerUpdate()));
        testTimer_ -> start(100);

        imageGrabber_ = new ImageGrabber(cameraPtr_);

    }
}

void MainWindow::testTimerUpdate()
{
    std::cout << "testTimerUpdate" << std::endl;
    cv::Mat img;
    bool haveNewImage;

    while (true)
    {
        haveNewImage = false;

        imagePool_.acquireNewImageLock();
        if (imagePool_.numberOfNewImages() > 0) {
            std::cout << " newImage -> oldImage" << std::endl;
            img = imagePool_.dequeueNewImage();
            haveNewImage = true;
        }
        imagePool_.releaseNewImageLock();

        if (haveNewImage) 
        {
            imagePool_.acquireOldImageLock();
            imagePool_.enqueueOldImage(img);
            imagePool_.releaseOldImageLock();
        }
        else
        {
            break;
        }
    }
}

void MainWindow::connectWidgets()
{
    connect(startButton,SIGNAL(clicked()),this,SLOT(startButtonClicked())); 
    connect(stopButton,SIGNAL(clicked()),this,SLOT(stopButtonClicked()));
    connect(timer_, SIGNAL(timeout()), this, SLOT(timerUpdate())); 
}

void MainWindow::createCamera()
{

    // Find cameras
    bias::CameraFinder cameraFinder;
    bias::CameraPtrList cameraPtrList = cameraFinder.createCameraPtrList();
    if (cameraPtrList.empty())
    {
        haveCamera_ = false;
    }
    else 
    {
        // Get first camera in list and connect
        cameraPtr_ = cameraPtrList.front();
        cameraPtr_ -> connect();
        cameraPtr_ -> printInfo();
        haveCamera_ = true;
    }
}

void MainWindow::updateImageLabel()
{
    QPixmap pixmapScaled =  pixmapOriginal_.scaled(
            imageLabel_ -> size(),
            Qt::KeepAspectRatio, 
            Qt::SmoothTransformation
            );

    // -------------------------------------
    // Drawing test
    // -------------------------------------
    QPainter painter(&pixmapScaled);
    QColor red(255,0,0);
    //QColor grn(0,255,0);
    QString msg;  
    msg.sprintf("Count: %d",counter_);
    painter.setPen(red);
    painter.drawText(10,10, msg);
    //painter.setPen(grn);
    //painter.drawRect(40,40,50,50);
    // --------------------------------------

    imageLabel_ -> setPixmap(pixmapScaled);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (havePixmap_)  
    {
        QSize sizeImageLabel = imageLabel_ -> size();
        QSize sizeAdjusted = pixmapOriginal_.size();
        sizeAdjusted.scale(sizeImageLabel, Qt::KeepAspectRatio);
        QSize sizeImageLabelPixmap = imageLabel_ -> pixmap() -> size();

        if (sizeImageLabelPixmap != sizeAdjusted) 
        {
            updateImageLabel();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    std::cout << "closeEvent" << std::endl;
    event -> accept();
}
