#ifndef BIAS_LOGGING_SETTINGS_DIALOG_HPP
#define BIAS_LOGGING_SETTINGS_DIALOG_HPP

#include <QDialog>
#include "ui_logging_settings_dialog.h"
#include "video_writer_params.hpp"

namespace bias
{

    class LoggingSettingsDialog : public QDialog, public Ui::LoggingSettingsDialog
    {
        Q_OBJECT

        public:

            LoggingSettingsDialog(QWidget *parent=0);
            LoggingSettingsDialog(VideoWriterParams params, QWidget *parent=0);

        signals:

            void parametersChanged(VideoWriterParams params);


        private slots:

            void bmpFrameSkip_EditingFinished();
            void jpgFrameSkip_EditingFinished();
            void jpgQuality_EditingFinished();
            void jpgCompressionThreads_EditingFinished();
            void aviFrameSkip_EditingFinished();
            void aviCodecComboBox_CurrentIndexChanged(QString text);
            void fmfFrameSkip_EditingFinished();
            void ufmfFrameSkip_EditingFinished();
            void ufmfBackgroundThreshold_EditingFinished();
            void ufmfBoxLength_EditingFinished();
            void ufmfMedianUpdateCount_EditingFinished();
            void ufmfMedianUpdateInterval_EditingFinished();
            void ufmfCompressionThreads_EditingFinished();
            void ufmfDilateCheckBox_StateChanged(int state);
            void ufmfDilate_EditingFinished();
            void imageCaptureStarted(bool logging);
            void imageCaptureStopped();

        private:

            VideoWriterParams params_;

            void initialize(VideoWriterParams params);
            void setInitialValues();
            void setValidators();
            void connectWidgets();

    };


} // namespace bias

#endif // #ifndef BIAS_LOGGING_SETTINGS_DIALOG_HPP:W

