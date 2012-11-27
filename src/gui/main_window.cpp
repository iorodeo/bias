#include "main_window.hpp"
#include <iostream>
#include <memory>
#include <QTimer>
#include <QThreadPool>
#include <opencv2/core/core.hpp>
#include "camera_facade.hpp"
#include "mat_to_qimage.hpp"
#include "stamped_image.hpp"
#include "lockable_queue.hpp"
#include "image_grabber.hpp"
#include "image_dispatcher.hpp"

namespace bias
{

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
        imageGrabber_ = new ImageGrabber(cameraPtr_, newImageQueuePtr_);
        imageDispatcher_ = new ImageDispatcher(newImageQueuePtr_);

        threadPoolPtr_ -> start(imageGrabber_);
        threadPoolPtr_ -> start(imageDispatcher_);
        imageDisplayTimerPtr_ -> start(66);
    }

    void MainWindow::stopButtonClicked()
    {
        imageDisplayTimerPtr_ -> stop();
        imageGrabber_ -> stop();
        imageDispatcher_ -> stop();
        newImageQueuePtr_ -> clear();
    }

    void MainWindow::updateImageDisplay()
    {
        imageDispatcher_ -> acquireDisplayImageLock();
        cv::Mat mat = imageDispatcher_ -> getDisplayImage();
        QImage img = matToQImage(mat);
        imageDispatcher_ -> releaseDisplayImageLock();

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

        threadPoolPtr_ = new QThreadPool(this);
        imageDisplayTimerPtr_ = new QTimer(this);
        newImageQueuePtr_ = std::make_shared<LockableQueue<StampedImage>>();

        connect(imageDisplayTimerPtr_, SIGNAL(timeout()), this, SLOT(updateImageDisplay()));
    }

    void MainWindow::connectWidgets()
    {
        connect(startButton,SIGNAL(clicked()),this,SLOT(startButtonClicked())); 
        connect(stopButton,SIGNAL(clicked()),this,SLOT(stopButtonClicked()));
    }

    void MainWindow::createCamera()
    {

        // Find cameras
        CameraFinder cameraFinder;
        CameraPtrList cameraPtrList = cameraFinder.createCameraPtrList();
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
        //msg.sprintf("Hello");
        //painter.setPen(red);
        //painter.drawText(10,10, msg);
        //painter.setPen(grn);
        //painter.drawRect(40,40,50,50);
        // --------------------------------------

        imageLabel_ -> setPixmap(pixmapScaled);
    }

    void MainWindow::resizeEvent(QResizeEvent *event)
    {
        if (havePixmap_ && !(pixmapOriginal_.isNull()))  
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

} // namespace bias

