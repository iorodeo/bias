#include "auto_naming_options.hpp"
#include <QStringList>

namespace bias
{
     int AutoNamingOptions::DEFAULT_CAMERA_IDENTIFIER = 
         AutoNamingOptions::CAMERA_NUMBER_IDENTIFIER;

     QStringList AutoNamingOptions::createAllowedTimeAndDateFormats()
     {
         QStringList allowedFormats;
         allowedFormats << QString("dd_MM_yyyy_hh:mm:ss");
         allowedFormats << QString("ddd_MMMM_d_yyyy_h:m:s_ap");
         return allowedFormats;
     }

     QStringList AutoNamingOptions::ALLOWED_TIME_AND_DATE_FORMATS = 
         AutoNamingOptions::createAllowedTimeAndDateFormats();


     QString AutoNamingOptions::DEFAULT_TIME_AND_DATE_FORMAT = 
       AutoNamingOptions::ALLOWED_TIME_AND_DATE_FORMATS[0]; 


     AutoNamingOptions::AutoNamingOptions()
     {
         includeCameraIdentifier = true;
         includeTimeAndDate = true;
         cameraIdentifier = DEFAULT_CAMERA_IDENTIFIER;
         timeAndDateFormat = DEFAULT_TIME_AND_DATE_FORMAT;

     }


     QString AutoNamingOptions::toString()
     {
         QStringList optionsList;
         optionsList << QString("includeCameraIdentifier: %1").arg(includeCameraIdentifier);
         optionsList << QString("includeTimeAndDate:      %1").arg(includeTimeAndDate);
         optionsList << QString("cameraIdentifier:        %1").arg(cameraIdentifier);
         optionsList << QString("timeAndDateFormat:       %1").arg(timeAndDateFormat);
         return optionsList.join("\n");
     }


     std::string AutoNamingOptions::toStdString()
     {
         return toString().toStdString();
     }

}
