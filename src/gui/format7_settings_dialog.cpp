#include "format7_settings_dialog.hpp"
#include "validators.hpp"
#include "lockable.hpp"
#include "camera_window.hpp"
#include <algorithm>
#include <QGraphicsScene>
#include <QPointer>
#include <QMessageBox>
#include <iostream>



namespace bias 
{
    enum ROI_ENABLE_STATE
    {
        ROI_ENABLE_OFF=0,
        ROI_ENABLE_ON,
    };
    const unsigned int SLIDER_SINGLE_STEP = 1;
    const unsigned int SLIDER_PAGE_STEP = 10;
    const int CAMERA_LOCK_TRY_DT = 100;

    // Public methods
    // ----------------------------------------------------------------------------------
    Format7SettingsDialog::Format7SettingsDialog(QWidget *parent) : QDialog(parent)
    {
        cameraPtr_ = NULL;
        initialize(true,true);
    }


    Format7SettingsDialog::Format7SettingsDialog(
            std::shared_ptr<Lockable<Camera>> cameraPtr, 
            bool capturing,
            bool logging,
            QWidget *parent
            ) 
        : QDialog(parent)
    {
        cameraPtr_ = cameraPtr;
        initialize(capturing, logging);
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


    int Format7SettingsDialog::getRoiXWidthMax()
    {
        return info_.maxWidth;
    }


    int Format7SettingsDialog::getRoiYHeightMax()
    {
        return info_.maxHeight;
    }

    // Private slots
    // ----------------------------------------------------------------------------------
    void Format7SettingsDialog::modeComboBoxChanged(int index)
    {
        // ------------------------------------------------------------------------------
        // TO DO 
        // ------------------------------------------------------------------------------
    }


    void Format7SettingsDialog::pixelFormatComboBoxChanged(int index)
    {
        PixelFormat format = formatToIndexMap_.key(index);
        settings_.pixelFormat = format;
        if (isRoiEnableChecked())
        {
            updateFormat7Settings();
           
        }
        else
        {
            updateFormat7SettingsFullSize();
        }
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
        if ((value%info_.offsetHStepSize) != 0)
        {
            int n = value/info_.offsetHStepSize;
            value = (n+1)*info_.offsetHStepSize;
            roiXOffsetLineEditPtr_ -> setText(QString::number(value));
        }
        settings_.offsetX = value;
        roiXOffsetSliderPtr_ -> blockSignals(true);
        roiXOffsetSliderPtr_ -> setValue(value/info_.offsetHStepSize);
        roiXOffsetSliderPtr_ -> blockSignals(false);
        adjustOnXOffsetChange();
    }


    void Format7SettingsDialog::roiYOffsetEditingFinished()
    {
        QString valueString = roiYOffsetLineEditPtr_ -> text();
        int value = valueString.toInt();
        if ((value%info_.offsetVStepSize) != 0)
        {
            int n = value/info_.offsetVStepSize;
            value = (n+1)*info_.offsetVStepSize;
            roiYOffsetLineEditPtr_ -> setText(QString::number(value));
        }
        settings_.offsetY = value;
        roiYOffsetSliderPtr_ -> blockSignals(true);
        roiYOffsetSliderPtr_ -> setValue(value/info_.offsetVStepSize);
        roiYOffsetSliderPtr_ -> blockSignals(false);
        adjustOnYOffsetChange();
    }


    void Format7SettingsDialog::roiXWidthEditingFinished()
    {
        QString valueString = roiXWidthLineEditPtr_ -> text();
        int value = valueString.toInt();
        if ((value%info_.imageHStepSize) != 0)
        {
            int n = value/info_.imageHStepSize;
            value = (n+1)*info_.imageHStepSize;
            roiXWidthLineEditPtr_ -> setText(QString::number(value));
        }
        settings_.width = value;
        roiXWidthSliderPtr_ -> blockSignals(true);
        roiXWidthSliderPtr_ -> setValue(value/info_.imageHStepSize);
        roiXWidthSliderPtr_ -> blockSignals(false);
        adjustOnXWidthChange();
    }


    void Format7SettingsDialog::roiYHeightEditingFinished()
    {
        QString valueString = roiYHeightLineEditPtr_ -> text();
        int value = valueString.toInt();
        if ((value%info_.imageVStepSize) != 0) 
        {
            int n = value/info_.imageVStepSize;
            value = (n+1)*info_.imageVStepSize;
            roiYHeightLineEditPtr_ -> setText(QString::number(value));
        }
        settings_.height = value;
        roiYHeightSliderPtr_ -> blockSignals(true);
        roiYHeightSliderPtr_ -> setValue(value/info_.imageVStepSize);
        roiYHeightSliderPtr_ -> blockSignals(false);
        adjustOnYHeightChange();
    }


    void Format7SettingsDialog::roiOffRadioButtonChanged(bool checked)
    {
        if (checked)
        {
            updateFormat7SettingsFullSize();
            setRoiControlsEnabled(true);
            if (lastRoiEnableState_ == ROI_ENABLE_ON)
            {
                emit roiEnableStateChanged();
                lastRoiEnableState_ = ROI_ENABLE_OFF;
            }
        }
    }


    void Format7SettingsDialog::roiShowRadioButtonChanged(bool checked)
    {
        if (checked) 
        {
            updateFormat7SettingsFullSize();
            setRoiControlsEnabled(true);
            if (lastRoiEnableState_ == ROI_ENABLE_ON)
            {
                emit roiEnableStateChanged();
                lastRoiEnableState_ = ROI_ENABLE_OFF;
            }
        }
    }



    void Format7SettingsDialog::roiEnableRadioButtonChanged(bool checked)
    {
        if (checked)
        {
            updateFormat7Settings();
            setRoiControlsEnabled(false);
            if (lastRoiEnableState_ == ROI_ENABLE_OFF)
            {
                emit roiEnableStateChanged();
                lastRoiEnableState_ = ROI_ENABLE_ON;
            }
        }
    }


    void Format7SettingsDialog::roiMaxSizePushButtonClicked()
    {
        settings_.offsetX = 0;
        settings_.offsetY = 0;
        settings_.width = info_.maxWidth;
        settings_.height = info_.maxHeight;
        updateSlidersAndLineEdits();
    }


    void Format7SettingsDialog::imageCaptureStarted(bool logging)
    {
        imageParamGroupBoxPtr_ -> setEnabled(false);
        if (logging)
        {
            roiGroupBoxPtr_ -> setEnabled(false);
            if (isRoiShowChecked())
            {
                roiShowRadioButtonPtr_ -> setChecked(false);
                roiOffRadioButtonPtr_ -> setChecked(true);
            }

        }
    }


    void Format7SettingsDialog::imageCaptureStopped()
    { 
        imageParamGroupBoxPtr_ -> setEnabled(true); 
        roiGroupBoxPtr_ -> setEnabled(true);
    }


    void Format7SettingsDialog::format7SettingsChanged()
    {
        QPointer<CameraWindow> cameraWindowPtr = qobject_cast<CameraWindow *>(parent());
        if (cameraWindowPtr.isNull())
        {
            return;
        }
        bool capturing = cameraWindowPtr -> isCapturing();
        bool logging = cameraWindowPtr -> isLoggingEnabled();
        updateWidgets(capturing,logging);
    }

    
    // Private methods
    // ----------------------------------------------------------------------------------
    void Format7SettingsDialog::initialize(bool capturing, bool logging)
    {
        setupUi(this);
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        updateWidgets(capturing,logging);
        connectWidgets();
    } 
    
    void Format7SettingsDialog::updateWidgets(bool capturing, bool logging)
    {
        if (cameraPtr_ == NULL)
        {
            return;
        }

        // Get current settings from Camera
        getFormat7SettingsAndInfo();

        // Setup mode list comboBox
        int index = 0;
        ImageModeList::iterator modeIt;
        for (modeIt=modeList_.begin(); modeIt!=modeList_.end(); modeIt++)
        {
            ImageMode mode = *modeIt;
            std::string modeStdString  = getImageModeString(mode);
            modeComboBoxPtr_ -> addItem(QString::fromStdString(modeStdString));
            modeToIndexMap_[mode] = index;
            index++;
        }

        // Setup pixel format comboBox
        index = 0;
        PixelFormatList::iterator formatIt;
        for (formatIt=formatList_.begin(); formatIt!=formatList_.end(); formatIt++)
        {
            PixelFormat format = *formatIt;
            std::string formatStdString = getPixelFormatString(format);
            // --------------------------------------------------------------------------
            // TEMPORARY - only allow RAW8, MONO8, RGB8
            // --------------------------------------------------------------------------
            if ( 
                    (format == PIXEL_FORMAT_RAW8)  || 
                    (format == PIXEL_FORMAT_MONO8) || 
                    (format == PIXEL_FORMAT_RGB8)
                ) 
            { 
                pixelFormatComboBoxPtr_ -> addItem(QString::fromStdString(formatStdString));
                formatToIndexMap_[format] = index;
                index++;
            }
            // --------------------------------------------------------------------------
            //pixelFormatComboBoxPtr_ -> addItem(QString::fromStdString(formatStdString));
            //formatToIndexMap_[index] = format;
            //index++;
        }

        // Set combobox indices
        setModeComboBoxIndex();
        setPixelFormatComboBoxIndex();

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
        updateSlidersAndLineEdits();

        // Set enable values for group boxes
        if (capturing)
        {
            imageParamGroupBoxPtr_ -> setEnabled(false);
        }
        else
        {
            imageParamGroupBoxPtr_ -> setEnabled(true);
        }

        if (capturing && logging)
        {
            roiGroupBoxPtr_ -> setEnabled(false);
        }
        else
        {
            roiGroupBoxPtr_ -> setEnabled(true);
        }

        // Set radio button checked based whether or not ROI is enabled.
        updateRoiRadioButtons();

        // ------------------------------------------------------------------------------
        // TEMPORARY - only allow mode 0 (disable mode setting)
        // ------------------------------------------------------------------------------
        modeComboBoxPtr_ -> setEnabled(false);
        // ------------------------------------------------------------------------------

    }


    void Format7SettingsDialog::getFormat7SettingsAndInfo()
    {
        // Reads format7 settings and infomartion from camera
        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            settings_ = cameraPtr_ -> getFormat7Settings();
            info_ = cameraPtr_ -> getFormat7Info(settings_.mode);
            modeList_ = cameraPtr_ -> getListOfSupportedImageModes();
            formatList_ = cameraPtr_ -> getListOfSupportedPixelFormats(settings_.mode);
            cameraPtr_ -> releaseLock();
        }
        else
        {
            QString msgText("unable to retrieve settings/info from camera");
            cameraLockFailErrMsg(msgText);
        }
    }


