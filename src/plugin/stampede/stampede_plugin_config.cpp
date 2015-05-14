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

    VibrationEvent StampedePluginConfig::getVibrationEvent(int index)
    {
        VibrationEvent event;
        if ((index >= 0) && index < vibrationEventList_.size())
        {
            event = vibrationEventList_[index];
        }
        return event;
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


    DisplayEvent StampedePluginConfig::getDisplayEvent(int index)
    {
        DisplayEvent event;
        if ((index >=0) && index < displayEventList_.size())
        {
            event = displayEventList_[index];
        }
        return event;
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


    unsigned long StampedePluginConfig::duration() 
    {
        return duration_;
    } 

    RtnStatus StampedePluginConfig::setDuration(unsigned long duration) 
    {
        duration_ = duration;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    } 

    unsigned int StampedePluginConfig::arenaConfigId()
    {
        return arenaConfigId_;
    }


    RtnStatus StampedePluginConfig::setArenaConfigId(unsigned int id)
    {
        arenaConfigId_ = id;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    QString StampedePluginConfig::toString() 
    {
        QString configStr;
        configStr.append(QString("duration:         %1\n").arg(duration_));
        configStr.append(QString("vibration port:   %1\n").arg(vibrationPortName_));
        configStr.append(QString("display   port:   %1\n").arg(displayPortName_));
        configStr.append(QString("arena config id:  %1\n").arg(arenaConfigId_));
        configStr.append(QString("\n"));

        for (auto i=0; i<vibrationEventList_.size(); i++)
        {
            configStr.append(QString("vibration event   %1 \n").arg(i));
            configStr.append(vibrationEventList_[i].toString());
            configStr.append(QString("\n"));
        }

        for (auto i=0; i<displayEventList_.size(); i++)
        {
            configStr.append(QString("display event     %1 \n").arg(i));
            configStr.append(displayEventList_[i].toString());
            configStr.append(QString("\n"));
        }
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
        displayMap.insert("arenaConfigId", arenaConfigId_);
        displayMap.insert("events", displayMapList);

        QVariantMap configMap;
        configMap.insert("duration", (long long)(duration_));
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
            if (configMap["duration"].canConvert<unsigned long long>())
            {
                unsigned long durationTmp = (unsigned long)(configMap["duration"].toULongLong());
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
                rtnStatus.appendMessage(QString("unable to convert duration to unsigned long"));
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

        // Get arenaConfigId
        // --------------------------------------------------------------------
        if (map.contains("arenaConfigId"))
        {
            if (map["arenaConfigId"].canConvert<unsigned int>())
            {
                unsigned int arenaConfigIdTmp = map["arenaConfigId"].toUInt();
                RtnStatus rtnSetArenaConfigId = setArenaConfigId(arenaConfigIdTmp);
                if (!rtnSetArenaConfigId.success)
                {
                    rtnStatus.success = false;
                    rtnStatus.appendMessage(rtnSetArenaConfigId.message);
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert arenaConfigId to unsigned int"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("display event missing arenaConfigId"));
        }

        //  Get port name
        //  -------------------------------------------------------------------
        if (map.contains("port"))
        {
            if (map["port"].canConvert<QString>())
            {
                displayPortName_ = map["port"].toString();
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

    RtnStatus StampedePluginConfig::checkEvents() 
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        RtnStatus rtnCheckVibEvts = checkVibrationEvents();
        if (!rtnCheckVibEvts.success)
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(rtnCheckVibEvts.message);
        }

        RtnStatus rtnCheckDspEvts = checkDisplayEvents();
        if (!rtnCheckDspEvts.success)
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(rtnCheckDspEvts.message);
        }

        return rtnStatus;
    }


    RtnStatus StampedePluginConfig::checkVibrationEvents() 
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        QList<double> timeList;
        for (auto event : vibrationEventList_)
        {
            RtnStatus rtnCheckValues = event.checkValues();
            if (!rtnCheckValues.success)
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(rtnCheckValues.message);
            }

            double startTime = event.startTime();
            double stopTime = event.startTime() + event.number()*event.period();
            timeList.append(startTime);
            timeList.append(stopTime);
        }

        for (auto i=0; i<timeList.size()-1; i++)
        {
            if (timeList[i] > timeList[i+1])
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("overlapping vibration events"));
            }
        }
        return rtnStatus;
    }


    RtnStatus StampedePluginConfig::checkDisplayEvents() 
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        QList<double> timeList;
        for (auto event : displayEventList_)
        {
            RtnStatus rtnCheckValues = event.checkValues();
            if (!rtnCheckValues.success)
            {
                rtnStatus.success = false;
                rtnStatus.message = QString(rtnCheckValues.message);
            }
            timeList.append(event.startTime());
            timeList.append(event.stopTime());
        }

        for (auto i=0; i<timeList.size()-1; i++)
        {
            if (timeList[i] > timeList[i-1])
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("overlapping display events"));
            }
        }
        return rtnStatus;
    }

    void StampedePluginConfig::setToDefaultConfig()
    {
        setDuration(30);
        setVibrationPortName("ttyUSB0");
        setDisplayPortName("ttyUSB1");
        setArenaConfigId(1);

        // Add Vibration events
        clearVibrationEventList();

        VibrationEvent vibEvent;
        vibEvent.setStartTime(2.0);
        vibEvent.setPeriod(0.25);
        vibEvent.setNumber(10);
        appendVibrationEvent(vibEvent);

        vibEvent.setStartTime(12.0);
        vibEvent.setPeriod(0.25);
        vibEvent.setNumber(10);
        appendVibrationEvent(vibEvent);

        // Add Display events
        clearDisplayEventList();
        DisplayEvent dspEvent;
        dspEvent.setPatternId(1);
        dspEvent.setStartTime(6.0);
        dspEvent.setStopTime(10.0);
        dspEvent.setControlBias(5);
        appendDisplayEvent(dspEvent);

        // Add Display events
        dspEvent.setPatternId(1);
        dspEvent.setStartTime(20.0);
        dspEvent.setStopTime(25.0);
        dspEvent.setControlBias(-5.0);
        appendDisplayEvent(dspEvent);


    }

}
