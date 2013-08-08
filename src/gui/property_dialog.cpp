#include "property_dialog.hpp"
#include "camera_facade.hpp"
#include "lockable.hpp"
#include "validators.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <QString>
#include <QTimer>

namespace bias
{

    const int REFRESH_TIMER_INTERVAL_MS = 1000;
    const float ABSOLUTE_VALUE_SCALE = 100.0;
    const unsigned int FLOAT_PRECISION = 2;

    // PropertyDialog methods
    // ------------------------------------------------------------------------

    PropertyDialog::PropertyDialog(QWidget *parent, Qt::WindowFlags f) 
        : QDialog(parent,f)
    {
        cameraPtr_ = NULL;
        propertyType_ = PROPERTY_TYPE_UNSPECIFIED;
        initialize();
    }


    PropertyDialog::PropertyDialog(
            std::shared_ptr<Lockable<Camera>> cameraPtr,
            PropertyType propertyType, 
            QWidget *parent,
            Qt::WindowFlags f
            ) 
        : QDialog(parent, f)
    {
        cameraPtr_ = cameraPtr;
        propertyType_ = propertyType;
        initialize();
    }


    void PropertyDialog::onIntValueSliderChanged()
    {
        unsigned int value = (unsigned int)(intValueSliderPtr -> value());
        setPropertyValue(value);
    }


    void PropertyDialog::onIntEditingFinished()
    {
        QString valueString = intValueLineEditPtr -> text();
        unsigned int value = valueString.toUInt();
        setPropertyValue(value);
        startRefreshTimer();
    }


    void PropertyDialog::onAbsValueSliderChanged()
    {
        int absoluteValueInt = absValueSliderPtr -> value();
        float absoluteValue = float(absoluteValueInt)/ABSOLUTE_VALUE_SCALE;
        setPropertyAbsoluteValue(absoluteValue);
    }


    void PropertyDialog::onAbsEditingFinished()
    {
        QString absoluteValueString = absValueLineEditPtr -> text();
        float absoluteValue = absoluteValueString.toFloat();
        setPropertyAbsoluteValue(absoluteValue);
        startRefreshTimer();
    }


    void PropertyDialog::stopRefreshTimer()
    {
        refreshTimerPtr_ -> stop();
    }

    
    void PropertyDialog::startRefreshTimer()
    {
        refreshTimerPtr_ -> start();
    }

    
    void PropertyDialog::onAutoStateChanged(int state)
    {
        Property property = getProperty();
        if (state == Qt::Unchecked)
        {
            property.autoActive = false;
        }
        else
        {
            property.autoActive = true;
        }
        setProperty(property);
    }


    void PropertyDialog::onOffStateChanged(int state)
    {
        Property property = getProperty();
        if (state == Qt::Unchecked)
        {
            property.on = false;
        }
        else
        {
            property.on = true;
        }
        setProperty(property);
    }

    
    void PropertyDialog::onOnePushButtonClicked()
    {
        Property property = getProperty();
        property.onePush = true;
        setProperty(property);
    }

    void PropertyDialog::onRefreshTimer()
    {
        getPropertyAndUpdateDisplay();
    }


    void PropertyDialog::connectWidgets()
    {

        connect(
                intValueSliderPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(onIntValueSliderChanged())
               );

        connect(
                intValueSliderPtr,
                SIGNAL(sliderPressed()),
                this,
                SLOT(stopRefreshTimer())
               );

        connect(
                intValueSliderPtr,
                SIGNAL(sliderReleased()),
                this,
                SLOT(startRefreshTimer())
               );

        connect( 
                absValueSliderPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(onAbsValueSliderChanged())
               );

        connect(
                absValueSliderPtr,
                SIGNAL(sliderPressed()),
                this,
                SLOT(stopRefreshTimer())
               );

        connect(
                absValueSliderPtr,
                SIGNAL(sliderReleased()),
                this,
                SLOT(startRefreshTimer())
               );

        connect(
                intValueLineEditPtr,
                SIGNAL(textEdited(const QString)),
                this,
                SLOT(stopRefreshTimer())
               );

        connect(
                intValueLineEditPtr,
                SIGNAL(editingFinished()),
                this,
                SLOT(onIntEditingFinished())
               );

        connect(
                absValueLineEditPtr,
                SIGNAL(editingFinished()),
                this,
                SLOT(onAbsEditingFinished())
               );

        connect(
                absValueLineEditPtr,
                SIGNAL(textEdited(const QString)),
                this,
                SLOT(stopRefreshTimer())
               );

        connect(
                autoCheckBoxPtr,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(onAutoStateChanged(int))
               );

        connect(
                onCheckBoxPtr,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(onOffStateChanged(int))
               );

        connect(
                onePushPushButtonPtr,
                SIGNAL(clicked()),
                this,
                SLOT(onOnePushButtonClicked())
               );

    }