    void Format7SettingsDialog::updateSlidersAndLineEdits()
    {
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
    } 
    

    void Format7SettingsDialog::updateRoiRadioButtons()
    {
        if (isRoiMaxSize())
        {
            roiOffRadioButtonPtr_ -> setChecked(true);
            roiShowRadioButtonPtr_ -> setChecked(false);
            roiEnableRadioButtonPtr_ -> setChecked(false);
            setRoiControlsEnabled(true);
            lastRoiEnableState_ = ROI_ENABLE_OFF;
        }
        else 
        {
            roiOffRadioButtonPtr_ -> setChecked(false);
            roiShowRadioButtonPtr_ -> setChecked(false);
            roiEnableRadioButtonPtr_ -> setChecked(true);
            setRoiControlsEnabled(false);
            lastRoiEnableState_ = ROI_ENABLE_ON;
        }
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

        connect(
                parent(),
                SIGNAL(format7SettingsChanged()),
                this,
                SLOT(format7SettingsChanged())
               );
    }


    void Format7SettingsDialog::setModeComboBoxIndex()
    {
        int index = modeToIndexMap_[settings_.mode];
        modeComboBoxPtr_ -> setCurrentIndex(index);
    }


    void Format7SettingsDialog::setPixelFormatComboBoxIndex()
    {
        int index = formatToIndexMap_[settings_.pixelFormat];
        pixelFormatComboBoxPtr_ -> setCurrentIndex(index);
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
        roiXOffsetSliderPtr_ -> setSingleStep(SLIDER_SINGLE_STEP);
        roiXOffsetSliderPtr_ -> setPageStep(SLIDER_PAGE_STEP);

        // Set fixup and slider range for y offset 
        bottom = 0;
        top = info_.maxHeight - info_.imageVStepSize;
        step = info_.offsetVStepSize;
        validatorPtr = new IntValidatorWithFixup(roiYOffsetLineEditPtr_);
        validatorPtr -> setRange(bottom,top);
        roiYOffsetLineEditPtr_ -> setValidator(validatorPtr);
        roiYOffsetSliderPtr_ -> setRange(bottom/step, top/step);
        roiXOffsetSliderPtr_ -> setSingleStep(SLIDER_SINGLE_STEP);
        roiXOffsetSliderPtr_ -> setPageStep(SLIDER_PAGE_STEP);

        // Set fixup and slider range for x width 
        bottom = info_.imageHStepSize;
        top = info_.maxWidth;
        step = info_.imageHStepSize;
        validatorPtr = new IntValidatorWithFixup(roiXWidthLineEditPtr_);
        validatorPtr -> setRange(bottom, top);
        roiXWidthLineEditPtr_ -> setValidator(validatorPtr);
        roiXWidthSliderPtr_ -> setRange(bottom/step, top/step);
        roiXWidthSliderPtr_ -> setSingleStep(SLIDER_SINGLE_STEP);
        roiXWidthSliderPtr_ -> setPageStep(SLIDER_PAGE_STEP);

        // Set fixup and slider range for y height 
        bottom = info_.imageVStepSize;
        top = info_.maxHeight;
        step = info_.imageVStepSize;
        validatorPtr = new IntValidatorWithFixup(roiYHeightLineEditPtr_);
        validatorPtr -> setRange(bottom,top);
        roiYHeightLineEditPtr_ -> setValidator(validatorPtr);
        roiYHeightSliderPtr_ -> setRange(bottom/step, top/step);
        roiYHeightSliderPtr_ -> setSingleStep(SLIDER_SINGLE_STEP);
        roiYHeightSliderPtr_ -> setPageStep(SLIDER_PAGE_STEP);
        
    }


