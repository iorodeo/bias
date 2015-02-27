#ifndef GRAB_DETECTOR_PLUGIN_HPP
#define GRAB_DETECTOR_PLUGIN_HPP

#include "ui_grab_detector_plugin.h"
#include "bias_plugin.hpp"

namespace bias
{
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

            GrabDetectorPlugin(QWidget *parent=0);

        protected:

            bool triggerArmed_;
            int triggerThreshold_;
            int triggerFilterSize_;

            void connectWidgets();
            void initialize();

        private slots:

            void comPortComboBoxIndexChanged(QString text);
            void connectPushButtonPressed();
            void connectPushButtonClicked();
            void levelDblSpinBoxValueChanged(double value);
            void durationDblSpinBoxValueChanged(double value);
            void xPosSliderValueChanged(int value);
            void yPosSliderValueChanged(int value);
            void widthSliderValueChanged(int value);
            void heightSliderValueChanged(int value);
            void colorSelectPushButtonClicked();
            void trigResetPushButtonClicked();

    };
}
#endif
