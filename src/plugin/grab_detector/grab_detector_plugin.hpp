#ifndef GRAB_DETECTOR_PLUGIN_HPP
#define GRAB_DETECTOR_PLUGIN_HPP

#include "ui_grab_detector_plugin.h"
#include "bias_plugin.hpp"
#include <QPointer>

namespace cv
{
    class Mat;
}

namespace bias
{

    class ImageLabel;


    class GrabDetectorPlugin : public BiasPlugin, public Ui::GrabDetectorPluginDialog
    {
        Q_OBJECT

        public:

            static int DEFAULT_XPOS;
            static int DEFAULT_YPOS;
            static int DEFAULT_WIDTH;
            static int DEFAULT_HEIGHT;
            static int DEFAULT_TRIGGER_ARMED;
            static int DEFAULT_TRIGGER_THRESHOLD;
            static int DEFAULT_TRIGGER_FILTER_SIZE;

            GrabDetectorPlugin(ImageLabel *imageLabelPtr, QWidget *parentPtr=0);
            virtual void processFrame(StampedImage frame);
            virtual cv::Mat getCurrentImage();

        protected:

            bool triggerArmed_;
            int triggerThreshold_;
            int triggerFilterSize_;
            double signalMax_;
            double signalMin_;
            unsigned long frameCount_;
            bool found_;
            QPointer<ImageLabel> imageLabelPtr_;

            void connectWidgets();
            void initialize();

            cv::Rect getDetectionBoxCv();
            QRect getDetectionBox();
            void setDetectionBox(QRect box);
            int getThreshold();
            int getMedianFilter();

        private slots:

            void comPortComboBoxIndexChanged(QString text);
            void connectPushButtonPressed();
            void connectPushButtonClicked();
            void levelDblSpinBoxValueChanged(double value);
            void durationDblSpinBoxValueChanged(double value);
            void colorSelectPushButtonClicked();
            void trigResetPushButtonClicked();
            void detectionBoxChanged(QRect boxRect);

    };
}
#endif
