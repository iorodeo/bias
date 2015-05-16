#include "grab_detector_config.hpp"
#include "json.hpp"
#include <iostream>
#include <QMessageBox>

namespace bias
{

    // Device parameters 
    const QString GrabDetectorConfig::DEFAULT_DEVICE_PORT_NAME("ttyUSB0");
    const bool GrabDetectorConfig::DEFAULT_DEVICE_AUTO_CONNECT = false;
    const double GrabDetectorConfig::DEFAULT_DEVICE_PULSE_DURATION = 0.2; 

    // Detection box parameters
    const int GrabDetectorConfig::DEFAULT_DETECTION_BOX_XPOS = 0;
    const int GrabDetectorConfig::DEFAULT_DETECTION_BOX_YPOS = 0;
    const int GrabDetectorConfig::DEFAULT_DETECTION_BOX_WIDTH = 100;
    const int GrabDetectorConfig::DEFAULT_DETECTION_BOX_HEIGHT = 100;
    const QColor GrabDetectorConfig::DEFAULT_DETECTION_BOX_COLOR = QColor(255,0,0);

    // Trigger parameters
    const bool GrabDetectorConfig::DEFAULT_TRIGGER_ENABLED = true;
    const bool GrabDetectorConfig::DEFAULT_TRIGGER_ARMED_STATE = false;
    const int GrabDetectorConfig::DEFAULT_TRIGGER_THRESHOLD = 100;
    const int GrabDetectorConfig::DEFAULT_TRIGGER_MEDIAN_FILTER = 3;


    GrabDetectorConfig::GrabDetectorConfig()
    {
            devicePortName = DEFAULT_DEVICE_PORT_NAME;;
            deviceAutoConnect = DEFAULT_DEVICE_AUTO_CONNECT;
            devicePulseDuration = DEFAULT_DEVICE_PULSE_DURATION;

            detectBoxXPos = DEFAULT_DETECTION_BOX_XPOS;
            detectBoxYPos = DEFAULT_DETECTION_BOX_YPOS;
            detectBoxWidth = DEFAULT_DETECTION_BOX_WIDTH;
            detectBoxHeight = DEFAULT_DETECTION_BOX_HEIGHT;
            detectBoxColor = DEFAULT_DETECTION_BOX_COLOR;;

            triggerEnabled = DEFAULT_TRIGGER_ENABLED;
            triggerArmedState = DEFAULT_TRIGGER_ARMED_STATE;
            triggerThreshold = DEFAULT_TRIGGER_THRESHOLD;
            triggerMedianFilter = DEFAULT_TRIGGER_MEDIAN_FILTER;
    }


    RtnStatus GrabDetectorConfig::fromMap(QVariantMap configMap)
    {
        QVariantMap oldConfigMap = toMap();

        RtnStatus rtnStatusDevice = setDeviceFromMap(configMap["device"].toMap());
        if (!rtnStatusDevice.success)
        {
            setDeviceFromMap(oldConfigMap["device"].toMap());
        }
        RtnStatus rtnStatusDetectBox = setDetectBoxFromMap(configMap["detectBox"].toMap());
        if (!rtnStatusDetectBox.success)
        {
            setDetectBoxFromMap(oldConfigMap["detectBox"].toMap());
        }
        RtnStatus rtnStatusTrigger = setTriggerFromMap(configMap["trigger"].toMap()); 
        if (!rtnStatusTrigger.success)
        {
            setTriggerFromMap(oldConfigMap["trigger"].toMap());
        }

        RtnStatus rtnStatus;
        rtnStatus.success =  rtnStatusDevice.success && rtnStatusDetectBox.success && rtnStatusTrigger.success;
        rtnStatus.message += rtnStatusDevice.message + QString(", ");  
        rtnStatus.message += rtnStatusDetectBox.message + QString(", ");
        rtnStatus.message += rtnStatusTrigger.message;
        return rtnStatus;
    }


    RtnStatus GrabDetectorConfig::setDeviceFromMap(QVariantMap configMap)
    {
        RtnStatus rtnStatus;

        if (configMap.isEmpty())
        {
            rtnStatus.success = true;
            rtnStatus.message = QString("grabDetector device config empty");
            return rtnStatus;
        }

        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (configMap.contains("portName"))
        {
            if (configMap["portName"].canConvert<QString>())
            {
                devicePortName = configMap["portName"].toString();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage("unable to convert portName to string");
            }
        }

        if (configMap.contains("autoConnect"))
        {
            if (configMap.contains("autoConnect"))
            {
                if (configMap["autoConnect"].canConvert<bool>())
                {
                    deviceAutoConnect = configMap["autoConnect"].toBool();
                }
                else
                {
                    rtnStatus.success = false;
                    rtnStatus.appendMessage("unable to convert autoConnect to bool");
                }
            }
        }

        if (configMap.contains("pulseDuration"))
        {
            if (configMap["pulseDuration"].canConvert<double>())
            {
                devicePulseDuration = configMap["pulseDuration"].toDouble();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage("unable to convert pulse duration to double");
            }
        }

        return rtnStatus;
    }


