#include "format7_settings_dialog.hpp"
#include "validators.hpp"
#include "lockable.hpp"
#include <algorithm>
#include <QGraphicsScene>
#include <QPointer>
#include <iostream>

namespace bias 
{
    const unsigned int sliderSingleStep = 1;
    const unsigned int sliderPageStep = 10;

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


    bool Format7SettingsDialog::isRoiOffChecked()
    {
        return roiOffRadioButtonPtr_ -> isChecked();
    }


    bool Format7SettingsDialog::isRoiShowChecked()
    {
        return roiShowRadioButtonPtr_ -> isChecked();
    }


    bool Format7SettingsDialog::isRoiEnableChecked()
    {
        return roiEnableRadioButtonPtr_ -> isChecked();
    }


    int Format7SettingsDialog::getRoiXOffset()
    {
        return settings_.offsetX;
    }


    int Format7SettingsDialog::getRoiYOffset()
    {
        return settings_.offsetY;
    }


    int Format7SettingsDialog::getRoiXWidth()
    {
        return settings_.width;
    }


    int Format7SettingsDialog::getRoiYHeight()
    {
        return settings_.height;
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
        int adjustedValue = value*info_.offsetHStepSize;  
        settings_.offsetX = adjustedValue;
        roiXOffsetLineEditPtr_ -> setText(QString::number(adjustedValue));
        adjustOnXOffsetChange();
    }


    void Format7SettingsDialog::roiYOffsetSliderChanged(int value)
    {
        int adjustedValue = value*info_.offsetVStepSize; 
        settings_.offsetY = adjustedValue;
        roiYOffsetLineEditPtr_ -> setText(QString::number(adjustedValue));
        adjustOnYOffsetChange();
    }


    void Format7SettingsDialog::roiXWidthSliderChanged(int value)
    {
        int adjustedValue = value*info_.imageHStepSize;
        settings_.width = adjustedValue;
        roiXWidthLineEditPtr_ -> setText(QString::number(adjustedValue));
        adjustOnXWidthChange();
    }


    void Format7SettingsDialog::roiYHeightSliderChanged(int value)
    {
        int adjustedValue = value*info_.imageVStepSize;
        settings_.height = adjustedValue;
        roiYHeightLineEditPtr_ -> setText(QString::number(adjustedValue));
        adjustOnYHeightChange();
    }


    void Format7SettingsDialog::roiXOffsetEditingFinished()
    {
        QString valueString = roiXOffsetLineEditPtr_ -> text();
        int value = valueString.toInt();
        roiXOffsetSliderPtr_ -> blockSignals(true);
        roiXOffsetSliderPtr_ -> setValue(value);
        roiXOffsetSliderPtr_ -> blockSignals(false);
        adjustOnXOffsetChange();
    }


    void Format7SettingsDialog::roiYOffsetEditingFinished()
    {
        QString valueString = roiYOffsetLineEditPtr_ -> text();
        int value = valueString.toInt();
        roiYOffsetSliderPtr_ -> blockSignals(true);
        roiYOffsetSliderPtr_ -> setValue(value);
        roiYOffsetSliderPtr_ -> blockSignals(false);
        adjustOnYOffsetChange();
    }


    void Format7SettingsDialog::roiXWidthEditingFinished()
    {
        QString valueString = roiXWidthLineEditPtr_ -> text();
        int value = valueString.toInt();
        roiXWidthSliderPtr_ -> blockSignals(true);
        roiXWidthSliderPtr_ -> setValue(value);
        roiXWidthSliderPtr_ -> blockSignals(false);
        adjustOnXWidthChange();
    }


