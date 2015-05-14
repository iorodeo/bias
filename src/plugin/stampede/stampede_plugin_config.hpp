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
            VibrationEvent getVibrationEvent(int index);
            RtnStatus appendVibrationEvent(VibrationEvent vibrationEvent);
            void clearVibrationEventList();

            QList<DisplayEvent> displayEventList();
            RtnStatus setDisplayEventList(QList<DisplayEvent> displayEventList);
            DisplayEvent getDisplayEvent(int index);
            RtnStatus appendDisplayEvent(DisplayEvent displayEvent);
            void clearDisplayEventList();

            QString vibrationPortName();
            RtnStatus setVibrationPortName(QString name);

            QString displayPortName();
            RtnStatus setDisplayPortName(QString name);

            unsigned long duration();
            RtnStatus setDuration(unsigned long duration);

            unsigned int arenaConfigId();
            RtnStatus setArenaConfigId(unsigned int id);

            QString toString();
            QVariantMap toMap();
            QByteArray toJson();

            RtnStatus fromMap(QVariantMap map);
            RtnStatus fromJson(QByteArray jsonArray);

            RtnStatus setVibrationFromMap(QVariantMap map);
            RtnStatus setDisplayFromMap(QVariantMap map);

            RtnStatus checkEvents();
            RtnStatus checkVibrationEvents();
            RtnStatus checkDisplayEvents();

            void setToDefaultConfig();

        protected:

            unsigned long duration_ = 0; 
            unsigned int arenaConfigId_ = 1; 
            QString vibrationPortName_;
            QString displayPortName_;
            QList<VibrationEvent> vibrationEventList_;
            QList<DisplayEvent> displayEventList_;



    };
}

#endif
