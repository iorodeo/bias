#include "auto_naming_dialog.hpp"
#include <iostream>

namespace bias
{

    // Public methods
    // ------------------------------------------------------------------------
    AutoNamingDialog::AutoNamingDialog(QWidget *parent)
    {
        AutoNamingOptions options; // Options w/ default values
        initialize(options,1);
    }

    AutoNamingDialog::AutoNamingDialog(
            AutoNamingOptions options, 
            unsigned int numberOfCameras,
            QWidget *parent
            )
    {
        initialize(options,numberOfCameras);
    }

    // Private slots
    // ------------------------------------------------------------------------
    void AutoNamingDialog::cameraIdGroupBoxToggled(bool checked)
    {
        std::cout << __PRETTY_FUNCTION__ << ", checked = " << checked << std::endl;
        AutoNamingOptions options;
        emit autoNamingOptionsChanged(options);
    }


    void AutoNamingDialog::guidRadioButtonToggled(bool checked)
    {
        std::cout << __PRETTY_FUNCTION__ << ", checked = " << checked << std::endl;
        AutoNamingOptions options;
        emit autoNamingOptionsChanged(options);
    }


    void AutoNamingDialog::timeAndDateGroupBoxToggled(bool checked)
    {
        std::cout << __PRETTY_FUNCTION__ << ", checked = " << checked << std::endl;
        AutoNamingOptions options;
        emit autoNamingOptionsChanged(options);
    }


    // Private methods
    // ------------------------------------------------------------------------
    void AutoNamingDialog::initialize(
            AutoNamingOptions options, 
            unsigned int numberOfCameras
            )
    {
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

        setupUi(this);
        connectWidgets();

        QStringListIterator allowedFormatsIt(AutoNamingOptions::ALLOWED_TIME_AND_DATE_FORMATS);
        while (allowedFormatsIt.hasNext())
        {
            timeAndDateFormatComboBoxPtr_ -> addItem(allowedFormatsIt.next());
        }

        
    }


    void AutoNamingDialog::connectWidgets()
    {

        connect(
                cameraIdGroupBoxPtr_,
                SIGNAL(toggled(bool)),
                this,
                SLOT(cameraIdGroupBoxToggled(bool))
               );

        connect(
                guidRadioButtonPtr_,
                SIGNAL(toggled(bool)),
                this,
                SLOT(guidRadioButtonToggled(bool))
               );

        connect(
                timeAndDateGroupBoxPtr_,
                SIGNAL(toggled(bool)),
                this,
                SLOT(timeAndDateGroupBoxToggled(bool))
               );

    }

}
