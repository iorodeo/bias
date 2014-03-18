#include "auto_naming_dialog.hpp"
#include <iostream>

namespace bias
{

    // Public methods
    // ------------------------------------------------------------------------
    AutoNamingDialog::AutoNamingDialog(QWidget *parent) : QDialog(parent)
    {
        AutoNamingOptions options; // Options w/ default values
        initialize(options);
    }

    AutoNamingDialog::AutoNamingDialog(
            AutoNamingOptions options, 
            unsigned int numberOfCameras,
            QWidget *parent
            ) 
        : QDialog(parent)
    {
        initialize(options,numberOfCameras);
    }

    // Private slots
    // ------------------------------------------------------------------------
    void AutoNamingDialog::dialogWidgetChanged()
    {
        AutoNamingOptions options = getOptionsFromDialog();
        emit autoNamingOptionsChanged(options);
    }

    
    void AutoNamingDialog::imageCaptureStarted(bool logging)
    {
        setEnabled(false);
    }


    void AutoNamingDialog::imageCaptureStopped()
    {
        setEnabled(true);
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

        numberOfCameras_ = numberOfCameras;
        if (numberOfCameras_ > 1)
        {
            cameraIdentifierGroupBoxPtr_ -> setCheckable(false);
        }
        else
        {
            cameraIdentifierGroupBoxPtr_ -> setCheckable(true);
        }

        QStringListIterator allowedFormatsIt(AutoNamingOptions::ALLOWED_TIME_AND_DATE_FORMATS);
        while (allowedFormatsIt.hasNext())
        {
            timeAndDateFormatComboBoxPtr_ -> addItem(allowedFormatsIt.next());
        }

        setDialogFromOptions(options);
        
    }


    void AutoNamingDialog::connectWidgets()
    {

        connect(
                cameraIdentifierGroupBoxPtr_,
                SIGNAL(toggled(bool)),
                this,
                SLOT(dialogWidgetChanged())
               );

        connect(
                guidIdentifierRadioButtonPtr_,
                SIGNAL(toggled(bool)),
                this,
                SLOT(dialogWidgetChanged())
               );

        connect(
                numberIdentifierRadioButtonPtr_,
                SIGNAL(toggled(bool)),
                this,
                SLOT(dialogWidgetChanged())
               );

        connect(
                timeAndDateGroupBoxPtr_,
                SIGNAL(toggled(bool)),
                this,
                SLOT(dialogWidgetChanged())
               );

        connect(
                timeAndDateFormatComboBoxPtr_,
                SIGNAL(currentIndexChanged(int)),
                this,
                SLOT(dialogWidgetChanged())
               );

        connect(
                parent(),
                SIGNAL(imageCaptureStarted(bool)),
                this,
                SLOT(imageCaptureStarted(bool))
               );

        connect(
                parent(),
                SIGNAL(imageCaptureStopped()),
                this,
                SLOT(imageCaptureStopped())
               );

    }


    void AutoNamingDialog::setDialogFromOptions(AutoNamingOptions options)
    {
        // Set camera identifier radios button
        if (options.cameraIdentifier == AutoNamingOptions::CAMERA_NUMBER_IDENTIFIER)
        {
            guidIdentifierRadioButtonPtr_ -> setChecked(false);
            numberIdentifierRadioButtonPtr_ -> setChecked(true);

        }
        else
        {
            guidIdentifierRadioButtonPtr_ -> setChecked(true);
            numberIdentifierRadioButtonPtr_ -> setChecked(false);
        }

        // Set camera identifier checkbox
        if (options.includeCameraIdentifier) 
        {
            cameraIdentifierGroupBoxPtr_ -> setChecked(true);

        }
        else
        {
            cameraIdentifierGroupBoxPtr_ -> setChecked(false);
        }

        // Set time and date check box
        if (options.includeTimeAndDate)
        {
            timeAndDateGroupBoxPtr_ -> setChecked(true);
        }
        else
        {
            timeAndDateGroupBoxPtr_ -> setChecked(false);
        }

        // Set time and date format options
        int index = timeAndDateFormatComboBoxPtr_ -> findText(options.timeAndDateFormat);
        if (index != -1)
        {
            // Format found 
            timeAndDateFormatComboBoxPtr_ -> setCurrentIndex(index);
        }
        else
        {
            // Format not found
            timeAndDateFormatComboBoxPtr_ -> setCurrentIndex(1);
        }
    }


    AutoNamingOptions AutoNamingDialog::getOptionsFromDialog()
    {
        AutoNamingOptions options;

        if (numberOfCameras_ > 1)
        {
            options.includeCameraIdentifier = true;
        }
        else
        {
            options.includeCameraIdentifier = cameraIdentifierGroupBoxPtr_ -> isChecked();
        }

        if (guidIdentifierRadioButtonPtr_ -> isChecked())
        {
            options.cameraIdentifier = AutoNamingOptions::GUID_IDENTIFIER;
        }
        else
        {
            options.cameraIdentifier = AutoNamingOptions::CAMERA_NUMBER_IDENTIFIER;
        }

        options.includeTimeAndDate = timeAndDateGroupBoxPtr_ -> isChecked();
        options.timeAndDateFormat = timeAndDateFormatComboBoxPtr_ -> currentText();

        return options;
    }

}
