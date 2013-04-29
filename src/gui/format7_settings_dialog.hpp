#ifndef BIAS_FORMAT7_SETTINGS_DIALOG_HPP
#define BIAS_FORMAT7_SETTINGS_DIALOG_HPP

#include "ui_format7_settings_dialog.h"
#include "camera_facade_fwd.hpp"
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

            std::shared_ptr<Lockable<Camera>> cameraPtr_;

            void initialize();
            void connectWidgets();
            void setupLineEditValidators(
                    Format7Settings settings, 
                    Format7Info info
                    );

    }; // class Format7SettingsDialog

} // namespace bias


#endif // #ifndef BIAS_FORMAT7_SETTINGS_DIALOG_HPP
