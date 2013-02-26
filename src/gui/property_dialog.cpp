#include "property_dialog.hpp"
#include "camera_facade.hpp"
#include "lockable.hpp"
#include <string>
#include <iostream>
#include <QPointer>
#include <QString>

namespace bias
{
    const float ABSOLUTE_VALUE_SCALE = 100.0;
    const unsigned int FLOAT_PREC = 2;

    PropertyDialog::PropertyDialog(QWidget *parent) : QDialog(parent)
    {
        setupUi(this);
        propertyType_ = PROPERTY_TYPE_UNSPECIFIED;
        cameraPtr_ = NULL;
        initialize();
        connectWidgets();
    }


    PropertyDialog::PropertyDialog(
            std::shared_ptr<Lockable<Camera>> cameraPtr,
            PropertyType propertyType, 
            QWidget *parent
            ) : QDialog(parent)
    {
        setupUi(this);
        propertyType_ = propertyType;
        cameraPtr_ = cameraPtr;
        connectWidgets();
        initialize();
    }


    void PropertyDialog::onIntValueSliderChanged()
    {
        unsigned int value = (unsigned int)(intValueSliderPtr -> value());
        setPropertyValue(value);
        std::cout << "onIntValueSliderChanged: " << value << std::endl;
    }


    void PropertyDialog::onIntEditingFinished()
    {
        QString valueString = intValueLineEditPtr -> text();
        unsigned int value = valueString.toUInt();
        setPropertyValue(value);
        std::cout << "onIntEditingFinished: " << value << std::endl;
    }


    void PropertyDialog::onAbsValueSliderChanged()
    {
        float absoluteValue = float(absValueSliderPtr -> value())/ABSOLUTE_VALUE_SCALE;
        setPropertyAbsoluteValue(absoluteValue);
        std::cout << "onAbsValueSliderChanged: " << absoluteValue << std::endl;
    }


    void PropertyDialog::onAbsEditingFinished()
    {
        QString absoluteValueString = absValueLineEditPtr -> text();
        float absoluteValue = absoluteValueString.toFloat();
        setPropertyAbsoluteValue(absoluteValue);
        std::cout << "onAbsEditingFinished: " << absoluteValue << std::endl;
    }

    void PropertyDialog::setPropertyValue(unsigned int value)
    {
        Property property = cameraPtr_ -> getProperty(propertyType_);
        PropertyInfo propertyInfo = cameraPtr_ -> getPropertyInfo(propertyType_);
        property.value = value;
        property.absoluteControl = false;

        cameraPtr_ -> acquireLock();
        cameraPtr_ -> setProperty(property);
        property = cameraPtr_ -> getProperty(propertyType_);
        cameraPtr_ -> releaseLock();

        updateDisplayValues(property, propertyInfo);
    }

    void PropertyDialog::setPropertyAbsoluteValue(float absoluteValue)
    {
        Property property = cameraPtr_ -> getProperty(propertyType_);
        PropertyInfo propertyInfo = cameraPtr_ -> getPropertyInfo(propertyType_);
        property.absoluteValue = absoluteValue;
        property.absoluteControl = true;

        cameraPtr_ -> acquireLock();
        cameraPtr_ -> setProperty(property);
        property = cameraPtr_ -> getProperty(propertyType_);
        cameraPtr_ -> releaseLock();

        updateDisplayValues(property, propertyInfo);
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
                absValueSliderPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(onAbsValueSliderChanged())
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
    }


    void PropertyDialog::initialize()
    {
        propertyNameLabelPtr -> setText(
                QString::fromStdString(getPropertyTypeString(propertyType_))
                );

        intValueSliderPtr -> setTracking(false);
        absValueSliderPtr -> setTracking(false);

        cameraPtr_ -> acquireLock();
        Property property = cameraPtr_ -> getProperty(propertyType_);
        PropertyInfo propertyInfo = cameraPtr_ -> getPropertyInfo(propertyType_);
        cameraPtr_ -> releaseLock();

        updateDisplayValues(property,propertyInfo);
    }


