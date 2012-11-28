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
        // Get information from image dispatcher
        imageDispatcherPtr_ -> acquireLock();
        cv::Mat mat = imageDispatcherPtr_ -> getImage();
        double fps = imageDispatcherPtr_ -> getFPS();
        QImage img = matToQImage(mat);
        imageDispatcherPtr_ -> releaseLock();

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

    void CameraWindow::startImageCaptureError(unsigned int errorId, QString errorMsg)
    {
        stopImageCapture();
        QString msgTitle("Start Image Capture Error");
        QString msgText("Failed to start image capture\n\nError ID: ");
        msgText += QString::number(errorId);
        msgText += "\n\n";
        msgText += errorMsg;
        QMessageBox::critical(this, msgTitle, msgText);
    }

    void CameraWindow::stopImageCaptureError(unsigned int errorId, QString errorMsg)
    {
        QString msgTitle("Stop Image Capture Error");
        QString msgText("Failed to stop image capture\n\nError ID: ");
        msgText += QString::number(errorId);
        msgText += "\n\n";
        msgText += errorMsg;
        QMessageBox::critical(this, msgTitle, msgText);
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

        setCameraInfoMessage(QString("____"), QString("____"));

        startButtonPtr_ -> setText(QString("Start"));
        startButtonPtr_ -> setEnabled(false);
        connectButtonPtr_ -> setEnabled(true);
        statusbarPtr_ -> showMessage(QString("Camera found, disconnected"));
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
        bool error = false;
        unsigned int errorId;
        QString errorMsg;

        cameraPtr_ -> acquireLock();
        try
        {
            cameraPtr_ -> connect();
        }
        catch (RuntimeError &runtimeError)
        {
            error = true;
            errorId = runtimeError.id();
            errorMsg = QString::fromStdString(runtimeError.what());
        }
        cameraPtr_ -> releaseLock();

        if (!error) 
        {
            connectButtonPtr_ -> setText(QString("Disconnect"));
            startButtonPtr_ -> setEnabled(true);
            statusbarPtr_ -> showMessage(QString("Connected, Stopped"));

            setCameraInfoMessage(
                    QString::fromStdString(cameraPtr_ -> getVendorName()),
                    QString::fromStdString(cameraPtr_ -> getModelName())
                    );

            connected_ = true;
        }
        else 
        {
            QString msgTitle("Connection Error");
            QString msgText("Failed to connect camera:\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
        }
    }

    void CameraWindow::disconnectCamera()
    {
        bool error = false;
        unsigned int errorId;
        QString errorMsg; 

        if (capturing_) 
        {
            stopImageCapture();
        }

        cameraPtr_ -> acquireLock();
        try
        {
            cameraPtr_ -> disconnect();
        }
        catch (RuntimeError &runtimeError)
        {
            error = true;
            errorId = runtimeError.id();
            errorMsg = QString::fromStdString(runtimeError.what());
        }
        cameraPtr_ -> releaseLock();

        if (!error) 
        {
            connectButtonPtr_ -> setText(QString("Connect"));
            startButtonPtr_ -> setEnabled(false);
            statusbarPtr_ -> showMessage(QString("Disconnected"));
            connected_ = false;
        }
        else 
        {
            QString msgTitle("Disconnection Error");
            QString msgText("Failed to disconnect camera:\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
        }
    }
    
    void CameraWindow::startImageCapture() 
    {
        if (!connected_)
        {
            QString msgTitle("Capture Error");
            QString msgText("Unable to start image capture: not connected");
            QMessageBox::critical(this, msgTitle, msgText);
        }

        imageGrabberPtr_ = new ImageGrabber(cameraPtr_, newImageQueuePtr_);
        imageDispatcherPtr_ = new ImageDispatcher(newImageQueuePtr_);

        connect(
                imageGrabberPtr_, 
                SIGNAL(startCaptureError(unsigned int, QString)),
                this,
                SLOT(startImageCaptureError(unsigned int, QString))
               );

        connect(
                imageGrabberPtr_,
                SIGNAL(stopCaptureError(unsigned int, QString)),
                this,
                SLOT(stopImageCaptureError(unsigned int, QString))
               );

        threadPoolPtr_ -> start(imageGrabberPtr_);
        threadPoolPtr_ -> start(imageDispatcherPtr_);

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
            QString msgTitle("Capture Error");
            QString msgText("Unable to stop image capture: not connected");
            QMessageBox::critical(this, msgTitle, msgText);
        }

        // Note, image grabber and dispatcher are destroyed by the 
        // threadPool when their run methods exit.
        imageDisplayTimerPtr_ -> stop();

        if (!imageGrabberPtr_.isNull())
        {
            imageGrabberPtr_ -> acquireLock();
            imageGrabberPtr_ -> stop();
            imageGrabberPtr_ -> releaseLock();
        }

        if (!imageDispatcherPtr_.isNull())
        {
            imageDispatcherPtr_ -> acquireLock();
            imageDispatcherPtr_ -> stop();
            imageDispatcherPtr_ -> releaseLock();
        }

        newImageQueuePtr_ -> clear();
        startButtonPtr_ -> setText(QString("Start"));
        connectButtonPtr_ -> setEnabled(true);
        statusbarPtr_ -> showMessage(QString("Connected, Stopped"));
        capturing_ = false;
    }
    
    void CameraWindow::setCameraInfoMessage(QString vendorName, QString modelName)
    {
        QString cameraInfoString("Vendor = ");
        cameraInfoString += vendorName;
        cameraInfoString += QString(",  Model = ");
        cameraInfoString += modelName; 
        cameraInfoLabelPtr_ -> setText(cameraInfoString);
    }

} // namespace bias

       

