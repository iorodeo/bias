#include "bias_plugin.hpp"
//#include "camera_facade.hpp"
#include <QtDebug>
#include <opencv2/core/core.hpp>

namespace bias
{

    const QString BiasPlugin::PLUGIN_NAME = QString("basePlugin"); 
    const QString BiasPlugin::PLUGIN_DISPLAY_NAME = QString("Base Plugin"); 

    // Pulbic
    // ------------------------------------------------------------------------

    BiasPlugin::BiasPlugin(QWidget *parent) : QDialog(parent) 
    { 
        active_ = false;
    }


    void BiasPlugin::setActive(bool value)
    {
        active_ = value;
    }


    bool BiasPlugin::isActive()
    {
        return active_;
    }

    void BiasPlugin::processFrames(QList<StampedImage> frameList) 
    { 
        acquireLock();
        StampedImage latestFrame = frameList.back();
        frameList.clear();
        currentImage_ = latestFrame.image;
        releaseLock();
    } 


    cv::Mat BiasPlugin::getCurrentImage()
    {
        acquireLock();
        cv::Mat currentImageCopy = currentImage_.clone();
        releaseLock();
        return currentImageCopy;
    }

    QString BiasPlugin::getName()
    {
        return PLUGIN_NAME;
    }

    QString BiasPlugin::getDisplayName()
    {
        return PLUGIN_DISPLAY_NAME;
    }

    RtnStatus BiasPlugin::runCmdFromMap(QVariantMap cmdMap, bool showErrorDlg)
    {
        qDebug() << __PRETTY_FUNCTION__;
        RtnStatus rtnStatus;
        return rtnStatus;
    }

    // Protected
    // ------------------------------------------------------------------------

    bool BiasPlugin::pluginsEnabled_ = false;


    bool BiasPlugin::pluginsEnabled()
    {
        return pluginsEnabled_;
    }


    void BiasPlugin::setPluginsEnabled(bool value)
    {
        pluginsEnabled_ = value;
    }

}
