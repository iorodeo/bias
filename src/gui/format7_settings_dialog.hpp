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
                    QWidget *parent=0
                    );
            bool isRoiOffChecked();
            bool isRoiShowChecked();
            bool isRoiEnableChecked();
            int getRoiXOffset();
            int getRoiYOffset();
            int getRoiXWidth();
            int getRoiYHeight();

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

        private:

            std::shared_ptr<Lockable<Camera>> cameraPtr_;
            Format7Settings settings_;
            Format7Info info_;
            bool changed_;

            void initialize();
            void connectWidgets();
            void setRanges();

            void adjustOnXOffsetChange();
            void adjustOnYOffsetChange();
            void adjustOnXWidthChange();
            void adjustOnYHeightChange();


    }; // class Format7SettingsDialog

} // namespace bias


#endif // #ifndef BIAS_FORMAT7_SETTINGS_DIALOG_HPP
