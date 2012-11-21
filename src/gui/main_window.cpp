#include "main_window.hpp"
#include <iostream>
#include <QTimer>
#include <opencv2/core/core.hpp>
#include "camera_finder.hpp"
#include "mat_to_qimage.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    setupUi(this);
    initialize();
}

// Private slots
// ----------------------------------------------------------------------------
void MainWindow::startButtonClicked()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    counter_ = 0;
    cameraPtr_ -> startCapture();
    timer_ -> start(15);
}

void MainWindow::stopButtonClicked()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    cameraPtr_ -> stopCapture();
    timer_ -> stop();
}

void MainWindow::timerUpdate()
{
    std::cout << __PRETTY_FUNCTION__ << " " << counter_ << std::endl;
    cv::Mat mat = cameraPtr_ -> grabImage();
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
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    timer_ = new QTimer(this);
    connectWidgets();
    createCamera();
    havePixmap_ = false;

    imageLabel -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageLabel -> setAlignment(Qt::AlignCenter);
    imageLabel -> setMinimumSize(100,100);
}

void MainWindow::connectWidgets()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    connect(startButton,SIGNAL(clicked()),this,SLOT(startButtonClicked())); 
    connect(stopButton,SIGNAL(clicked()),this,SLOT(stopButtonClicked()));
    connect(timer_, SIGNAL(timeout()), this, SLOT(timerUpdate())); 
}

void MainWindow::createCamera()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    // Find cameras
    bias::CameraFinder cameraFinder;
    bias::CameraPtrList cameraPtrList = cameraFinder.createCameraPtrList();

    // Get first camera in list and connect
    cameraPtr_ = cameraPtrList.front();
    cameraPtr_ -> connect();
    cameraPtr_ -> printInfo();
}

void MainWindow::updateImageLabel()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    QPixmap pixmapScaled =  pixmapOriginal_.scaled(
            imageLabel -> size(),
            Qt::KeepAspectRatio, 
            Qt::SmoothTransformation
            );
    imageLabel -> setPixmap(pixmapScaled);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (havePixmap_)  
    {
        QSize sizeImageLabel = imageLabel -> size();
        QSize sizeAdjusted = pixmapOriginal_.size();
        sizeAdjusted.scale(sizeImageLabel, Qt::KeepAspectRatio);
        QSize sizeImageLabelPixmap = imageLabel -> pixmap() -> size();

        if (sizeImageLabelPixmap != sizeAdjusted) 
        {
            updateImageLabel();
        }
    }
}
