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

        connect(
                parent(),
                SIGNAL(timerDurationChanged(unsigned long)),
                this,
                SLOT(onTimerDurationChanged(unsigned long))
               );


        // Develpement
        // ------------------------------------------------------------------
        StampedePluginConfig config;
        config.setToDefaultConfig();

        qDebug() << config.toString();

        QVariantMap configMap = config.toMap();
        QByteArray configJson = config.toJson();
        qDebug() << configJson;
        

        StampedePluginConfig config2;
        config2.fromJson(configJson);
        qDebug() << config.toJson();
    }

    // Private slots
    // ------------------------------------------------------------------------

    void StampedePlugin::onTimerDurationChanged(unsigned long duration)
    {
        qDebug() << "duration = " << duration;
    }

}