    void PropertyDialog::initialize()
    {
        setupUi(this);

        setAttribute(Qt::WA_DeleteOnClose);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

        intValueSliderPtr -> setTracking(false);
        absValueSliderPtr -> setTracking(false);

        propertyNameLabelPtr -> setText(
                QString::fromStdString(getPropertyTypeString(propertyType_))
                );

        getPropertyAndUpdateDisplay();

        connectWidgets();

        // Setup refresh timer
        refreshTimerPtr_ = new QTimer(this);
        refreshTimerPtr_ -> setInterval(REFRESH_TIMER_INTERVAL_MS);

        connect(
                refreshTimerPtr_,
                SIGNAL(timeout()),
                this,
                SLOT(onRefreshTimer())
               );

        refreshTimerPtr_ -> start();


    }

        

    void PropertyDialog::updateDisplay(Property property, PropertyInfo propertyInfo)
    {

        // Enable/disable checkboxes and onepush button and set values
        onCheckBoxPtr -> setEnabled(propertyInfo.onOffCapable);
        onCheckBoxPtr -> setChecked(property.on);

        autoCheckBoxPtr -> setEnabled(
                propertyInfo.manualCapable && propertyInfo.autoCapable
                );

        autoCheckBoxPtr -> setChecked(property.autoActive);

        onePushPushButtonPtr -> setEnabled(
                propertyInfo.onePushCapable && (!property.autoActive) 
                );


        // Enable/disable group boxes
        bool intGroupBoxEnabled = propertyInfo.manualCapable && (!property.autoActive);
        intValueGroupBoxPtr -> setEnabled(intGroupBoxEnabled);

        // Get property value, set value AB selection visibility and selection radio button 
        unsigned int value;
        if (property.type == PROPERTY_TYPE_WHITE_BALANCE)
        {
            valueABSelectionWidgetPtr -> setVisible(true);
            valueARadioButtonPtr -> setText("Red");
            valueBRadioButtonPtr -> setText("Blue");
            resize(0,0);

            if ( valueARadioButtonPtr -> isChecked() )
            {
                value = property.valueA;
                
            }
            else if ( valueBRadioButtonPtr -> isChecked() )
            {
                value = property.valueB;
            }
            else
            {
                value = property.valueA;
                valueARadioButtonPtr -> setChecked(true);
                valueBRadioButtonPtr -> setChecked(false);
            }
        }
        else
        {
            value = property.value;
            valueABSelectionWidgetPtr -> setVisible(false);
            resize(0,0);
        }

        bool absGroupBoxEnabled = propertyInfo.manualCapable; 
        absGroupBoxEnabled = absGroupBoxEnabled && propertyInfo.absoluteCapable; 
        absGroupBoxEnabled = absGroupBoxEnabled && (!property.autoActive);
        absValueGroupBoxPtr -> setEnabled(absGroupBoxEnabled);

        // Set integer value slider 
        intValueSliderPtr -> blockSignals(true);
        intValueSliderPtr -> setMinimum(propertyInfo.minValue);
        intValueSliderPtr -> setMaximum(propertyInfo.maxValue);
        intValueSliderPtr -> setValue(value);
        intValueSliderPtr -> setSingleStep(1);
        int deltaMaxMin = propertyInfo.maxValue-propertyInfo.minValue;
        intValueSliderPtr -> setPageStep(std::max(2,deltaMaxMin/10));
        intValueSliderPtr -> blockSignals(false);

        minIntValueLabelPtr -> setText(QString::number(propertyInfo.minValue));
        maxIntValueLabelPtr -> setText(QString::number(propertyInfo.maxValue));

        intValueLineEditPtr -> blockSignals(true);
        intValueLineEditPtr -> setText(QString::number(value));
        if (propertyInfo.manualCapable)
        {
            QPointer<IntValidatorWithFixup> validatorPtr; 
            validatorPtr = new IntValidatorWithFixup(intValueLineEditPtr);
            validatorPtr -> setRange(propertyInfo.minValue, propertyInfo.maxValue);
            intValueLineEditPtr -> setValidator(validatorPtr);
        }
        intValueLineEditPtr -> blockSignals(false);


        // Set absolute value slider
        if (propertyInfo.absoluteCapable) 
        {
            absValueSliderPtr -> blockSignals(true);
            int minAbsValueInt = int(ABSOLUTE_VALUE_SCALE*propertyInfo.minAbsoluteValue);
            int maxAbsValueInt = int(ABSOLUTE_VALUE_SCALE*propertyInfo.maxAbsoluteValue);
            int absValueInt = int(ABSOLUTE_VALUE_SCALE*property.absoluteValue);
            absValueSliderPtr -> setMinimum(minAbsValueInt);
            absValueSliderPtr -> setMaximum(maxAbsValueInt);
            absValueSliderPtr -> setValue(absValueInt);
            deltaMaxMin = maxAbsValueInt - minAbsValueInt;
            absValueSliderPtr -> setSingleStep(std::max(1,deltaMaxMin/100));
            absValueSliderPtr -> setPageStep(std::max(2,deltaMaxMin/10));
            absValueSliderPtr -> blockSignals(false);

            QString minAbsValueString = QString::number(
                    propertyInfo.minAbsoluteValue, 
                    'f', 
                    FLOAT_PRECISION
                    );
            minAbsValueLabelPtr -> setText(minAbsValueString);

            QString maxAbsValueString = QString::number(
                    propertyInfo.maxAbsoluteValue, 
                    'f', 
                    FLOAT_PRECISION
                    );
            maxAbsValueLabelPtr -> setText(maxAbsValueString);

            absValueLineEditPtr -> blockSignals(true);
            QString absValueString = QString::number(
                    property.absoluteValue, 
                    'f', 
                    FLOAT_PRECISION
                    );
            absValueLineEditPtr -> setText(absValueString);
            if (propertyInfo.manualCapable)
            {
                QPointer<DoubleValidatorWithFixup> validatorPtr; 
                validatorPtr = new DoubleValidatorWithFixup(absValueLineEditPtr);
                validatorPtr -> setBottom(propertyInfo.minAbsoluteValue);
                validatorPtr -> setTop(propertyInfo.maxAbsoluteValue);
                absValueLineEditPtr -> setValidator(validatorPtr);
            }
            absValueLineEditPtr -> blockSignals(false);
        }
        else
        {
            absValueSliderPtr -> blockSignals(true);
            absValueSliderPtr -> setMinimum(0);
            absValueSliderPtr -> setMaximum(100);
            absValueSliderPtr -> setValue(0);
            absValueSliderPtr -> blockSignals(false);

            minAbsValueLabelPtr -> setText(QString("NA"));
            maxAbsValueLabelPtr -> setText(QString("NA"));
        }

        // Set units for absoulute value
        if (propertyInfo.haveUnits)
        {
            unitsNameLabelPtr -> setText(QString::fromStdString(propertyInfo.units));
        }
        else
        {
            unitsNameLabelPtr -> setText(QString("NA"));
        }
    }