    RtnStatus GrabDetectorConfig::setDetectBoxFromMap(QVariantMap configMap)
    {
        RtnStatus rtnStatus;
        if (configMap.isEmpty())
        {
            rtnStatus.success = true;
            rtnStatus.message = QString("grabDetector detectbox config empty");
            return rtnStatus;
        }

        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (configMap.contains("xPos"))
        {
            if (configMap["xPos"].canConvert<int>())
            {
                detectBoxXPos = configMap["xPos"].toInt();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage("unable to convert xPos to int");
            }
        }

        if (configMap.contains("yPos"))
        {
            if (configMap["yPos"].canConvert<int>())
            {
                detectBoxYPos= configMap["yPos"].toInt();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage("unable to convert yPos to int");
            }
        }

        if (configMap.contains("width"))
        {
            if (configMap["width"].canConvert<int>())
            {
                detectBoxWidth = configMap["width"].toInt();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage("unable to convert width to int");
            }
        }

        if (configMap.contains("height"))
        {
            if (configMap["height"].canConvert<int>())
            {
                detectBoxHeight = configMap["height"].toInt();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage("unable to convert height to int");
            }
        }

        if (configMap.contains("color"))
        {
            if (configMap["color"].canConvert<QString>())
            {
                QColor color;  
                color.setNamedColor(configMap["color"].toString());
                if (color.isValid())
                {
                    detectBoxColor = color;
                }
                else
                {
                    rtnStatus.success = false;
                    rtnStatus.appendMessage("color is not valid");
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage("unable to convert color to string");
            }
        }

        return rtnStatus;
    }


    RtnStatus GrabDetectorConfig::setTriggerFromMap(QVariantMap configMap)
    {
        RtnStatus rtnStatus;
        if (configMap.isEmpty())
        {
            rtnStatus.success = true;
            rtnStatus.message = QString("grabDetector trigger config empty");
            return rtnStatus;
        }

        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (configMap.contains("enabled"))
        {
            if (configMap["enabled"].canConvert<bool>())
            {
                triggerEnabled = configMap["enabled"].toBool();
            }
            else
            {
                rtnStatus.success =  false;
                rtnStatus.appendMessage(QString("unable to convert trigger enable to bool"));
            }
        }

        if (configMap.contains("armedState"))
        {
            if (configMap["armedState"].canConvert<bool>())
            {
                triggerArmedState = configMap["armedState"].toBool();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert armedState to bool"));
            }
        }

        if (configMap.contains("threshold"))
        {
            if (configMap["threshold"].canConvert<int>())
            {
                triggerThreshold = configMap["threshold"].toInt();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert threshold to int"));
            }
        }

        if (configMap.contains("medianFilter"))
        {
            if (configMap["medianFilter"].canConvert<int>())
            {
                triggerMedianFilter = configMap["medianFilter"].toInt();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage("unable to convert median filter to intl");
            }
        }
        return rtnStatus;
    }


    QVariantMap GrabDetectorConfig::toMap()
    {
        // Create Device map
        QVariantMap deviceMap;
        deviceMap.insert("portName", devicePortName);
        deviceMap.insert("autoConnect", deviceAutoConnect);
        deviceMap.insert("pulseDuration", devicePulseDuration);

        // Create detectBox map
        QVariantMap detectBoxMap;
        detectBoxMap.insert("xPos", detectBoxXPos);
        detectBoxMap.insert("yPos", detectBoxYPos);
        detectBoxMap.insert("width", detectBoxWidth);
        detectBoxMap.insert("height", detectBoxHeight);
        detectBoxMap.insert("color", detectBoxColor.name());

        // Create trigger map
        QVariantMap triggerMap;
        triggerMap.insert("enabled", triggerEnabled);
        triggerMap.insert("armedState", triggerArmedState);
        triggerMap.insert("threshold", triggerThreshold);
        triggerMap.insert("medianFilter", triggerMedianFilter);

        // Create map for whole configuration
        QVariantMap configMap;
        configMap.insert("device", deviceMap);
        configMap.insert("detectBox", detectBoxMap);
        configMap.insert("trigger", triggerMap);

        return configMap;
    }


    RtnStatus GrabDetectorConfig::fromJson(QByteArray jsonConfigArray)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        bool ok;
        QVariantMap configMap = QtJson::parse(QString(jsonConfigArray), ok).toMap();
        if (!ok)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("grabDetector unable to parse json configuration string");
            return rtnStatus;

        }
        rtnStatus = fromMap(configMap);
        return rtnStatus;
    }


    QByteArray GrabDetectorConfig::toJson()
    {
        QVariantMap configMap = toMap();

        bool ok;
        QByteArray jsonConfigArray = QtJson::serialize(configMap,ok);
        if (!ok)
        {
            jsonConfigArray = QByteArray();
        }
        return jsonConfigArray;
    }


    QString GrabDetectorConfig::toString()
    {
        QString configStr;
        configStr.append(QString("\n"));
        configStr.append(QString("Device: \n"));
        configStr.append(QString("  portName:       %1\n").arg(devicePortName ));
        configStr.append(QString("  autoConnect:    %1\n").arg(deviceAutoConnect));
        configStr.append(QString("  pulseDuration:  %1\n").arg(devicePulseDuration));

        configStr.append(QString("\n"));
        configStr.append(QString("DetectBox: \n"));
        configStr.append(QString("  xPos:           %1\n").arg(detectBoxXPos));
        configStr.append(QString("  yPos:           %1\n").arg(detectBoxYPos));
        configStr.append(QString("  width:          %1\n").arg(detectBoxWidth));
        configStr.append(QString("  height:         %1\n").arg(detectBoxHeight));
        configStr.append(QString("  color:          %1\n").arg(detectBoxColor.name()));

        configStr.append(QString("\n"));
        configStr.append(QString("Trigger: \n"));
        configStr.append(QString("  enabled:        %1\n").arg(triggerEnabled));
        configStr.append(QString("  armedState:     %1\n").arg(triggerArmedState));
        configStr.append(QString("  threshold:      %1\n").arg(triggerThreshold));
        configStr.append(QString("  medianFilter:   %1\n").arg(triggerMedianFilter));
        configStr.append(QString("\n"));
        return configStr;
    }


    void GrabDetectorConfig::print()
    {
        QString configStr = toString();
        std::cout << configStr.toStdString();
    }

}
