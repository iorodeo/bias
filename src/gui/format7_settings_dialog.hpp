#ifndef BIAS_FORMAT7_SETTINGS_DIALOG_HPP
#define BIAS_FORMAT7_SETTINGS_DIALOG_HPP

#include "ui_format7_settings_dialog.h"
#include "camera_facade.hpp"
#include <QDialog>
#include <memory>

namespace bias
{
    template <class T> class Lockable;

    class Format7SettingsDialog : public QDialog, public Ui::Format7SettingsDialog
    {
        Q_OBJECT

        public:

            Format7SettingsDialog(QWidget *parent=0);
            Format7SettingsDialog( 
                    std::shared_ptr<Lockable<Camera>> cameraPtr,
                    bool capturing,
                    bool logging,
                    QWidget *parent=0
                    );
            bool isRoiOffChecked();
            bool isRoiShowChecked();
            bool isRoiEnableChecked();
            int getRoiXOffset();
            int getRoiYOffset();
            int getRoiXWidth();
            int getRoiYHeight();
            int getRoiXWidthMax();
            int getRoiYHeightMax();

        signals:

            void roiEnableStateChanged();

        private slots:

            void modeComboBoxChanged(int index);
            void pixelFormatComboBoxChanged(int index);
            void roiXOffsetSliderChanged(int value);
            void roiYOffsetSliderChanged(int value);
            void roiXWidthSliderChanged(int value);
            void roiYHeightSliderChanged(int value);
            void roiXOffsetEditingFinished();
            void roiYOffsetEditingFinished();
            void roiXWidthEditingFinished();
            void roiYHeightEditingFinished();
            void roiOffRadioButtonChanged(bool checked);
            void roiShowRadioButtonChanged(bool checked);
            void roiEnableRadioButtonChanged(bool checked);
            void roiMaxSizePushButtonClicked();
            void imageCaptureStarted(bool logging);
            void imageCaptureStopped();
            void format7SettingsChanged();

        private:

            std::shared_ptr<Lockable<Camera>> cameraPtr_;
            Format7Settings settings_;
            Format7Info info_;
            ImageModeList modeList_;
            PixelFormatList formatList_;
            unsigned int lastRoiEnableState_;
            QMap<ImageMode, int> modeToIndexMap_;
            QMap<PixelFormat, int> formatToIndexMap_;

            void initialize(bool capturing, bool logging);
            void updateWidgets(bool capturing, bool logging);
            void getFormat7SettingsAndInfo();
            void updateSlidersAndLineEdits();
            void updateRoiRadioButtons();
            void connectWidgets();
            void setModeComboBoxIndex();
            void setPixelFormatComboBoxIndex();
            void setRanges();
            void setRoiControlsEnabled(bool value);
            void setFormat7Settings(Format7Settings settings, float percentSpeed=100.0);
            void updateFormat7SettingsFullSize();
            void updateFormat7Settings();
            void adjustOnXOffsetChange();
            void adjustOnYOffsetChange();
            void adjustOnXWidthChange();
            void adjustOnYHeightChange();
            void cameraLockFailErrMsg(QString msg);
            bool isRoiMaxSize();


    }; // class Format7SettingsDialog

} // namespace bias


#endif // #ifndef BIAS_FORMAT7_SETTINGS_DIALOG_HPP
