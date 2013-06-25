#include "fly_sorter_window.hpp"
#include <QMessageBox>
#include <QThreadPool>
#include <iostream>
#include <list>

// Constants
// ----------------------------------------------------------------------------
const unsigned int MAX_THREAD_COUNT=5;


// Public Methods
// ----------------------------------------------------------------------------

FlySorterWindow::FlySorterWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    connectWidgets();
    initialize();
}

// Private Slots
// ----------------------------------------------------------------------------

void FlySorterWindow::startPushButtonClicked()
{
    if (running_ == false)
    {
        imageGrabberPtr_ = new ImageGrabber(param_.imageGrabber); 
        threadPoolPtr_ -> start(imageGrabberPtr_);

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
    std::cout << __PRETTY_FUNCTION__ << ", state = " << state << std::endl;
}


void FlySorterWindow::newImage(ImageData imageData)
{
    std::cout << "new image: " << imageData.frameCount << std::endl;
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
    threadPoolPtr_ = new QThreadPool(this);
    threadPoolPtr_ -> setMaxThreadCount(MAX_THREAD_COUNT);

    param_.imageGrabber.frameRate = 10.0;
}


void FlySorterWindow::updateImageLabels()
{
}


