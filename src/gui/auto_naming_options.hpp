#ifndef AUTO_NAMING_OPTIONS_HPP
#define AUTO_NAMING_OPTIONS_HPP
#include <QString>
#include <QStringList>
#include <QMap>
#include <string>

namespace bias
{
    class AutoNamingOptions
    {
        public:
            enum {GUID_IDENTIFIER, CAMERA_NUMBER_IDENTIFIER};

            AutoNamingOptions();

            bool includeCameraIdentifier;
            bool includeTimeAndDate;
            bool includeVersionNumber;
            int cameraIdentifier;
            QString timeAndDateFormat;

            QString getCameraIdentifierString();
            QString toString();
            std::string toStdString();

            // Static data
            static int DEFAULT_CAMERA_IDENTIFIER;
            static QString DEFAULT_TIME_AND_DATE_FORMAT;
            static QMap<int,QString> IDENTIFIER_TO_STRING_MAP;
            static QStringList ALLOWED_TIME_AND_DATE_FORMATS;

            // Static methods
            static QString cameraIdentifierToString(int identifier);
            static int cameraIdentifierFromString(QString identifierString);
            static QMap<int,QString> createIdentifierToStringMap(); 
            static QStringList createAllowedTimeAndDateFormats();
            static bool isAllowedTimeAndDateFormat(QString formatString);

    };

}
#endif