    void Format7SettingsDialog::setFormat7Settings(
            Format7Settings settings, 
            float percentSpeed
            )
    { 
        bool error = false;
        unsigned int errorId;
        QString errorMsg;
        bool settingsAreValid;

        QPointer<CameraWindow> cameraWindowPtr = qobject_cast<CameraWindow *>(parent());
        if (cameraWindowPtr.isNull())
        {
            return;
        }

        // Get captureing state so it can be restored
        bool isCapturing = cameraWindowPtr -> isCapturing();
        if (isCapturing) 
        {
            cameraWindowPtr -> stopImageCapture();
        }

        TriggerType triggerType;  
        if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        {
            // Get trigger type so that it can be restored and set to internal trigger
            // NOTE: restoring trigger type isn't working at the moment for some reason.
            triggerType = cameraPtr_ -> getTriggerType();
            cameraPtr_ -> setTriggerInternal();

            // Check that settings are valid
            try
            {
                settingsAreValid = cameraPtr_ -> validateFormat7Settings(settings);
            }
            catch (RuntimeError &runtimeError)
            {
                error = true;
                errorId = runtimeError.id();
                errorMsg = QString::fromStdString(runtimeError.what());
            }
            cameraPtr_ -> releaseLock();
        }
        else
        {
            QString msgText("unable to validate format7 settings");
            cameraLockFailErrMsg(msgText);
            return;
        }

        if (error)
        {
            QString msgTitle("Camera Query Error");
            QString msgText("Failed to validate format7 settings:");
            msgText += QString("\n\nError ID: ") + QString::number(errorId);
            msgText += "\n\n";
            msgText += errorMsg;
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }

        if (settingsAreValid)
        {
            if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
            {
                try
                {
                    cameraPtr_ -> setFormat7Configuration(settings, percentSpeed);
                }
                catch (RuntimeError &runtimeError)
                {
                    error = true;
                    errorId = runtimeError.id();
                    errorMsg = QString::fromStdString(runtimeError.what());
                }
                cameraPtr_ -> releaseLock();
            }
            else
            {
                QString msgText("unable to set format7 configuration");
                cameraLockFailErrMsg(msgText);
            }
            
            if (error)
            {
                QString msgTitle("Set Format7 Error");
                QString msgText("Failed to set format7 configuration:");
                msgText += QString("\n\nError ID: ") + QString::number(errorId);
                msgText += "\n\n";
                msgText += errorMsg;
                QMessageBox::critical(this, msgTitle, msgText);
                return;
            }
        }
        else
        { 
            QString msgTitle("Set Format7 Error");
            QString msgText("Format7 settings invalid");
            QMessageBox::critical(this, msgTitle, msgText);
            return;
        }


        //// Restore trigger type - this doesn't work...
        //if (cameraPtr_ -> tryLock(CAMERA_LOCK_TRY_DT))
        //{
        //    cameraPtr_ -> setTriggerType(triggerType);
        //}
        //else
        //{
        //    QString msgText("unable to restore trigger type");
        //    cameraLockFailErrMsg(msgText);
        //}

        // Restore capturing state
        if (isCapturing)
        {
            cameraWindowPtr -> startImageCapture();
        }
    }


