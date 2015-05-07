#ifndef VIBRATION_EVENT_HPP
#define VIBRATION_EVENT_HPP

#include "rtn_status.hpp"
#include <QVariantMap>


namespace bias
{
    class VibrationEvent
    {
        public:

            VibrationEvent();

            unsigned int number();
            RtnStatus setNumber(unsigned int number);

            double startTime();
            RtnStatus setStartTime(double startTime);

            double period();
            RtnStatus setPeriod(double period);

            QString toString();
            QVariantMap toMap();
            QByteArray toJson();

            RtnStatus fromMap(QVariantMap map);
            RtnStatus fromJson(QByteArray jsonArray);

            RtnStatus checkValues();

        protected:

            unsigned int number_ = 10;
            double startTime_ = 1.0;
            double period_ = 1.0;
            
    };

}

#endif

