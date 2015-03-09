#include "stampede_plugin.hpp"
#include <opencv2/core/core.hpp>

namespace bias
{

    // Public Methods
    // ------------------------------------------------------------------------
    StampedePlugin::StampedePlugin(QWidget *parent) : BiasPlugin(parent)
    {
        setupUi(this);
        initialize();
    }

    void StampedePlugin::processFrames(QList<StampedImage> frameList)
    {
        acquireLock();
        StampedImage latestFrame = frameList.back();
        frameList.clear();
        currentImage_ = latestFrame.image;
        releaseLock();
    }

    cv::Mat StampedePlugin::getCurrentImage()
    {
        acquireLock();
        cv::Mat currentImageCopy = currentImage_.clone();
        releaseLock();
        return currentImageCopy;
    }

    // Protected Methods
    // ------------------------------------------------------------------------
    void StampedePlugin::initialize()
    {
        vibrationEventWidgetPtr -> setNameText(QString("Vibration"));
        displayEventWidgetPtr -> setNameText(QString("Display"));

    }

}
