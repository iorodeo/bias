#ifndef STAMPEDE_PLUGIN_HPP
#define STAMPEDE_PLUGIN_HPP
#include "ui_stampede_plugin.h"
#include "bias_plugin.hpp"
#include "stampede_plugin_config.hpp"
#include "panels_controller.hpp"
#include "nano_ssr_pulse.hpp"
#include "rtn_status.hpp"
#include <QDir>

namespace cv
{ 
    class Mat;
}

namespace bias
{



    class StampedePlugin : public BiasPlugin, public Ui::StampedePluginDialog
    {
        Q_OBJECT

        enum EventState {WAITING, RUNNING, COMPLETE};

        public:

            static const QString PLUGIN_NAME;
            static const QString PLUGIN_DISPLAY_NAME;
            static const QString DEFAULT_CONFIG_FILENAME;
            static const QString CONFIG_FILE_EXTENSION;
            static const QString LOG_FILE_EXTENSION;
            static const QString LOG_FILE_POSTFIX;

            StampedePlugin(QWidget *parent=0);
            virtual void reset();
            virtual void setActive(bool value);
            virtual void processFrames(QList<StampedImage> frameList);
            virtual cv::Mat getCurrentImage();
            virtual QString getName();
            virtual QString getDisplayName();
            virtual RtnStatus runCmdFromMap(QVariantMap cmdMap,bool showErrorDlg=true);

            void processEvents();
            RtnStatus loadConfigFromFile(QString fileName, bool showErrorDlg=true);

            RtnStatus connectVibrationDev();
            RtnStatus disconnectVibrationDev();

            RtnStatus connectDisplayDev();
            RtnStatus disconnectDisplayDev();
            RtnStatus connectAll();
            RtnStatus disconnectAll();

        protected:

            StampedePluginConfig config_;

            QDir configFileDir_;
            QDir defaultConfigFileDir_;
            QString configFileName_;

            QDir logFileDir_;
            QString logFileName_;
            QString logFileFullPath_;

            PanelsController displayDev_;
            NanoSSRPulse vibrationDev_;

            QList<EventState> vibrationEventStateList_;
            QList<EventState> displayEventStateList_;

            void initialize();
            void connectWidgets();
            void updateWidgetsEnabled();
            void updateConnectionStatusLabels();
            void updateConnectPushButtonText();
            void updateLogFileName();
            void resetEventStates();

            void updateConfigEditText();
            QString getConfigFileFullPath();
            
            bool checkForSerialPort(QString port);

        private slots:
            void onVibrationDevConnectClicked();
            void onVibrationDevTestClicked();
            void onDisplayDevConnectClicked();
            void onDisplayDevBlinkClicked();
            void onDevConnectAllClicked();
            void onLoadConfigClicked();
            void onTimerDurationChanged(unsigned long duration);
            void onVideoFileChanged();

    };

}


#endif
