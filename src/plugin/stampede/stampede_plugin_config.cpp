#include "stampede_plugin_config.hpp"
#include "json.hpp"

namespace bias
{
    // Public methods
    // ------------------------------------------------------------------------

    StampedePluginConfig::StampedePluginConfig()
    {
    }

    void StampedePluginConfig::clearAllEvents()
    {
        clearVibrationEventList();
        clearDisplayEventList();
    }

    QList<VibrationEvent> StampedePluginConfig::vibrationEventList() 
    {
        return vibrationEventList_;
    }


    RtnStatus StampedePluginConfig::setVibrationEventList(QList<VibrationEvent> vibrationEventList) 
    {
        for (auto event : vibrationEventList)
        {
            RtnStatus rtnCheckValues = event.checkValues();
            if (!rtnCheckValues.success)
            {
                return rtnCheckValues;
            }
        }
        vibrationEventList_ = vibrationEventList;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    RtnStatus StampedePluginConfig::appendVibrationEvent(VibrationEvent vibrationEvent) 
    {
        RtnStatus rtnCheckValues = vibrationEvent.checkValues();
        if (!rtnCheckValues.success)
        {
            return rtnCheckValues;
        }

        vibrationEventList_.append(vibrationEvent);
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }



    void StampedePluginConfig::clearVibrationEventList() 
    {
        vibrationEventList_.clear();
    }


    QList<DisplayEvent> StampedePluginConfig::displayEventList() 
    {
        return displayEventList_;
    }


    RtnStatus StampedePluginConfig::setDisplayEventList(QList<DisplayEvent> displayEventList) 
    {
        for (auto event : displayEventList)
        {
            RtnStatus rtnCheckValues = event.checkValues();
            if (!rtnCheckValues.success)
            {
                return rtnCheckValues;
            }
        }
        displayEventList_ = displayEventList;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
    }


    RtnStatus StampedePluginConfig::appendDisplayEvent(DisplayEvent displayEvent) 
    {
        RtnStatus rtnCheckValues = displayEvent.checkValues();
        if (!rtnCheckValues.success)
        {
            return rtnCheckValues;
        }
        displayEventList_.append(displayEvent);
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
    }


    void StampedePluginConfig::clearDisplayEventList() 
    {
        displayEventList_.clear();
    }


    QString StampedePluginConfig::vibrationPortName() 
    {
        return vibrationPortName_;
    } 

    RtnStatus StampedePluginConfig::setVibrationPortName(QString name) 
    {
        vibrationPortName_= name;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    } 


    QString StampedePluginConfig::displayPortName() 
    {
        return displayPortName_;
    } 

    RtnStatus StampedePluginConfig::setDisplayPortName(QString name) 
    {
        displayPortName_ = name;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    } 


    double StampedePluginConfig::duration() 
    {
        return duration_;
    } 

    RtnStatus StampedePluginConfig::setDuration(double duration) 
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (duration <= 0.0)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("duration must be > 0");
        }
        else
        {
            duration_ = duration;
        }
        return rtnStatus;
    } 


    QString StampedePluginConfig::toString() 
    {
        QString configStr;
        configStr.append(QString("\n"));
        configStr.append(QString("StampedePluginConfig:\n"));
        configStr.append(QString("duration: %1\n").arg(duration_));
        configStr.append(QString("vibrationPort: %1\n").arg(vibrationPortName_));
        configStr.append(QString("displayPort: %1\n").arg(displayPortName_));
        for (auto event : vibrationEventList_)
        {
            configStr.append(event.toString());
        }
        for (auto event : displayEventList_)
        {
            configStr.append(event.toString());
        }
        configStr.append(QString("\n"));
        return configStr;
    }


    QVariantMap StampedePluginConfig::toMap() 
    {
        QVariantList vibrationMapList;
        for (auto event : vibrationEventList_)
        {
            vibrationMapList.append(QVariant(event.toMap()));
        }

        QVariantList displayMapList;
        for (auto event : displayEventList_)
        {
            displayMapList.append(QVariant(event.toMap()));
        }

        QVariantMap vibrationMap;
        vibrationMap.insert("port", vibrationPortName_);
        vibrationMap.insert("events", vibrationMapList);

        QVariantMap displayMap;
        displayMap.insert("port", displayPortName_);
        displayMap.insert("events", displayMapList);

        QVariantMap configMap;
        configMap.insert("duration", duration_);
        configMap.insert("vibration", vibrationMap);
        configMap.insert("display", displayMap);
        return configMap;
    }


    QByteArray StampedePluginConfig::toJson() 
    {
        QVariantMap map = toMap();
        bool ok;
        QByteArray jsonArray = QtJson::serialize(map,ok);
        if (!ok)
        {
            jsonArray = QByteArray();
        }
        return jsonArray;
    }


    RtnStatus StampedePluginConfig::fromMap(QVariantMap configMap) 
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        // Get druation
        // --------------------------------------------------------------------
        if (configMap.contains("duration"))
        {
            if (configMap["duration"].canConvert<double>())
            {
                double durationTmp = configMap["duration"].toDouble();
                RtnStatus rtnSetDuration = setDuration(durationTmp);
                if (!rtnSetDuration.success)
                {
                    rtnStatus.success = false;
                    rtnStatus.appendMessage(rtnSetDuration.message);
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert duration to double"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("duration field missing"));
        }

        // Get vibration configuration
        // --------------------------------------------------------------------
        if (configMap.contains("vibration"))
        {
            QVariantMap vibrationMap = configMap["vibration"].toMap();
            RtnStatus rtnSetVibration = setVibrationFromMap(vibrationMap);
            if (!rtnSetVibration.success)
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(rtnSetVibration.message);
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("vibration field missing"));
        }

        // Get dsiplay configuration
        if (configMap.contains("display"))
        {
            QVariantMap displayMap = configMap["display"].toMap();
            RtnStatus rtnSetDisplay = setDisplayFromMap(displayMap);
            if (!rtnSetDisplay.success)
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(rtnSetDisplay.message);
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage("display field missing");
        }

        return rtnStatus;
    }


    RtnStatus StampedePluginConfig::setVibrationFromMap(QVariantMap map)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (map.isEmpty())
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("vibration configuration map is empty"));
            return rtnStatus;
        }

        //  Get port name
        //  -------------------------------------------------------------------
        if (map.contains("port"))
        {
            if (map["port"].canConvert<QString>())
            {
                vibrationPortName_ = map["port"].toString();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert port to QString"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("vibration port field missing"));
        }

        // Get events
        // --------------------------------------------------------------------
        if (map.contains("events"))
        {
            if (map["events"].canConvert<QVariantList>())
            {

                QVariantList eventsList = map["events"].toList();
                QList<VibrationEvent> vibrationEventListTmp;

                // Convert each item in the events list and store in temporary list
                for (auto item : eventsList)
                {
                    QVariantMap itemMap = item.toMap();
                    if (!itemMap.isEmpty())
                    {
                        VibrationEvent event;
                        RtnStatus rtnFromMap = event.fromMap(itemMap);
                        if (rtnFromMap.success)
                        {
                            vibrationEventListTmp.append(event);
                        }
                        else
                        {
                            rtnStatus.success = false;
                            rtnStatus.appendMessage(rtnFromMap.message);
                        }
                    }
                    else
                    {
                        rtnStatus.success = false;
                        rtnStatus.appendMessage(QString("unable to convert event list item to QVariantMap"));
                    }
                }
                // If conversions are success set new vibrationEventList
                if (rtnStatus.success)
                {
                    vibrationEventList_ = vibrationEventListTmp;
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert events to QVariantList"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("vibration configuration missing events field"));
        }
        return rtnStatus;
    }


    RtnStatus StampedePluginConfig::setDisplayFromMap(QVariantMap map)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        if (map.isEmpty())
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("display configuration map is empty");
            return rtnStatus;
        }

        //  Get port name
        //  -------------------------------------------------------------------
        if (map.contains("port"))
        {
            if (map["port"].canConvert<QString>())
            {
                vibrationPortName_ = map["port"].toString();
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert port to QString"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("display port field missing"));
        }

        // Get events
        // --------------------------------------------------------------------
        if (map.contains("events"))
        {
            if (map["events"].canConvert<QVariantList>())
            {

                QVariantList eventsList = map["events"].toList();
                QList<DisplayEvent> displayEventListTmp;

                // Convert each item in the events list and store in temporary list
                for (auto item : eventsList)
                {
                    QVariantMap itemMap = item.toMap();
                    if (!itemMap.isEmpty())
                    {
                        DisplayEvent event;
                        RtnStatus rtnFromMap = event.fromMap(itemMap);
                        if (rtnFromMap.success)
                        {
                            displayEventListTmp.append(event);
                        }
                        else
                        {
                            rtnStatus.success = false;
                            rtnStatus.appendMessage(rtnFromMap.message);
                        }
                    }
                    else
                    {
                        rtnStatus.success = false;
                        rtnStatus.appendMessage(QString("unable to convert event list item to QVariantMap"));
                    }
                }
                // If conversions are success set new displayEventList
                if (rtnStatus.success)
                {
                    displayEventList_ = displayEventListTmp;
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert events to QVariantList"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("display configuration missing events field"));
        }
        return rtnStatus;
    }


    RtnStatus StampedePluginConfig::fromJson(QByteArray jsonArray) 
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        bool ok;
        QVariantMap map = QtJson::parse(QString(jsonArray), ok).toMap();
        if (!ok)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("unable to parse json configuration string");
        }
        else
        {
            rtnStatus = fromMap(map);
        }
        return rtnStatus;
    }

   void StampedePluginConfig::setToDefaultConfig()
   {
       duration_ = 90.0;
       QString vibrationPortName_ = QString("COM1");
       QString displayPortName_ = QString("COM2");

       // Add Vibration events
       clearVibrationEventList();

       VibrationEvent vibEvent;
       vibEvent.setStartTime(5.0);
       vibEvent.setPeriod(1.0);
       vibEvent.setNumber(10);
       appendVibrationEvent(vibEvent);
       vibEvent.setStartTime(35.0);
       vibEvent.setPeriod(1.0);
       vibEvent.setNumber(10);
       appendVibrationEvent(vibEvent);


       // Add Display events
       clearDisplayEventList();
       DisplayEvent dspEvent;
       dspEvent.setPatternId(1);
       dspEvent.setStartTime(20.0);
       dspEvent.setStopTime(30.0);
       dspEvent.setControlBias(1.0);
       appendDisplayEvent(dspEvent);
       dspEvent.setPatternId(1);
       dspEvent.setStartTime(50.0);
       dspEvent.setStopTime(60.0);
       dspEvent.setControlBias(1.0);
       appendDisplayEvent(dspEvent);

   }

}
