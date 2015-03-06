#ifndef BIAS_PLUGIN_HPP
#define BIAS_PLUGIN_HPP
#include <QDialog>
#include <QWidget>
#include "lockable.hpp"
#include "stamped_image.hpp"

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

            virtual void processFrame(StampedImage frame);
            virtual cv::Mat getCurrentImage();


        protected:

            static bool pluginsEnabled_;
            bool active_;

            cv::Mat currentImage_;
            
    };

}


#endif 


