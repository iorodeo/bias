#include "signal_slot_demo_plugin.hpp"
#include "image_label.hpp"
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

    void SignalSlotDemoPlugin::reset()
    {
    }


    void SignalSlotDemoPlugin::stop()
    {
    }


    void SignalSlotDemoPlugin::processFrames(QList<StampedImage> frameList)
    {
        // --------------------------------------------------------------
        // NOTE: called in separate thread.
        // --------------------------------------------------------------
        
        acquireLock();
        StampedImage latestFrame = frameList.back();
        frameList.clear();
        currentImage_ = latestFrame.image;
        timeStamp_ = latestFrame.timeStamp;
        frameCount_ = latestFrame.frameCount;
        releaseLock();

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


    void SignalSlotDemoPlugin::initialize()
    {
    }


    // Private Slots
    // ------------------------------------------------------------------------
    


}
