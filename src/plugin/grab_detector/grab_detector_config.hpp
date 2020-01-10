#ifndef GRAB_DETECTOR_CONFIG_HPP
#define GRAB_DETECTOR_CONFIG_HPP

#include "rtn_status.hpp"
#include <QVariantMap>
#include <QColor>


namespace bias
{

    class GrabDetectorConfig
    {

        public:

            enum TriggerMode {
                TRIGGER_MODE_ONESHOT,
                TRIGGER_MODE_CONTINUOUS 
            };

            // Default device parameters 
            static const QString DEFAULT_DEVICE_PORT_NAME;
            static const bool DEFAULT_DEVICE_AUTO_CONNECT;
            static const double DEFAULT_DEVICE_PULSE_DURATION; 
            static const int DEFAULT_OUTPUT_PIN;

            // Default detection box parameters
            static const int DEFAULT_DETECTION_BOX_XPOS;
            static const int DEFAULT_DETECTION_BOX_YPOS;
            static const int DEFAULT_DETECTION_BOX_WIDTH;
            static const int DEFAULT_DETECTION_BOX_HEIGHT;
            static const QColor DEFAULT_DETECTION_BOX_COLOR;

            // Default trigger parameters
            static const TriggerMode DEFAULT_TRIGGER_MODE;
            static const bool DEFAULT_TRIGGER_ENABLED; 
            static const bool DEFAULT_TRIGGER_ARMED_STATE;
            static const bool DEFAULT_TRIGGER_INVERTED;
            static const int DEFAULT_TRIGGER_THRESHOLD;
            static const int DEFAULT_TRIGGER_MEDIAN_FILTER;

            // Device parameters
            QString devicePortName;
            bool deviceAutoConnect;
            double devicePulseDuration;
            int outputPin;

            // Dectection boc parameters
            int detectBoxXPos;
            int detectBoxYPos;
            int detectBoxWidth;
            int detectBoxHeight;
            QColor detectBoxColor;

            // Trigger parameters
            TriggerMode triggerMode;
            bool triggerEnabled;
            bool triggerArmedState;
            bool triggerInverted;
            int triggerThreshold;
            int triggerMedianFilter;

            GrabDetectorConfig();

            RtnStatus setDeviceFromMap(QVariantMap configMap);
            RtnStatus setDetectBoxFromMap(QVariantMap configMap);
            RtnStatus setTriggerFromMap(QVariantMap configMap);
            RtnStatus fromMap(QVariantMap configMap);
            QVariantMap toMap();

            RtnStatus fromJson(QByteArray jsonConfigArray);
            QByteArray toJson();

            QString getTriggerModeString();
            RtnStatus setTriggerMode(TriggerMode mode);
            RtnStatus setTriggerMode(QString modeString);

            QString toString();
            void print();

        protected:

    
    };

}

#endif
