#include "display_event.hpp"
#include "json.hpp"

namespace bias
{
    // Public methods
    // ------------------------------------------------------------------------

    DisplayEvent::DisplayEvent()
    {
    }

    unsigned int DisplayEvent::patternId()
    {
        return patternId_;
    }


    RtnStatus DisplayEvent::setPatternId(unsigned int patternId)
    {
        patternId_ = patternId;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    double DisplayEvent::startTime()
    {
        return startTime_;
    }


    RtnStatus DisplayEvent::setStartTime(double startTime)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        if (startTime_ < 0.0)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("startTime must be >= 0");
        }
        else
        {
            startTime_ = startTime;
        }
        return rtnStatus;
    }


    double DisplayEvent::stopTime()
    {
        return stopTime_;
    }


    RtnStatus DisplayEvent::setStopTime(double stopTime)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        if (stopTime_ < 0.0)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("stopTime must be >= 0");
        }
        else
        {
            stopTime_ = stopTime;
        }
        return rtnStatus;
    }


    double DisplayEvent::controlBias()
    {
        return controlBias_;
    }


    RtnStatus DisplayEvent::setControlBias(double controlBias)
    {
        controlBias_ = controlBias;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    QString DisplayEvent::toString()
    {
        QString eventStr;
        eventStr.append(QString("  patternId:      %1\n").arg(patternId_));
        eventStr.append(QString("  startTime:      %1\n").arg(startTime_));
        eventStr.append(QString("  stopTime:       %1\n").arg(stopTime_));
        eventStr.append(QString("  controlBias:    %1\n").arg(controlBias_));
        return eventStr;
    }


    QVariantMap DisplayEvent::toMap()
    {
        QVariantMap map;
        map.insert("patternId", patternId_);
        map.insert("startTime", startTime_);
        map.insert("stopTime", stopTime_);
        map.insert("controlBias", controlBias_);
        return map;
    }

    QByteArray DisplayEvent::toJson()
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

    RtnStatus DisplayEvent::fromMap(QVariantMap map)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        // Get patternId
        // --------------------------------------------------------------------
        if (map.contains("patternId"))
        {
            if (map["patternId"].canConvert<unsigned int>())
            {
                unsigned int patternIdTmp = map["patternId"].toUInt();
                RtnStatus rtnSetPatternId = setPatternId(patternIdTmp);
                if (!rtnSetPatternId.success)
                {
                    rtnStatus.success = false;
                    rtnStatus.appendMessage(rtnSetPatternId.message);
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert patternId to unsigned int"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("display event missing patternId"));
        }

        // Get Start time
        // --------------------------------------------------------------------
        if (map.contains("startTime"))
        {
            if (map["startTime"].canConvert<double>())
            {
                double startTimeTmp = map["startTime"].toDouble();
                RtnStatus rtnSetStartTime = setStartTime(startTimeTmp);
                if (!rtnSetStartTime.success)
                {
                    rtnStatus.success = false;
                    rtnStatus.appendMessage(rtnSetStartTime.message);
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert startTime to double"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("display event missing startTime"));
        }

        // Get stopTime
        // --------------------------------------------------------------------
        if (map.contains("stopTime"))
        {
            if (map["stopTime"].canConvert<double>())
            {
                double stopTimeTmp = map["stopTime"].toDouble();
                RtnStatus rtnSetStopTime = setStopTime(stopTimeTmp);
                if (!rtnSetStopTime.success)
                {
                    rtnStatus.success = false;
                    rtnStatus.appendMessage(rtnSetStopTime.message);
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert stopTime to double"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("display event missing startTime"));
        }

        // Get controlBias
        // --------------------------------------------------------------------
        if (map.contains("controlBias"))
        {
            if (map["controlBias"].canConvert<double>())
            {
                double controlBiasTmp = map["controlBias"].toDouble();
                RtnStatus rtnSetControlBias = setControlBias(controlBiasTmp);
                if (!rtnSetControlBias.success)
                {
                    rtnStatus.success = false;
                    rtnStatus.appendMessage(rtnSetControlBias.message);
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert controlBias to double"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("display event missing controlBias"));
        }

        // Check values for correctness
        //  -------------------------------------------------------------------
        RtnStatus rtnCheckValues = checkValues();
        if (!rtnCheckValues.success)
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(rtnCheckValues.message);
        }
        return rtnStatus;
    }

    RtnStatus DisplayEvent::fromJson(QByteArray jsonArray)
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

    RtnStatus DisplayEvent::checkValues()
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        
        if (stopTime_ <= startTime_)
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("stopTime <= startTime"));
        }
        return rtnStatus;
    }

}
