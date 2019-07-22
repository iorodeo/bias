#include "signal_slot_demo_plugin.hpp"
#include "image_label.hpp"
#include "camera_window.hpp"
#include <QtDebug>
#include <QTimer>
#include <QMessageBox>
#include <QThread>
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>
#include <iostream>

namespace bias
{
    // Public static variables 
    // ------------------------------------------------------------------------
    const QString SignalSlotDemoPlugin::PLUGIN_NAME = QString("signalSlotDemo");
    const QString SignalSlotDemoPlugin::PLUGIN_DISPLAY_NAME = QString("Signal Slot Demo");

    // Public Methods
    // ------------------------------------------------------------------------
    
    SignalSlotDemoPlugin::SignalSlotDemoPlugin(ImageLabel *imageLabelPtr, QWidget *parentPtr) : BiasPlugin(parentPtr)
    {
        imageLabelPtr_ = imageLabelPtr;
        setupUi(this);
        connectWidgets();
        initialize();
    }

    void SignalSlotDemoPlugin::finalSetup()
    {
        QPointer<CameraWindow> partnerCameraWindowPtr = getPartnerCameraWindowPtr();
        if (partnerCameraWindowPtr)
        {
            QPointer<BiasPlugin> partnerPluginPtr = partnerCameraWindowPtr -> getPluginByName("signalSlotDemo");
            qRegisterMetaType<FrameData>("FrameData");
            connect(partnerPluginPtr, SIGNAL(newFrameData(FrameData)), this, SLOT(onNewFrameData(FrameData)));
        }
        updateMessageLabels();
    }

    void SignalSlotDemoPlugin::reset()
    {
    }


    void SignalSlotDemoPlugin::stop()
    {
    }


    void SignalSlotDemoPlugin::processFrames(QList<StampedImage> frameList)
    {
        // -------------------------------------------------------------------
        // NOTE: called in separate thread. Use lock to access data shared 
        // with other class methods. 
        // -------------------------------------------------------------------
        
        acquireLock();
        StampedImage latestFrame = frameList.back();
        frameList.clear();
        currentImage_ = latestFrame.image;
        timeStamp_ = latestFrame.timeStamp;
        frameCount_ = latestFrame.frameCount;
        releaseLock();

        FrameData frameData;
        frameData.count = frameCount_;
        frameData.image = currentImage_;
        emit newFrameData(frameData);
        numMessageSent_++;

        updateMessageLabels();

    }

    cv::Mat SignalSlotDemoPlugin::getCurrentImage()
    {
        acquireLock();
        cv::Mat currentImageCopy = currentImage_.clone();
        releaseLock();
        return currentImageCopy;
    }


    QString SignalSlotDemoPlugin::getName()
    {
        return PLUGIN_NAME;
    }


    QString SignalSlotDemoPlugin::getDisplayName()
    {
        return PLUGIN_DISPLAY_NAME;
    }


    // Protected Methods
    // ------------------------------------------------------------------------
    
    void SignalSlotDemoPlugin::connectWidgets()
    {

    }

    unsigned int SignalSlotDemoPlugin::getPartnerCameraNumber()
    {
        // Returns camera number of partner camera. For this example
        // we just use camera 0 and 1. In another setting you might do
        // this by GUID or something else.
        if (cameraNumber_ == 0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    QPointer<CameraWindow> SignalSlotDemoPlugin::getPartnerCameraWindowPtr()
    {
        QPointer<CameraWindow> partnerCameraWindowPtr = nullptr;
        if ((cameraWindowPtrList_ -> size()) > 1)
        {
            for (auto cameraWindowPtr : *cameraWindowPtrList_)
            {
                if ((cameraWindowPtr -> getCameraNumber()) == partnerCameraNumber_)
                {
                    partnerCameraWindowPtr = cameraWindowPtr;
                }
            }
        }
        return partnerCameraWindowPtr;
    }


    void SignalSlotDemoPlugin::initialize()
    {

        QPointer<CameraWindow> cameraWindowPtr = getCameraWindow();
        cameraNumber_ = cameraWindowPtr -> getCameraNumber();
        cameraGuidString_ = cameraWindowPtr ->  getCameraGuidString();
        cameraWindowPtrList_ = cameraWindowPtr -> getCameraWindowPtrList();

        QString labelStr = QString("Camera #: %0,     GUID: %2").arg(cameraNumber_).arg(cameraGuidString_);
        cameraNumberLabelPtr -> setText(labelStr);

        partnerCameraNumber_ = getPartnerCameraNumber();
        connectedToPartner_ = false;

        numMessageSent_ = 0;
        numMessageReceived_ = 0;

    }


    void SignalSlotDemoPlugin::updateMessageLabels()
    {
        QString sentLabelText = QString("messages sent: %1").arg(numMessageSent_);
        messageSentLabelPtr -> setText(sentLabelText);

        QString recvLabelText = QString("messages recv: %1").arg(numMessageReceived_);
        messageReceivedLabelPtr -> setText(recvLabelText);
    }


    // Private Slots
    // ------------------------------------------------------------------------
    
    void SignalSlotDemoPlugin::onNewFrameData(FrameData data)
    {
        //std::cout << "cam " << cameraNumber_ << " received data from cam " << partnerCameraNumber_ << std::endl;
        numMessageReceived_++;
        updateMessageLabels();

    }


}
