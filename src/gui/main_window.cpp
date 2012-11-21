#include "main_window.hpp"
#include <iostream>
#include <QTimer>
#include <opencv2/core/core.hpp>
#include "camera_finder.hpp"
#include "mat_to_qimage.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    initialize();
}

// Private slots
// ----------------------------------------------------------------------------
void MainWindow::startButtonClicked()
{
    std::cout << "startButtonClicked" << std::endl;
    counter_ = 0;
    cameraPtr_ -> startCapture();
    timer_ -> start(20);
}

void MainWindow::stopButtonClicked()
{
    std::cout << "stopButtonClicked" << std::endl;
    cameraPtr_ -> stopCapture();
    timer_ -> stop();
}

void MainWindow::timerUpdate()
{
    std::cout << " timerUpdate: counter = " << counter_ << std::endl;
    cv::Mat mat = cameraPtr_ -> grabImage();
    QImage img = matToQImage(mat);
    pixmapOriginal_ = QPixmap::fromImage(img);
    pixmapScaled_ = pixmapOriginal_.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); 
    imageLabel -> setPixmap(pixmapScaled_);
    counter_++;
}

// Private methods
// ----------------------------------------------------------------------------

void MainWindow::initialize()
{
    timer_ = new QTimer(this);
    connectWidgets();
    createCamera();
    //imageLabel -> setScaledContents(true);
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
    std::cout << "number of cameras: " << cameraFinder.numberOfCameras() << std::endl;
    bias::CameraPtrList cameraPtrList = cameraFinder.createCameraPtrList();

    // Get first camera in list and connect
    cameraPtr_ = cameraPtrList.front();
    cameraPtr_ -> connect();
    cameraPtr_ -> printInfo();
}

void MainWindow::updateImageLabel()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    std::cout << "MainWindow::resizeEvent" << std::endl;
    QSize scaledSize = pixmapScaled_.size();

}
