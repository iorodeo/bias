#include "format7_settings_dialog.hpp"
#include "validators.hpp"
#include "camera_facade.hpp"
#include "lockable.hpp"
#include <algorithm>
#include <QGraphicsScene>
#include <QPointer>
#include <iostream>

namespace bias 
{
    // Public methods
    // ----------------------------------------------------------------------------------
    Format7SettingsDialog::Format7SettingsDialog(QWidget *parent) : QDialog(parent)
    {
        cameraPtr_ = NULL;
        initialize();
    }

    Format7SettingsDialog::Format7SettingsDialog(
            std::shared_ptr<Lockable<Camera>> cameraPtr, 
            QWidget *parent
            ) 
        : QDialog(parent)
    {
        cameraPtr_ = cameraPtr;
        initialize();
    }

    // Private slots
    // ----------------------------------------------------------------------------------
    void Format7SettingsDialog::modeComboBoxChanged(int index)
    {
        std::cout << __PRETTY_FUNCTION__ << ", index = " << index << std::endl;
    }


    void Format7SettingsDialog::pixelFormatComboBoxChanged(int index)
    {
        std::cout << __PRETTY_FUNCTION__ << ", index = " << index << std::endl;
    }


    void Format7SettingsDialog::roiXOffsetSliderChanged(int value)
    {
        roiXOffsetLineEditPtr_ -> setText(QString::number(value));
        std::cout << __PRETTY_FUNCTION__ << ", value = " << value << std::endl;
    }


    void Format7SettingsDialog::roiYOffsetSliderChanged(int value)
    {
        roiYOffsetLineEditPtr_ -> setText(QString::number(value));
        std::cout << __PRETTY_FUNCTION__ << ", value = " << value << std::endl;
    }


    void Format7SettingsDialog::roiXWidthSliderChanged(int value)
    {
        roiXWidthLineEditPtr_ -> setText(QString::number(value));
        std::cout << __PRETTY_FUNCTION__ << ", value = " << value << std::endl;
    }


    void Format7SettingsDialog::roiYHeightSliderChanged(int value)
    {
        roiYHeightLineEditPtr_ -> setText(QString::number(value));
        std::cout << __PRETTY_FUNCTION__ << ", value = " << value << std::endl;
    }


    void Format7SettingsDialog::roiXOffsetEditingFinished()
    {
        QString valueString = roiXOffsetLineEditPtr_ -> text();
        int value = valueString.toInt();
        roiYOffsetSliderPtr_ -> setValue(value);
        std::cout << __PRETTY_FUNCTION__ << ", value = " << value << std::endl;
    }


    void Format7SettingsDialog::roiYOffsetEditingFinished()
    {
        QString valueString = roiYOffsetLineEditPtr_ -> text();
        int value = valueString.toInt();
        roiYOffsetSliderPtr_ -> setValue(value);
        std::cout << __PRETTY_FUNCTION__ << ", value = " << value << std::endl;
    }


    void Format7SettingsDialog::roiXWidthEditingFinished()
    {
        QString valueString = roiXWidthLineEditPtr_ -> text();
        int value = valueString.toInt();
        roiXWidthSliderPtr_ -> setValue(value);
        std::cout << __PRETTY_FUNCTION__ << ", value = " << value << std::endl;
    }


    void Format7SettingsDialog::roiYHeightEditingFinished()
    {
        QString valueString = roiYHeightLineEditPtr_ -> text();
        int value = valueString.toInt();
        roiYHeightSliderPtr_ -> setValue(value);
        std::cout << __PRETTY_FUNCTION__ << ", value = " << value << std::endl;
    }


    void Format7SettingsDialog::roiOffRadioButtonChanged(bool checked)
    {
        if (checked)
        {
            std::cout << __PRETTY_FUNCTION__ << ", " << checked << std::endl; 
        }
    }


    void Format7SettingsDialog::roiShowRadioButtonChanged(bool checked)
    {
        if (checked) 
        {
            std::cout << __PRETTY_FUNCTION__ << ", " << checked << std::endl;
        }
    }



