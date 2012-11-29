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
        QImage img = matToQImage(mat);
        double fps = imageDispatcherPtr_ -> getFPS();
        frameCount_ = imageDispatcherPtr_ -> getFrameCount();
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

        QString windowTitle("BIAS Camera Window, Guid: ");
        windowTitle += QString::fromStdString(cameraGuid.toString());
        setWindowTitle(windowTitle);

        setCameraInfoMessage(QString("____"), QString("____"));

        startButtonPtr_ -> setText(QString("Start"));
        startButtonPtr_ -> setEnabled(false);
        connectButtonPtr_ -> setEnabled(true);
        statusbarPtr_ -> showMessage(QString("Camera found, disconnected"));

        menuConfigurationPtr_ -> setEnabled(false);

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

        
        // Add framecount message
        QPainter painter(&pixmapScaled);
        QString msg;  
        msg.sprintf("%d",frameCount_);
        painter.setPen(QColor(0,220,0));
        painter.drawText(5,12, msg);

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

        if (error)
        {
            QString msgTitle("Connection Error");
            QString msgText("Failed to connect camera:\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }

        connectButtonPtr_ -> setText(QString("Disconnect"));
        statusbarPtr_ -> showMessage(QString("Connected, Stopped"));
        startButtonPtr_ -> setEnabled(true);
        menuConfigurationPtr_ -> setEnabled(true);

        setCameraInfoMessage(
                QString::fromStdString(cameraPtr_ -> getVendorName()),
                QString::fromStdString(cameraPtr_ -> getModelName())
                );

        connected_ = true;
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

        if (error)
        {
            QString msgTitle("Disconnection Error");
            QString msgText("Failed to disconnect camera:\n\nError ID: ");
            msgText += QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }

        connectButtonPtr_ -> setText(QString("Connect"));
        statusbarPtr_ -> showMessage(QString("Disconnected"));
        startButtonPtr_ -> setEnabled(false);
        menuConfigurationPtr_ -> setEnabled(false);
        connected_ = false;
    }
    
    void CameraWindow::startImageCapture() 
    {
        if (!connected_)
        {
            QString msgTitle("Capture Error");
            QString msgText("Unable to start image capture: not connected");
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }

        setMenuChildrenEnabled(menuLoggingPtr_, false);
        setMenuChildrenEnabled(menuTimerPtr_, false);

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
            return;
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
        setMenuChildrenEnabled(menuLoggingPtr_, true);
        setMenuChildrenEnabled(menuTimerPtr_, true);
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

    void CameraWindow::setMenuChildrenEnabled(QWidget *parentWidgetPtr, bool value)
    {
        QList<QMenu *> childList = parentWidgetPtr -> findChildren<QMenu *>();
        QList<QMenu *>::iterator childIt;
        for (childIt=childList.begin(); childIt!=childList.end(); childIt++)
        {
            QMenu *menuPtr = *childIt;
            menuPtr -> setEnabled(value);
        }

        QList<QAction *> actionList = parentWidgetPtr -> actions();
        QList<QAction *>::iterator actionIt;
        for (actionIt=actionList.begin(); actionIt!=actionList.end(); actionIt++)
        {
            QAction *actionPtr = *actionIt;
            actionPtr -> setEnabled(value);
        }
    }

} // namespace bias

       

