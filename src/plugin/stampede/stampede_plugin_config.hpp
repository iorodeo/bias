#ifndef STAMPEDE_PLUGIN_CONFIG_HPP
#define STAMPEDE_PLUGIN_CONFIG_HPP

#include "vibration_event.hpp"
#include "display_event.hpp"
#include <QVariantMap>

namespace bias
{
    class StampedePluginConfig 
    {
        public:

            StampedePluginConfig();

            void clearAllEvents();

            QList<VibrationEvent> vibrationEventList();
            RtnStatus setVibrationEventList(QList<VibrationEvent> vibrationEventList);
            RtnStatus appendVibrationEvent(VibrationEvent vibrationEvent);
            void clearVibrationEventList();

            QList<DisplayEvent> displayEventList();
            RtnStatus setDisplayEventList(QList<DisplayEvent> displayEventList);
            RtnStatus appendDisplayEvent(DisplayEvent displayEvent);
            void clearDisplayEventList();

            QString vibrationPortName();
            RtnStatus setVibrationPortName(QString name);

            QString displayPortName();
            RtnStatus setDisplayPortName(QString name);

            double duration();
            RtnStatus setDuration(double duration);

            QString toString();
            QVariantMap toMap();
            QByteArray toJson();

            RtnStatus fromMap(QVariantMap map);
            RtnStatus fromJson(QByteArray jsonArray);

            RtnStatus setVibrationFromMap(QVariantMap map);
            RtnStatus setDisplayFromMap(QVariantMap map);

            RtnStatus checkValues();
            RtnStatus checkVibrationEvents();
            RtnStatus checkDisplayEvents();

            void setToDefaultConfig();


        protected:

            double duration_ = 0.0; 
            QString vibrationPortName_;
            QString displayPortName_;
            QList<VibrationEvent> vibrationEventList_;
            QList<DisplayEvent> displayEventList_;



    };
}

#endif