    void Format7SettingsDialog::roiEnableRadioButtonChanged(bool checked)
    {
        if (checked)
        {
            std::cout << __PRETTY_FUNCTION__ << ", " << checked << std::endl; 
        }
    }

    
    // Private methods
    // ----------------------------------------------------------------------------------
    void Format7SettingsDialog::initialize()
    {
        setupUi(this);
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

        roiOffRadioButtonPtr_ -> setChecked(true);
        roiShowRadioButtonPtr_ -> setChecked(false);
        roiEnableRadioButtonPtr_ -> setChecked(false);

        connectWidgets();

        cameraPtr_ -> acquireLock();
        Format7Settings settings = cameraPtr_ -> getFormat7Settings();
        Format7Info info = cameraPtr_ -> getFormat7Info(settings.mode);
        ImageModeList modeList = cameraPtr_ -> getListOfSupportedImageModes();
        PixelFormatList formatList = cameraPtr_ -> getListOfSupportedPixelFormats(
                settings.mode
                );
        cameraPtr_ -> releaseLock();

        // Setup mode list comboBox
        ImageModeList::iterator modeIt;
        for (modeIt=modeList.begin(); modeIt!=modeList.end(); modeIt++)
        {
            ImageMode mode = *modeIt;
            std::string modeStdString  = getImageModeString(mode);
            modeComboBoxPtr_ -> addItem(QString::fromStdString(modeStdString));
        }

        // Setup pixel format comboBox
        PixelFormatList::iterator formatIt;
        for (formatIt=formatList.begin(); formatIt!=formatList.end(); formatIt++)
        {
            PixelFormat format = *formatIt;
            std::string formatStdString = getPixelFormatString(format);
            pixelFormatComboBoxPtr_ -> addItem(QString::fromStdString(formatStdString));
        }

        // Set ROI Step labels
        QString xOffsetStepText = QString("Step = %1").arg(info.offsetHStepSize);
        roiXOffsetStepLabelPtr_ -> setText(xOffsetStepText);

        QString yOffsetStepText = QString("Step = %1").arg(info.offsetVStepSize);
        roiYOffsetStepLabelPtr_ -> setText(yOffsetStepText);

        QString xWidthStepText = QString("Step = %1").arg(info.imageHStepSize);
        roiXWidthStepLabelPtr_ -> setText(xWidthStepText);

        QString yHeightStepText = QString("Step = %1").arg(info.imageVStepSize);
        roiYHeightStepLabelPtr_ -> setText(yHeightStepText);


        setupLineEditValidators(settings, info);

        // TEMP DEBUG
        // ----------------------------------
        std::cout << std::endl;
        std::cout << "settings" << std::endl;
        settings.print();
        std::cout << std::endl;
        std::cout << "info" << std::endl;
        info.print();
        // ----------------------------------
    }

    void Format7SettingsDialog::connectWidgets()
    {
        connect(
                modeComboBoxPtr_,
                SIGNAL(currentIndexChanged(int)),
                this,
                SLOT(modeComboBoxChanged(int))
               );

        connect(
                pixelFormatComboBoxPtr_,
                SIGNAL(currentIndexChanged(int)),
                this,
                SLOT(pixelFormatComboBoxChanged(int))
               );

        connect(
                roiXOffsetSliderPtr_,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(roiXOffsetSliderChanged(int))
               );

        connect(
                roiYOffsetSliderPtr_,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(roiYOffsetSliderChanged(int))
               );

        connect(
                roiXWidthSliderPtr_,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(roiXWidthSliderChanged(int))
               );

        connect(
                roiYHeightSliderPtr_,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(roiYHeightSliderChanged(int))
               );

        connect(
                roiXOffsetLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(roiXOffsetEditingFinished())
               );

        connect(
                roiYOffsetLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(roiYOffsetEditingFinished())
               );

        connect(
                roiXWidthLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(roiXWidthEditingFinished())
               );

        connect(
                roiYHeightLineEditPtr_,
                SIGNAL(editingFinished()),
                this,
                SLOT(roiYHeightEditingFinished())
               );

        connect(
                roiOffRadioButtonPtr_, 
                SIGNAL(toggled(bool)),
                this,
                SLOT(roiOffRadioButtonChanged(bool))
               );

        connect(
                roiShowRadioButtonPtr_,
                SIGNAL(toggled(bool)),
                this,
                SLOT(roiShowRadioButtonChanged(bool))
               );

        connect(
                roiEnableRadioButtonPtr_,
                SIGNAL(toggled(bool)),
                this,
                SLOT(roiEnableRadioButtonChanged(bool))
               );
    }


    void Format7SettingsDialog::setupLineEditValidators(
            Format7Settings settings, 
            Format7Info info
            )
    {
        QPointer<IntValidatorWithFixup> validatorPtr;

        // Fixup for x offset lineEdit 
        validatorPtr = new IntValidatorWithFixup(roiXOffsetLineEditPtr_);
        validatorPtr -> setBottom(0);
        validatorPtr -> setTop(100);
        roiXOffsetLineEditPtr_ -> setValidator(validatorPtr);

        // Fixup for y offset lineEdit
        validatorPtr = new IntValidatorWithFixup(roiYOffsetLineEditPtr_);
        validatorPtr -> setBottom(0);
        validatorPtr -> setTop(100);
        roiXOffsetLineEditPtr_ -> setValidator(validatorPtr);

        // Fixup for x width lineEdit
        validatorPtr = new IntValidatorWithFixup(roiXWidthLineEditPtr_);
        validatorPtr -> setBottom(0);
        validatorPtr -> setTop(100);
        roiXWidthLineEditPtr_ -> setValidator(validatorPtr);

        // Fixup for y height lineEdit
        validatorPtr = new IntValidatorWithFixup(roiYHeightLineEditPtr_);
        validatorPtr -> setBottom(0);
        validatorPtr -> setTop(100);
        roiXWidthLineEditPtr_ -> setValidator(validatorPtr);
        
    }

    

} // namespace bias