    void PropertyDialog::updateDisplayValues(Property property, PropertyInfo propertyInfo)
    {
        std::cout << property.toString() << std::endl;
        std::cout << propertyInfo.toString() << std::endl;

        // Enable/disable checkboxes and set values
        onCheckBoxPtr -> setEnabled(propertyInfo.onOffCapable);
        onCheckBoxPtr -> setChecked(property.on);

        autoCheckBoxPtr -> setEnabled(
                propertyInfo.manualCapable && propertyInfo.autoCapable
                );

        autoCheckBoxPtr -> setChecked(property.autoActive);

        onePushPushButtonPtr -> setEnabled(propertyInfo.onePushCapable);

        // Enable/disable group boxes
        bool intGroupBoxEnabled = propertyInfo.manualCapable && (!property.autoActive);
        intValueGroupBoxPtr -> setEnabled(intGroupBoxEnabled);

        bool absGroupBoxEnabled = propertyInfo.manualCapable; 
        absGroupBoxEnabled = absGroupBoxEnabled && propertyInfo.absoluteCapable; 
        absGroupBoxEnabled = absGroupBoxEnabled && (!property.autoActive);
        absValueGroupBoxPtr -> setEnabled(absGroupBoxEnabled);

        // Set integer value slider 
        intValueSliderPtr -> setMinimum(propertyInfo.minValue);
        intValueSliderPtr -> setMaximum(propertyInfo.maxValue);
        intValueSliderPtr -> setSliderPosition(property.value);
        minIntValueLabelPtr -> setText(QString::number(propertyInfo.minValue));
        maxIntValueLabelPtr -> setText(QString::number(propertyInfo.maxValue));
        intValueLineEditPtr -> setText(QString::number(property.value));

        if (propertyInfo.manualCapable)
        {
            QPointer<QIntValidator> intValueValidatorPtr = new QIntValidator(
                    propertyInfo.minValue,
                    propertyInfo.maxValue,
                    intValueLineEditPtr
                    );
            intValueLineEditPtr -> setValidator(intValueValidatorPtr);
        }

        // Set absolute value slider
        if (propertyInfo.absoluteCapable) 
        {
            absValueSliderPtr -> setMinimum(
                    int(ABSOLUTE_VALUE_SCALE*propertyInfo.minAbsoluteValue)
                    );
            absValueSliderPtr -> setMaximum(
                    int(ABSOLUTE_VALUE_SCALE*propertyInfo.maxAbsoluteValue)
                    );
            absValueSliderPtr -> setSliderPosition(
                    int(ABSOLUTE_VALUE_SCALE*property.absoluteValue)
                    );

            minAbsValueLabelPtr -> setText(
                    QString::number(propertyInfo.minAbsoluteValue, 'f', FLOAT_PREC)
                    );
            maxAbsValueLabelPtr -> setText(
                    QString::number(propertyInfo.maxAbsoluteValue, 'f', FLOAT_PREC)
                    );
            absValueLineEditPtr -> setText(
                    QString::number(property.absoluteValue, 'f', FLOAT_PREC)
                    );

            if (propertyInfo.manualCapable)
            {
                QPointer<QDoubleValidator> absValueValidatorPtr = new QDoubleValidator(
                        propertyInfo.minAbsoluteValue,
                        propertyInfo.maxAbsoluteValue,
                        FLOAT_PREC,
                        absValueLineEditPtr
                        );
                absValueLineEditPtr -> setValidator(absValueValidatorPtr);
            }
        }
        else
        {
            absValueSliderPtr -> setMinimum(0);
            absValueSliderPtr -> setMaximum(100);
            absValueSliderPtr -> setSliderPosition(0);
            minAbsValueLabelPtr -> setText(QString("NA"));
            maxAbsValueLabelPtr -> setText(QString("NA"));
        }

        // Set units
        if (propertyInfo.haveUnits)
        {
            unitsNameLabelPtr -> setText(QString::fromStdString(propertyInfo.units));
        }
        else
        {
            unitsNameLabelPtr -> setText(QString("NA"));
        }
    }


} // namespace bias
