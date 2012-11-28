#include "camera_window.hpp"
#include "camera_facade.hpp"
#include "mat_to_qimage.hpp"
#include "stamped_image.hpp"
#include "lockable.hpp"
#include "lockable_queue.hpp"
#include "image_grabber.hpp"
#include "image_dispatcher.hpp"
#include <QtGui>
#include <QTimer>
#include <QThreadPool>
#include <opencv2/core/core.hpp>
#include <iostream>

namespace bias
{

    // Public methods
    // ----------------------------------------------------------------------------------


    CameraWindow::CameraWindow(Guid cameraGuid, QWidget *parent) : QMainWindow(parent)
    {
        setupUi(this);
        connectWidgets();
        initialize(cameraGuid);
    }

    // Protected methods
    // ----------------------------------------------------------------------------------

    void CameraWindow::resizeEvent(QResizeEvent *event)
    {
        if (havePixmap_ && !(pixmapOriginal_.isNull()))  
        {
            QSize sizeImageLabel = imageLabelPtr_ -> size();
            QSize sizeAdjusted = pixmapOriginal_.size();
            sizeAdjusted.scale(sizeImageLabel, Qt::KeepAspectRatio);
            QSize sizeImageLabelPixmap = imageLabelPtr_ -> pixmap() -> size();
            if (sizeImageLabelPixmap != sizeAdjusted) 
            {
                updateImageLabel();
            }
        }
    }

    void CameraWindow::closeEvent(QCloseEvent *event)
    {
        if (capturing_)
        {
            stopImageCapture();
            threadPoolPtr_ -> waitForDone();
        }

        if (connected_)
        {
            disconnectCamera();
        }

        event -> accept();
    }

    // Private slots
    // ----------------------------------------------------------------------------------

    void CameraWindow::connectButtonClicked()
    {
        (!connected_) ? connectCamera() : disconnectCamera();
    }

    void CameraWindow::startButtonClicked()
    { 
        (!capturing_) ? startImageCapture() : stopImageCapture();
    }
   

    void CameraWindow::updateImageDisplay()
    {
        imageDispatcher_ -> acquireLock();
        cv::Mat mat = imageDispatcher_ -> getImage();
        double fps = imageDispatcher_ -> getFPS();
        QImage img = matToQImage(mat);
        imageDispatcher_ -> releaseLock();

        if (!img.isNull()) 
        {
            pixmapOriginal_ = QPixmap::fromImage(img);
            updateImageLabel();
            QString statusMsg;
            statusMsg.sprintf("Capturing: %1.1f fps", fps);
            statusbarPtr_ -> showMessage(statusMsg);
            havePixmap_ = true;
        }
    }


    // Private methods
    // -----------------------------------------------------------------------------------

    void CameraWindow::initialize(Guid guid)
    {
        connected_ = false;
        capturing_ = false;
        havePixmap_ = false;
        imageDisplayDt_ = DEFAULT_IMAGE_DISPLAY_DT;
        cameraPtr_ = std::make_shared<Lockable<Camera>>(guid);

        setupImageDisplayTimer();

        threadPoolPtr_ = new QThreadPool(this);
        newImageQueuePtr_ = std::make_shared<LockableQueue<StampedImage>>();

        connectButtonPtr_ -> setText(QString("Connect"));

        cameraPtr_ -> acquireLock();
        Guid cameraGuid = cameraPtr_ -> getGuid();
        cameraPtr_ -> releaseLock();

        QString guidString("GUID: ");
        guidString += QString::fromStdString(cameraGuid.toString());
        guidLabelPtr_ -> setText(guidString);
        connectButtonPtr_ -> setEnabled(true);
        statusbarPtr_ -> showMessage(QString("Camera found, disconnected"));

        startButtonPtr_ -> setText(QString("Start"));
        startButtonPtr_ -> setEnabled(false);
        settingsButtonPtr_ -> setEnabled(false);
    }

    void CameraWindow::connectWidgets()
    {
        connect(startButtonPtr_, SIGNAL(clicked()), this, SLOT(startButtonClicked())); 
        connect(connectButtonPtr_, SIGNAL(clicked()), this, SLOT(connectButtonClicked()));
    }

    void CameraWindow::setupImageDisplayTimer()
    {
        imageDisplayTimerPtr_ = new QTimer(this);
        connect(imageDisplayTimerPtr_, SIGNAL(timeout()), this, SLOT(updateImageDisplay()));
    }

    void CameraWindow::updateImageLabel()
    {
        QPixmap pixmapScaled =  pixmapOriginal_.scaled(
                imageLabelPtr_ -> size(),
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

        imageLabelPtr_ -> setPixmap(pixmapScaled);
    } 

    void CameraWindow::connectCamera() 
    {
        cameraPtr_ -> acquireLock();
        cameraPtr_ -> connect();
        cameraPtr_ -> releaseLock();

        connectButtonPtr_ -> setText(QString("Disconnect"));
        startButtonPtr_ -> setEnabled(true);
        statusbarPtr_ -> showMessage(QString("Connected, Stopped"));
        connected_ = true;
    }

    void CameraWindow::disconnectCamera()
    {
        if (capturing_) 
        {
            stopImageCapture();
        }

        cameraPtr_ -> acquireLock();
        cameraPtr_ -> disconnect();
        cameraPtr_ -> releaseLock();

        connectButtonPtr_ -> setText(QString("Connect"));
        startButtonPtr_ -> setEnabled(false);
        statusbarPtr_ -> showMessage(QString("Disconnected"));
        connected_ = false;
    }
    
    void CameraWindow::startImageCapture() 
    {
        if (!connected_)
        {
            // TO DO .. add error message 
            return;
        }

        imageGrabber_ = new ImageGrabber(cameraPtr_, newImageQueuePtr_);
        imageDispatcher_ = new ImageDispatcher(newImageQueuePtr_);
        threadPoolPtr_ -> start(imageGrabber_);
        threadPoolPtr_ -> start(imageDispatcher_);
        imageDisplayTimerPtr_ -> start(imageDisplayDt_);
        startButtonPtr_ -> setText(QString("Stop"));
        connectButtonPtr_ -> setEnabled(false);
        statusbarPtr_ -> showMessage(QString("Capturing"));
        capturing_ = true;
    }

    void CameraWindow::stopImageCapture()
    {
        if (!connected_)
        {
            // TO DO .. add error message 
            return;
        }

        // Note, image grabber and dispatcher are destroyed by the 
        // threadPool when their run methods exit.
        imageDisplayTimerPtr_ -> stop();

        imageGrabber_ -> acquireLock();
        imageGrabber_ -> stop();
        imageGrabber_ -> releaseLock();

        imageDispatcher_ -> acquireLock();
        imageDispatcher_ -> stop();
        imageDispatcher_ -> releaseLock();

        newImageQueuePtr_ -> clear();
        startButtonPtr_ -> setText(QString("Start"));
        connectButtonPtr_ -> setEnabled(true);
        statusbarPtr_ -> showMessage(QString("Connected, Stopped"));
        capturing_ = false;
    }

} // namespace bias

       

