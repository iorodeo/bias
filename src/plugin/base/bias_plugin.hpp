#ifndef BIAS_PLUGIN_HPP
#define BIAS_PLUGIN_HPP
#include <QDialog>
#include <QWidget>
#include <QList>
#include "lockable.hpp"
#include "stamped_image.hpp"
#include "rtn_status.hpp"
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

            static const QString PLUGIN_NAME;
            static const QString PLUGIN_DISPLAY_NAME;

            BiasPlugin(QWidget *parent=0);
            static bool pluginsEnabled();
            static void setPluginsEnabled(bool value);

            void setActive(bool value);
            bool isActive();

            virtual void processFrames(QList<StampedImage> frameList);
            virtual cv::Mat getCurrentImage();

            virtual QString getName();
            virtual QString getDisplayName();

            virtual RtnStatus runCmdFromMap(QVariantMap cmdMap, bool showErrorDlg=true);


        protected:


            static bool pluginsEnabled_;
            bool active_;

            cv::Mat currentImage_;
            
    };

}


#endif 


