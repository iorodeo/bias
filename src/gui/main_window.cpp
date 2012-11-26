#include "main_window.hpp"
#include <iostream>
#include <memory>
#include <QTimer>
#include <QThreadPool>
#include <opencv2/core/core.hpp>
#include "camera_finder.hpp"
#include "mat_to_qimage.hpp"
#include "image_grabber.hpp"
#include "image_processor.hpp"

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
    imageGrabber_ = new ImageGrabber(cameraPtr_, imagePoolPtr_);
    imageProcessor_ = new ImageProcessor(imagePoolPtr_);
    threadPool_ -> start(imageGrabber_);
    threadPool_ -> start(imageProcessor_);
    imageDisplayTimer_ -> start(66);
}

void MainWindow::stopButtonClicked()
{
    imageDisplayTimer_ ->stop();
    imageGrabber_ -> stop();
    imageProcessor_ -> stop();
    imagePoolPtr_ -> empty();
}

void MainWindow::updateImageDisplay()
{
    std::cout << "update image display" << std::endl;

    imageProcessor_ -> acquireDisplayImageLock();
    cv::Mat mat = imageProcessor_ -> getDisplayImage();
    QImage img = matToQImage(mat);
    imageProcessor_ -> releaseDisplayImageLock();

    pixmapOriginal_ = QPixmap::fromImage(img);
    updateImageLabel();
    havePixmap_ = true;
}


// Private methods
// ----------------------------------------------------------------------------

void MainWindow::initialize()
{
    havePixmap_ = false;
    connectWidgets();
    createCamera();

    threadPool_ = new QThreadPool(this);
    imagePoolPtr_ = std::make_shared<bias::ImagePool>();
    imageDisplayTimer_ = new QTimer;
    connect(imageDisplayTimer_, SIGNAL(timeout()), this, SLOT(updateImageDisplay()));

}

void MainWindow::connectWidgets()
{
    connect(startButton,SIGNAL(clicked()),this,SLOT(startButtonClicked())); 
    connect(stopButton,SIGNAL(clicked()),this,SLOT(stopButtonClicked()));
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
    //QPainter painter(&pixmapScaled);
    //QColor red(255,0,0);
    ////QColor grn(0,255,0);
    //QString msg;  
    //msg.sprintf("Count: %d",counter_);
    //painter.setPen(red);
    //painter.drawText(10,10, msg);
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
