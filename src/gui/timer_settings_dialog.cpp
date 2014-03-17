#include "timer_settings_dialog.hpp"
#include <iostream>

namespace bias
{

    TimerSettingsDialog::TimerSettingsDialog(unsigned long duration, QWidget *parent) 
        : QDialog(parent)

    {
        setupUi(this);
        initialize(duration);
        connectWidgets();
    }


    void TimerSettingsDialog::spinBoxValueChanged()
    {
        unsigned long hour = (unsigned long)(hourSpinBoxPtr -> value());
        unsigned long min  = (unsigned long)(minSpinBoxPtr  -> value());
        unsigned long sec  = (unsigned long)(secSpinBoxPtr  -> value());
        unsigned long newDuration = 3600*hour + 60*min + sec;
        emit durationChanged(newDuration);
    }


    void TimerSettingsDialog::imageCaptureStarted(bool logging)
    {
        setEnabled(false);
    }


    void TimerSettingsDialog::imageCaptureStopped()
    {
        setEnabled(true);
    }
        

    void TimerSettingsDialog::connectWidgets()
    {
        connect(
                hourSpinBoxPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(spinBoxValueChanged())
               );

        connect(
                minSpinBoxPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(spinBoxValueChanged())
               );

        connect(
                secSpinBoxPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(spinBoxValueChanged())
               );

        connect(
                parent(),
                SIGNAL(imageCaptureStarted(bool)),
                this,
                SLOT(imageCaptureStarted(bool))
               );

        connect(
                parent(),
                SIGNAL(imageCaptureStopped()),
                this,
                SLOT(imageCaptureStopped())
               );
    }


    void TimerSettingsDialog::initialize(unsigned long duration)
    {

        setAttribute(Qt::WA_DeleteOnClose);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

        // Set spinbox limits
        hourSpinBoxPtr -> setMaximum(99);
        hourSpinBoxPtr -> setMinimum(0);
        minSpinBoxPtr -> setMaximum(59);
        minSpinBoxPtr -> setMinimum(0);
        secSpinBoxPtr -> setMaximum(59);
        secSpinBoxPtr -> setMinimum(0);

        // Set spinbox initial values
        unsigned long hour = duration/3600;
        unsigned long rem = duration - hour*3600;
        unsigned long min = rem/60;
        unsigned long sec = rem - min*60;
        hourSpinBoxPtr -> setValue(hour);
        minSpinBoxPtr  -> setValue(min);
        secSpinBoxPtr  -> setValue(sec);
    };


} // namespase bias
