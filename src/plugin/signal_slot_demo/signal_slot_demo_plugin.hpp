#ifndef SIGNAL_SLOT_DEMO_HPP
#define SIGNAL_SLOT_DEMO_HPP
#include "ui_signal_slot_demo_plugin.h"
#include "bias_plugin.hpp"
#include "frame_data.hpp"
#include <QPointer>
#include <QVector>
#include <QList>


namespace cv
{
    class Mat;
}


namespace bias
{

    class ImageLabel;
    class CameraWindow;


    class SignalSlotDemoPlugin : public BiasPlugin, public Ui::SignalSlotDemoPluginDialog
    {
        Q_OBJECT

        public:

            static const QString PLUGIN_NAME;
            static const QString PLUGIN_DISPLAY_NAME;

            SignalSlotDemoPlugin(ImageLabel *imageLabelPtr, QWidget *parentPtr=0);

            virtual void finalSetup();
            virtual void reset();
            virtual void stop();

            virtual void processFrames(QList<StampedImage> frameList);
            virtual cv::Mat getCurrentImage();

            virtual QString getName();
            virtual QString getDisplayName();

        signals:

            void newFrameData(FrameData data); 

        protected:

            unsigned int cameraNumber_;
            unsigned int partnerCameraNumber_ ;
            QString cameraGuidString_;
            bool connectedToPartner_;

            unsigned long numMessageSent_;
            unsigned long numMessageReceived_;


            QPointer<ImageLabel> imageLabelPtr_;
            QSharedPointer<QList<QPointer<CameraWindow>>> cameraWindowPtrList_;;
            
            void connectWidgets();
            void initialize();
            
            unsigned int getPartnerCameraNumber();
            QPointer<CameraWindow> getPartnerCameraWindowPtr();

            void updateMessageLabels();

        private slots:

            void onNewFrameData(FrameData data);


    };
}
#endif
