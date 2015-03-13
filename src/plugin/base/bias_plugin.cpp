#include "bias_plugin.hpp"
//#include "camera_facade.hpp"
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

    //void BiasPlugin::setImageOrientation(bool flipVert, bool flipHorz, ImageRotationType imageRot)
    //{
    //    flipVert_ = flipVert;
    //    flipHorz_ = flipHorz;
    //    imageRot_ = imageRot;
    //}

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


    // Private slots
    // ------------------------------------------------------------------------

    //void BiasPlugin::imageOrientationChanged(bool flipVert, bool flipHorz, ImageRotationType imageRot)
    //{
    //    setImageOrientation(flipVert,flipHorz,imageRot);
    //}

}
