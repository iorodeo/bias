#ifndef BIAS_FORMAT7_SETTINGS_DIALOG_HPP
#define BIAS_FORMAT7_SETTINGS_DIALOG_HPP

#include <QDialog>
#include "ui_format7_settings_dialog.h"

namespace bias
{

    class Format7SettingsDialog : public QDialog, public Ui::Format7SettingsDialog
    {
        Q_OBJECT

        public:

            Format7SettingsDialog(QWidget *parent=0);

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

        private:
            void initialize();
            void connectWidgets();
            void setupLineEditValidators();

    }; // class Format7SettingsDialog

} // namespace bias


#endif // #ifndef BIAS_FORMAT7_SETTINGS_DIALOG_HPP