    void PropertyDialog::getPropertyAndUpdateDisplay()
    {
        if (cameraPtr_ == NULL)
        {
            return;
        }
        cameraPtr_ -> acquireLock();
        Property property = cameraPtr_ -> getProperty(propertyType_);
        PropertyInfo  propertyInfo = cameraPtr_ -> getPropertyInfo(propertyType_);
        cameraPtr_ -> releaseLock();
        updateDisplay(property, propertyInfo);
    }


    void PropertyDialog::setPropertyValue(unsigned int value)
    {
        Property property = getProperty();
        if (property.type == PROPERTY_TYPE_WHITE_BALANCE)
        {
            if (valueARadioButtonPtr -> isChecked())
            {
                property.valueA = value;
            }
            else
            {
                property.valueB = value;
            }
        }
        else
        {
            property.value = value;
        }
        property.absoluteControl = false;
        setProperty(property);
    }


    void PropertyDialog::setPropertyAbsoluteValue(float absoluteValue)
    {
        Property property = getProperty();
        property.absoluteValue = absoluteValue;
        property.absoluteControl = true;
        setProperty(property);
    }


    void PropertyDialog::setProperty(
            Property property
            )
    {
        if (cameraPtr_ == NULL)
        {
            return;
        }

        cameraPtr_ -> acquireLock();
        cameraPtr_ -> setProperty(property);
        Property propertyNew = cameraPtr_ -> getProperty(propertyType_);
        PropertyInfo propertyInfo = cameraPtr_ -> getPropertyInfo(propertyType_);
        cameraPtr_ -> releaseLock();
        updateDisplay(propertyNew,propertyInfo);
    }


    Property PropertyDialog::getProperty()
    {
        if (cameraPtr_ == NULL) 
        { 
            Property dummy;
            return dummy; 
        }
        cameraPtr_ -> acquireLock();
        Property property = cameraPtr_ -> getProperty(propertyType_);
        cameraPtr_ -> releaseLock();
        return property;
    }


    PropertyInfo PropertyDialog::getPropertyInfo()
    {
        if (cameraPtr_ == NULL)
        { 
            PropertyInfo dummy;
            return dummy;
        }
        cameraPtr_ -> acquireLock();
        PropertyInfo propertyInfo = cameraPtr_ -> getPropertyInfo(propertyType_);
        cameraPtr_ -> releaseLock();
        return propertyInfo;
    }


} // namespace bias
