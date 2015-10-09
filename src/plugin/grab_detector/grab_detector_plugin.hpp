#ifndef GRAB_DETECTOR_PLUGIN_HPP
#define GRAB_DETECTOR_PLUGIN_HPP
#include "ui_grab_detector_plugin.h"
#include "grab_detector_config.hpp"
#include "bias_plugin.hpp"
#include "pulse_device.hpp"
#include <QPointer>
#include <QVector>
#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>

class QTimer;

namespace cv
{
    class Mat;
}


namespace bias
{

    class ImageLabel;
    class CameraWindow;

    struct TriggerData
    {
        unsigned long frameCount;
        double timeStamp;
        double threshold;
        double signal;
    };


    class GrabDetectorPlugin : public BiasPlugin, public Ui::GrabDetectorPluginDialog
    {
        Q_OBJECT

        public:

            static const QString PLUGIN_NAME;
            static const QString PLUGIN_DISPLAY_NAME;
            static int DEFAULT_LIVEPLOT_UPDATE_DT;
            static double DEFAULT_LIVEPLOT_TIME_WINDOW; 
            static double DEFAULT_LIVEPLOT_SIGNAL_WINDOW;
            static const QString LOG_FILE_EXTENSION;
            static const QString LOG_FILE_POSTFIX;

            GrabDetectorPlugin(ImageLabel *imageLabelPtr, QWidget *parentPtr=0);
            virtual void reset();
            virtual void stop();

            virtual void processFrames(QList<StampedImage> frameList);
            virtual cv::Mat getCurrentImage();

            virtual QString getName();
            virtual QString getDisplayName();
            
            virtual RtnStatus runCmdFromMap(QVariantMap cmdMap, bool showErrorDlg=true);
            virtual QVariantMap getConfigAsMap();  
            virtual RtnStatus setConfigFromMap(QVariantMap configMap);
            virtual RtnStatus setConfigFromJson(QByteArray jsonArray);
            virtual QString getLogFileExtension();
            virtual QString getLogFilePostfix();

            void setTriggerEnabled(bool value);
            void resetTrigger();

            RtnStatus connectTriggerDev();
            RtnStatus disconnectTriggerDev();
            RtnStatus setFromConfig(GrabDetectorConfig config);


        signals:

            void triggerFired(TriggerData data); 

        protected:

            bool found_;
            double signalMax_;
            double signalMin_;
            unsigned long frameCount_;
            
            GrabDetectorConfig config_;

            int livePlotUpdateDt_;
            double livePlotTimeWindow_; 
            double livePlotSignalWindow_;
            QVector<double> livePlotTimeVec_;
            QVector<double> livePlotSignalVec_;
            QPointer<QTimer> livePlotUpdateTimerPtr_;
            QPointer<ImageLabel> imageLabelPtr_;

            QList<QSerialPortInfo> serialInfoList_;
            PulseDevice pulseDevice_;
            QVector<int> allowedOutputPin_;
            bool outputPinComboBoxReady_ = false;

            void connectWidgets();
            void initialize();

            cv::Rect getDetectionBoxCv();
            QRect getDetectionBox();
            void setDetectionBox(QRect box);
            bool isDetectionBoxLocked();

            int getThreshold();
            int getMedianFilter();
            bool getInverted();

            void updateTrigStateInfo();
            void refreshPortList();

            void writeLogData(TriggerData data);

            void updateColorExampleLabel();


        private slots:

            void comPortComboBoxIndexChanged(QString text);
            void connectPushButtonClicked();
            void refreshPortListPushButtonClicked();
            void outputTestPushButtonClicked();
            void outputPinComboBoxIndexChanged(int index);
            void durationDblSpinBoxValueChanged(double value);
            void colorSelectPushButtonClicked();
            void trigResetPushButtonClicked();
            void trigEnabledCheckBoxStateChanged(int state);
            void trigThresholdSpinBoxChanged(int value);
            void trigMedianFilterSpinBoxChanged(int value);
            void detectionBoxChanged(QRect boxRect);
            void updateLivePlotOnTimer();
            void onTriggerFired(TriggerData data);

    };
}
#endif
