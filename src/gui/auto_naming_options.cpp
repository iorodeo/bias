#include "auto_naming_options.hpp"
#include <QList>
#include <QStringList>

namespace bias
{


    // Static data
    // ------------------------------------------------------------------------
    int AutoNamingOptions::DEFAULT_CAMERA_IDENTIFIER = 
        AutoNamingOptions::CAMERA_NUMBER_IDENTIFIER;


    QMap<int,QString> AutoNamingOptions::IDENTIFIER_TO_STRING_MAP = 
        AutoNamingOptions::createIdentifierToStringMap();


    QStringList AutoNamingOptions::ALLOWED_TIME_AND_DATE_FORMATS = 
        AutoNamingOptions::createAllowedTimeAndDateFormats();


    QString AutoNamingOptions::DEFAULT_TIME_AND_DATE_FORMAT = 
        AutoNamingOptions::ALLOWED_TIME_AND_DATE_FORMATS[0]; 


    // Public methods
    // ------------------------------------------------------------------------
    AutoNamingOptions::AutoNamingOptions()
    {
        includeCameraIdentifier = true;
        includeTimeAndDate = true;
        includeVersionNumber = true;
        cameraIdentifier = DEFAULT_CAMERA_IDENTIFIER;
        timeAndDateFormat = DEFAULT_TIME_AND_DATE_FORMAT;

    }

    QString AutoNamingOptions::toString()
    {
        QStringList optionsList;
        optionsList << QString("includeCameraIdentifier: %1").arg(includeCameraIdentifier);
        optionsList << QString("includeTimeAndDate:      %1").arg(includeTimeAndDate);
        optionsList << QString("includeVersionNumber:    %1").arg(includeVersionNumber);
        optionsList << QString("cameraIdentifier:        %1").arg(getCameraIdentifierString());
        optionsList << QString("timeAndDateFormat:       %1").arg(timeAndDateFormat);
        return optionsList.join("\n");
    }


    std::string AutoNamingOptions::toStdString()
    {
        return toString().toStdString();
    }


    // Static methods
    // ------------------------------------------------------------------------ 
    QString AutoNamingOptions::cameraIdentifierToString(int identifier)
    {
        QString identifierString;
        if (!IDENTIFIER_TO_STRING_MAP.contains(identifier))
        {
            identifierString = QString("unknownIdentifier");
        }
        else
        {
            identifierString = IDENTIFIER_TO_STRING_MAP[identifier];
        }
        return identifierString;
    }


    int AutoNamingOptions::cameraIdentifierFromString(QString identifierString)
    {
        int identifier;

        QList<int> keyList = IDENTIFIER_TO_STRING_MAP.keys(identifierString);
        if (keyList.isEmpty())
        {
            identifier = -1;
        }
        else
        {
            identifier = keyList[0];
        }
        return identifier;
    }


    QString AutoNamingOptions::getCameraIdentifierString()
    {
        return AutoNamingOptions::cameraIdentifierToString(cameraIdentifier);
    }

    QMap<int,QString> AutoNamingOptions::createIdentifierToStringMap() 
    {
        QMap<int,QString> idToStringMap;
        idToStringMap.insert(GUID_IDENTIFIER,"Guid");
        idToStringMap.insert(CAMERA_NUMBER_IDENTIFIER, "CameraNumber");
        return idToStringMap;
    }

    QStringList AutoNamingOptions::createAllowedTimeAndDateFormats()
    {
        QStringList allowedFormats;
        allowedFormats << QString("'date'_yyyy_MM_dd_'time'_hh_mm_ss");
        allowedFormats << QString("'date'_dd_MM_yyyy_'time'_hh_mm_ss");
        allowedFormats << QString("yyyy_MM_dd_hh_mm_ss");
        allowedFormats << QString("dd_MM_yyyy_hh_mm_ss");
        allowedFormats << QString("ddd_MMMM_d_yyyy_'hr'h_'min'm_'sec's_ap");
        allowedFormats << QString("ddd_MMMM_d_yyyy_h_m_s_ap");
        return allowedFormats;
    }

    bool AutoNamingOptions::isAllowedTimeAndDateFormat(QString formatString)
    {
        return ALLOWED_TIME_AND_DATE_FORMATS.contains(formatString);
    }

}
