#include "stampede_plugin.hpp"
#include <opencv2/core/core.hpp>
#include <QtDebug>
#include "camera_window.hpp"
#include "json_utils.hpp"

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
        connectWidgets();
    }

    void StampedePlugin::setActive(bool value)
    {
        BiasPlugin::setActive(value);
        QPointer<CameraWindow> cameraWindowPtr = getCameraWindow();
        cameraWindowPtr -> setCaptureDuration(config_.duration());
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
        QFont monoSpaceFont("Monospace");
        monoSpaceFont.setStyleHint(QFont::TypeWriter);
        configTextEditPtr -> setFont(monoSpaceFont);

        setRequireTimer(true);
        config_.setToDefaultConfig();
        updateConfigEditText();
    }

    void StampedePlugin::connectWidgets()
    {
        connect(
                parent(),
                SIGNAL(timerDurationChanged(unsigned long)),
                this,
                SLOT(onTimerDurationChanged(unsigned long))
               );
    }

    void StampedePlugin::updateConfigEditText()
    {
        //QByteArray prettyJson = prettyIndentJson(config_.toJson());
        //configTextEditPtr -> setText(prettyJson);
        configTextEditPtr -> setText(config_.toString());
    }

    // Private slots
    // ------------------------------------------------------------------------

    void StampedePlugin::onTimerDurationChanged(unsigned long duration)
    {
        qDebug() << "duration = " << duration;
    }

}
