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

            QList<VibrationEvent> vibrationEventList_;
            QList<DisplayEvent> displayEventList_;

            StampedePluginConfig();
            QString toString();
            QVariantMap toMap();
            QByteArray toJson();

            RtnStatus fromMap(QVariantMap map);
            RtnStatus fromJson(QByteArray jsonArray);
    };
}

#endif