    void Format7SettingsDialog::roiYHeightEditingFinished()
    {
        QString valueString = roiYHeightLineEditPtr_ -> text();
        int value = valueString.toInt();
        roiYHeightSliderPtr_ -> blockSignals(true);
        roiYHeightSliderPtr_ -> setValue(value);
        roiYHeightSliderPtr_ -> blockSignals(false);
        adjustOnYHeightChange();
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


    void Format7SettingsDialog::roiMaxSizePushButtonClicked()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
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
        changed_ = false;


        if (cameraPtr_ == NULL)
        {
            return;
        }

        cameraPtr_ -> acquireLock();
        settings_ = cameraPtr_ -> getFormat7Settings();
        info_ = cameraPtr_ -> getFormat7Info(settings_.mode);
        ImageModeList modeList = cameraPtr_ -> getListOfSupportedImageModes();
        PixelFormatList formatList = cameraPtr_ -> getListOfSupportedPixelFormats(
                settings_.mode
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
        QString xOffsetStepText = QString("Step = %1").arg(info_.offsetHStepSize);
        roiXOffsetStepLabelPtr_ -> setText(xOffsetStepText);

        QString yOffsetStepText = QString("Step = %1").arg(info_.offsetVStepSize); 
        roiYOffsetStepLabelPtr_ -> setText(yOffsetStepText);

        QString xWidthStepText = QString("Step = %1").arg(info_.imageHStepSize);
        roiXWidthStepLabelPtr_ -> setText(xWidthStepText);

        QString yHeightInfoText = QString("Step = %1").arg(info_.imageVStepSize); 
        roiYHeightStepLabelPtr_ -> setText(yHeightInfoText);

        setRanges();

        // Set Slider values
        roiXOffsetSliderPtr_ -> setValue(settings_.offsetX/info_.offsetHStepSize);
        roiYOffsetSliderPtr_ -> setValue(settings_.offsetY/info_.offsetVStepSize);
        roiXWidthSliderPtr_ -> setValue(settings_.width/info_.imageHStepSize);
        roiYHeightSliderPtr_ -> setValue(settings_.height/info_.imageVStepSize);

        // Set line edit values
        roiXOffsetLineEditPtr_ -> setText(QString::number(settings_.offsetX));
        roiYOffsetLineEditPtr_ -> setText(QString::number(settings_.offsetY));
        roiXWidthLineEditPtr_ -> setText(QString::number(settings_.width));
        roiYHeightLineEditPtr_ -> setText(QString::number(settings_.height));

        connectWidgets();

        // TEMP DEBUG
        // ----------------------------------
        std::cout << std::endl;
        std::cout << "settings" << std::endl;
        settings_.print();
        std::cout << std::endl;
        std::cout << "info" << std::endl;
        info_.print();
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

        connect(
                roiMaxSizePushButtonPtr_,
                SIGNAL(clicked()),
                this,
                SLOT(roiMaxSizePushButtonClicked())
               );
    }


    void Format7SettingsDialog::setRanges()
    {
        QPointer<IntValidatorWithFixup> validatorPtr;
        int bottom;
        int top;
        int step;

        // Set fixup and slider range for x offset
        bottom = 0;
        top = info_.maxWidth - info_.imageHStepSize;
        step = info_.offsetHStepSize;
        validatorPtr = new IntValidatorWithFixup(roiXOffsetLineEditPtr_);
        validatorPtr -> setRange(bottom,top);
        roiXOffsetLineEditPtr_ -> setValidator(validatorPtr);
        roiXOffsetSliderPtr_ -> setRange(bottom/step, top/step);
        roiXOffsetSliderPtr_ -> setSingleStep(sliderSingleStep);
        roiXOffsetSliderPtr_ -> setPageStep(sliderPageStep);

        // Set fixup and slider range for y offset 
        bottom = 0;
        top = info_.maxHeight - info_.imageVStepSize;
        step = info_.offsetVStepSize;
        validatorPtr = new IntValidatorWithFixup(roiYOffsetLineEditPtr_);
        validatorPtr -> setRange(bottom,top);
        roiYOffsetLineEditPtr_ -> setValidator(validatorPtr);
        roiYOffsetSliderPtr_ -> setRange(bottom/step, top/step);
        roiXOffsetSliderPtr_ -> setSingleStep(sliderSingleStep);
        roiXOffsetSliderPtr_ -> setPageStep(sliderPageStep);

        // Set fixup and slider range for x width 
        bottom = info_.imageHStepSize;
        top = info_.maxWidth;
        step = info_.imageHStepSize;
        validatorPtr = new IntValidatorWithFixup(roiXWidthLineEditPtr_);
        validatorPtr -> setRange(bottom, top);
        roiXWidthLineEditPtr_ -> setValidator(validatorPtr);
        roiXWidthSliderPtr_ -> setRange(bottom/step, top/step);
        roiXWidthSliderPtr_ -> setSingleStep(sliderSingleStep);
        roiXWidthSliderPtr_ -> setPageStep(sliderPageStep);

        // Set fixup and slider range for y height 
        bottom = info_.imageVStepSize;
        top = info_.maxHeight;
        step = info_.imageVStepSize;
        validatorPtr = new IntValidatorWithFixup(roiYHeightLineEditPtr_);
        validatorPtr -> setRange(bottom,top);
        roiYHeightLineEditPtr_ -> setValidator(validatorPtr);
        roiYHeightSliderPtr_ -> setRange(bottom/step, top/step);
        roiYHeightSliderPtr_ -> setSingleStep(sliderSingleStep);
        roiYHeightSliderPtr_ -> setPageStep(sliderPageStep);
        
    }


    void Format7SettingsDialog::adjustOnXOffsetChange()
    {
        while ((settings_.width + settings_.offsetX) > info_.maxWidth)
        {
            settings_.width -= info_.imageHStepSize;
        }
        roiXWidthSliderPtr_ -> blockSignals(true);
        roiXWidthSliderPtr_ -> setValue(settings_.width/info_.imageHStepSize);
        roiXWidthSliderPtr_ -> blockSignals(false);
        roiXWidthLineEditPtr_ -> setText(QString::number(settings_.width));
    }


    void  Format7SettingsDialog::adjustOnYOffsetChange()
    {
        while ((settings_.height + settings_.offsetY) > info_.maxHeight)
        {
            settings_.height -= info_.imageVStepSize;
        }
        roiYHeightSliderPtr_ -> blockSignals(true);
        roiYHeightSliderPtr_ -> setValue(settings_.height/info_.imageVStepSize);
        roiYHeightSliderPtr_ -> blockSignals(false);
        roiYHeightLineEditPtr_ -> setText(QString::number(settings_.height));
    }


    void Format7SettingsDialog::adjustOnXWidthChange()
    {
        while ((settings_.width + settings_.offsetX) > info_.maxWidth)
        {
            settings_.offsetX -= info_.offsetHStepSize;
        }
        roiXOffsetSliderPtr_ -> blockSignals(true);
        roiXOffsetSliderPtr_ -> setValue(settings_.offsetX/info_.offsetHStepSize);
        roiXOffsetSliderPtr_ -> blockSignals(false);
        roiXOffsetLineEditPtr_ -> setText(QString::number(settings_.offsetX));
    }

    void Format7SettingsDialog::adjustOnYHeightChange()
    {
        while((settings_.height + settings_.offsetY) > info_.maxHeight)
        {
            settings_.offsetY -= info_.offsetVStepSize;
        }
        roiYOffsetSliderPtr_ -> blockSignals(true);
        roiYOffsetSliderPtr_ -> setValue(settings_.offsetY/info_.offsetVStepSize);
        roiYOffsetSliderPtr_ -> blockSignals(false);
        roiYOffsetLineEditPtr_ -> setText(QString::number(settings_.offsetY));
    }

} // namespace bias
