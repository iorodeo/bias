#ifndef BIAS_TIMER_SETTINGS_DIALOG_HPP
#define BIAS_TIMER_SETTINGS_DIALOG_HPP

#include <QDialog>
#include "ui_timer_settings_dialog.h"

namespace bias
{

    class TimerSettingsDialog : public QDialog, public Ui::TimerSettingsDialog
    {
        Q_OBJECT

        public:

            TimerSettingsDialog(unsigned long duration=300, QWidget *parent=0);

        signals:

            void durationChanged(unsigned long duration);

        private slots:

            void spinBoxValueChanged();
            void imageCaptureStarted(bool logging);
            void imageCaptureStopped();
            void onTimerDurationChanged(unsigned long duration);

        private:

            void connectWidgets();
            void initialize(unsigned long duration);
            void setSpinBoxValues(unsigned long duration);

    };

} // namespace bias

#endif // #ifndef BIAS_TIMER_SETTINGS_DIALOG_HPP
