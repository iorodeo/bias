#include "vibration_event.hpp"
#include "json.hpp"

namespace bias
{


    // Public methods
    // ------------------------------------------------------------------------
    VibrationEvent::VibrationEvent()
    {
    }


    unsigned int VibrationEvent::number()
    {
        return number_;
    }


    RtnStatus VibrationEvent::setNumber(unsigned int number)
    {
        number_ = number;
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }


    double VibrationEvent::startTime()
    {
        return startTime_;
    }


    RtnStatus VibrationEvent::setStartTime(double startTime)
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

    double VibrationEvent::stopTime()
    {
        return startTime_ + period_*number_;
    }

    double VibrationEvent::period()
    {
        return period_;
    }


    RtnStatus VibrationEvent::setPeriod(double period)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        if (startTime_ < 0.0)
        {
            rtnStatus.success = false;
            rtnStatus.message = QString("period must be >= 0");
        }
        else
        {
            period_ = period;
        }
        return rtnStatus;
    }


    QString VibrationEvent::toString()
    {
        QString eventStr;
        eventStr.append(QString("  number:         %1\n").arg(number_));
        eventStr.append(QString("  startTime:      %1\n").arg(startTime_));
        eventStr.append(QString("  period:         %1\n").arg(period_));
        return eventStr;
    }


    QVariantMap VibrationEvent::toMap()
    {
        QVariantMap map;
        map.insert("number", number_);
        map.insert("startTime", startTime_);
        map.insert("period", period_);
        return map;
    }


    QByteArray VibrationEvent::toJson()
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


    RtnStatus VibrationEvent::fromMap(QVariantMap map)
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");

        // Get number of pulses
        // --------------------------------------------------------------------
        if (map.contains("number"))
        {
            if (map["number"].canConvert<unsigned int>())
            {
                unsigned int numberTmp = map["number"].toUInt();
                RtnStatus rtnSetNumber = setNumber(numberTmp);
                if (!rtnSetNumber.success)
                {
                    rtnStatus.success = false;
                    rtnStatus.appendMessage(rtnSetNumber.message);
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert number field to unsigned int"));

            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("vibration event missing number (of pulses) field"));
        }

        // Get start time of pulses
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
            rtnStatus.appendMessage(QString("vibration event missing startTime field"));
        }

        // Get period of pulses 
        // --------------------------------------------------------------------
        if (map.contains("period"))
        {
            if (map["period"].canConvert<double>())
            {
                double periodTmp = map["period"].toDouble();
                RtnStatus rtnSetPeriod = setPeriod(periodTmp);
                if (!rtnSetPeriod.success)
                {
                    rtnStatus.success = false;
                    rtnStatus.appendMessage(rtnSetPeriod.message);
                }
            }
            else
            {
                rtnStatus.success = false;
                rtnStatus.appendMessage(QString("unable to convert period to double"));
            }
        }
        else
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(QString("vibration event missing period field"));
        }

        // Check values
        // --------------------------------------------------------------------
        RtnStatus rtnCheckValues = checkValues();
        if (!rtnCheckValues.success)
        {
            rtnStatus.success = false;
            rtnStatus.appendMessage(rtnCheckValues.message);
        }
        return rtnStatus;
    }


    RtnStatus VibrationEvent::fromJson(QByteArray jsonArray)
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

    RtnStatus VibrationEvent::checkValues()
    {
        RtnStatus rtnStatus;
        rtnStatus.success = true;
        rtnStatus.message = QString("");
        return rtnStatus;
    }
}
