#ifndef AUTO_NAMING_OPTIONS_HPP
#define AUTO_NAMING_OPTIONS_HPP
#include <QString>
#include <QStringList>
#include <string>

namespace bias
{
    class AutoNamingOptions
    {
        public:
            AutoNamingOptions();

            bool includeCameraIdentifier;
            bool includeTimeAndDate;

            int cameraIdentifier;
            static int DEFAULT_CAMERA_IDENTIFIER;
            enum {GUID_IDENTIFIER, CAMERA_NUMBER_IDENTIFIER};

            QString timeAndDateFormat;
            static QString DEFAULT_TIME_AND_DATE_FORMAT;

            static QStringList ALLOWED_TIME_AND_DATE_FORMATS;
            static QStringList createAllowedTimeAndDateFormats();

            QString toString();
            std::string toStdString();

    };

}
#endif
