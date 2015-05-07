#ifndef DISPLAY_EVENT_HPP
#define DISPLAY_EVENT_HPP

#include "rtn_status.hpp"
#include <QVariantMap>

namespace bias
{
    class DisplayEvent
    {
        public:

            DisplayEvent();

            unsigned int patternId();
            RtnStatus setPatternId(unsigned int patternId);

            double startTime();
            RtnStatus setStartTime(double startTime);

            double stopTime();
            RtnStatus setStopTime(double stopTime);

            double controlBias();
            RtnStatus setControlBias(double controlBias);

            QString toString();
            QVariantMap toMap();
            QByteArray toJson();

            RtnStatus fromMap(QVariantMap map);
            RtnStatus fromJson(QByteArray jsonArray);

            RtnStatus checkValues();

        protected:
            unsigned int patternId_ = 0;
            double startTime_ = 0.0;
            double stopTime_ = 1.0;
            double controlBias_ = 1.0;


    };

}

#endif 
