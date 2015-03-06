#include "bias_plugin.hpp"
#include <QtDebug>
#include <opencv2/core/core.hpp>

namespace bias
{

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

    void BiasPlugin::processFrame(StampedImage frame) 
    { 
        acquireLock();
        currentImage_ = frame.image;
        releaseLock();
    } 


    cv::Mat BiasPlugin::getCurrentImage()
    {
        acquireLock();
        cv::Mat currentImageCopy = currentImage_.clone();
        releaseLock();
        return currentImageCopy;
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
