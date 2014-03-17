#include "auto_naming_dialog.hpp"

namespace bias
{
    // AutoNamingOptions
    // ------------------------------------------------------------------------
     int AutoNamingOptions::DEFAULT_CAMERA_IDENTIFIER = 
         AutoNamingOptions::CAMERA_NUMBER_IDENTIFIER;

     QString AutoNamingOptions::DEFAULT_TIME_AND_DATE_FORMAT = 
         QString("dd.MM.yyyy_hh:mm:ss");

     AutoNamingOptions::AutoNamingOptions()
     {
         cameraIdentifier = DEFAULT_CAMERA_IDENTIFIER;
         timeAndDateFormat = DEFAULT_TIME_AND_DATE_FORMAT;

     }


    // AutoNaming Dialog
    // ------------------------------------------------------------------------

    // Public methods
    // ------------------------------------------------------------------------
    AutoNamingDialog::AutoNamingDialog(QWidget *parent)
    {
        AutoNamingOptions options; // Options w/ default values
        initialize(options);
    }

    AutoNamingDialog::AutoNamingDialog(AutoNamingOptions options, QWidget *parent)
    {
        initialize(options);
    }


    // Private methods
    // ------------------------------------------------------------------------
    void AutoNamingDialog::initialize(AutoNamingOptions options)
    {
        setupUi(this);
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

}
