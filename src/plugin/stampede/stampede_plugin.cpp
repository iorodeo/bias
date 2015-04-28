#include "stampede_plugin.hpp"
#include <opencv2/core/core.hpp>
#include <QtDebug>

namespace bias
{

    const QString StampedePlugin::PLUGIN_NAME = QString("stampede");
    const QString StampedePlugin::PLUGIN_DISPLAY_NAME = QString("Stampede");

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

    QString StampedePlugin::getName()
    {
        return PLUGIN_NAME;
    }


    QString StampedePlugin::getDisplayName()
    {
        return PLUGIN_DISPLAY_NAME;
    }

    RtnStatus StampedePlugin::runCmdFromMap(QVariantMap cmdMap,bool showErrorDlg)
    {
        qDebug() << __PRETTY_FUNCTION__;
        RtnStatus rtnStatus;
        return rtnStatus;
    }


    // Protected Methods
    // ------------------------------------------------------------------------
    void StampedePlugin::initialize()
    {
        setRequireTimer(true);
        vibrationEventWidgetPtr -> setNameText(QString("Vibration"));
        displayEventWidgetPtr -> setNameText(QString("Display"));

        connect(
                parent(),
                SIGNAL(timerDurationChanged(unsigned long)),
                this,
                SLOT(onTimerDurationChanged(unsigned long))
               );

        connect(
                parent(),
                SIGNAL(timerDurationChanged(unsigned long)),
                vibrationEventWidgetPtr,
                SLOT(onTimerDurationChanged(unsigned long))
               );

        connect(
                parent(),
                SIGNAL(timerDurationChanged(unsigned long)),
                displayEventWidgetPtr,
                SLOT(onTimerDurationChanged(unsigned long))
               );


    }

    // Private slots
    // ------------------------------------------------------------------------

    void StampedePlugin::onTimerDurationChanged(unsigned long duration)
    {
        QString stopTimeText = QString("Stop (t=%1 sec)").arg(duration);
        stopTimeLabelPtr -> setText(stopTimeText);
    }




}
