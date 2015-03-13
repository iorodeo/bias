#ifndef BIAS_PLUGIN_HPP
#define BIAS_PLUGIN_HPP
#include <QDialog>
#include <QWidget>
#include <QList>
#include "lockable.hpp"
#include "stamped_image.hpp"
//#include "camera_facade_fwd.hpp"

namespace cv
{
    class Mat;
}

namespace bias
{
    class BiasPlugin : public QDialog, public Lockable<Empty>
    {
        Q_OBJECT

        public:

            BiasPlugin(QWidget *parent=0);
            static bool pluginsEnabled();
            static void setPluginsEnabled(bool value);

            void setActive(bool value);
            bool isActive();
            //void setImageOrientation(bool flipVert, bool flipHorz, ImageRotationType imageRot);

            virtual void processFrames(QList<StampedImage> frameList);
            virtual cv::Mat getCurrentImage();

        protected:

            static bool pluginsEnabled_;
            bool active_;

            cv::Mat currentImage_;
            //bool flipVert_;
            //bool flipHorz_;
            //ImageRotationType imageRot_;


        //private slots:

        //    void imageOrientationChanged(bool flipVert, bool flipHorz, ImageRotationType iamgeRot);
            
    };

}


#endif 