    void Format7SettingsDialog::updateFormat7Settings()
    {
        QPointer<CameraWindow> cameraWindowPtr = qobject_cast<CameraWindow *>(parent());
        if (cameraWindowPtr.isNull())
        {
            return;
        }
        float percentSpeed = cameraWindowPtr -> getFormat7PercentSpeed();
        setFormat7Settings(settings_,percentSpeed);
    }


    void Format7SettingsDialog::updateFormat7SettingsFullSize()
    { 
        Format7Settings settingsFullSize = settings_;
        settingsFullSize.offsetX = 0;
        settingsFullSize.offsetY = 0;
        settingsFullSize.width = info_.maxWidth;
        settingsFullSize.height = info_.maxHeight;
        setFormat7Settings(settingsFullSize);
    }

    
    void Format7SettingsDialog::setRoiControlsEnabled(bool value)
    {
        roiXOffsetWidgetPtr_ -> setEnabled(value);
        roiYOffsetWidgetPtr_ -> setEnabled(value);
        roiXWidthWidgetPtr_ -> setEnabled(value);
        roiYHeightWidgetPtr_ -> setEnabled(value);
        roiMaxSizePushButtonPtr_ -> setEnabled(value);
        if (value)
        {
            roiInfoLabelPtr_ -> setText(QString(""));
        }
        else
        {
            roiInfoLabelPtr_ -> setText(QString("To Adjust ROI set to off or show"));
        }
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


    void Format7SettingsDialog::adjustOnYOffsetChange()
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


    bool Format7SettingsDialog::isRoiMaxSize()
    {
        return ((settings_.width == info_.maxWidth) && (settings_.height == info_.maxHeight));
    }


    void Format7SettingsDialog::cameraLockFailErrMsg(QString msg)
    {
        QString msgTitle("Format7 Dialog Lock Error");
        QString msgMod = msg + QString(" - failed to acquire camera lock");
        QMessageBox::critical(this, msgTitle, msgMod);
    }

} // namespace bias
