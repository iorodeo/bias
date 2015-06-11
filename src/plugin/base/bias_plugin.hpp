#ifndef BIAS_PLUGIN_HPP
#define BIAS_PLUGIN_HPP
#include <QDialog>
#include <QWidget>
#include <QList>
#include "lockable.hpp"
#include "stamped_image.hpp"
#include "rtn_status.hpp"

namespace cv
{
    class Mat;
}

namespace bias
{

    class CameraWindow;

    class BiasPlugin : public QDialog, public Lockable<Empty>
    {
        Q_OBJECT

        public:

            static const QString PLUGIN_NAME;
            static const QString PLUGIN_DISPLAY_NAME;

            BiasPlugin(QWidget *parent=0);
            bool pluginsEnabled();
            void setPluginsEnabled(bool value);

            bool requireTimer();
            bool isActive();
            QPointer<CameraWindow> getCameraWindow();

            virtual void reset();
            virtual void stop();
            virtual void setActive(bool value);
            virtual void processFrames(QList<StampedImage> frameList);
            virtual void setFileAutoNamingString(QString autoNamingString);
            virtual void setFileVersionNumber(unsigned verNum);
            virtual cv::Mat getCurrentImage();
            virtual QString getName();
            virtual QString getDisplayName();
            virtual QVariantMap getConfigAsMap();  
            virtual RtnStatus setConfigFromMap(QVariantMap configMap);
            virtual RtnStatus setConfigFromJson(QByteArray jsonArray);
            virtual RtnStatus runCmdFromMap(QVariantMap cmdMap, bool showErrorDlg=true);

        signals:

            void setCaptureDurationRequest(unsigned long);

        protected:

            bool active_;
            bool requireTimer_;
            cv::Mat currentImage_;

            double timeStamp_;
            unsigned long frameCount_;

            QString fileAutoNamingString_;
            unsigned int fileVersionNumber_;

            void setRequireTimer(bool value);

    };

}


#endif 


