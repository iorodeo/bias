#include "property_dialog.hpp"
#include "camera_facade.hpp"
#include "lockable.hpp"
#include <string>
#include <iostream>
#include <QPointer>
#include <QString>

namespace bias
{

    PropertyDialog::PropertyDialog(QWidget *parent) : QDialog(parent)
    {
        setupUi(this);
        propertyType_ = PROPERTY_TYPE_UNSPECIFIED;
        cameraPtr_ = NULL;
        connectWidgets();
        initialize();
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
        std::cout << "onIntValueSliderChanged: " << intValueSliderPtr -> value() << std::endl;
    }

    void PropertyDialog::connectWidgets()
    {
        connect(
                intValueSliderPtr,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(onIntValueSliderChanged())
               );

        
    }

    void PropertyDialog::initialize()
    {
        propertyNameLabelPtr -> setText(
                QString::fromStdString(getPropertyTypeString(propertyType_))
                );
        updateDisplayValues();
    }


    void PropertyDialog::updateDisplayValues()
    {
        // Get property values and info from camera
        Property property = cameraPtr_ -> getProperty(propertyType_);
        PropertyInfo propertyInfo = cameraPtr_ -> getPropertyInfo(propertyType_);

        std::cout << property.toString() << std::endl;
        std::cout << propertyInfo.toString() << std::endl;

        // Enable/disable checkboxes and set values
        onCheckBoxPtr -> setEnabled(propertyInfo.onOffCapable);
        onCheckBoxPtr -> setChecked(property.on);

        autoCheckBoxPtr -> setEnabled(
                propertyInfo.manualCapable && propertyInfo.autoCapable
                );
        autoCheckBoxPtr -> setChecked(property.autoActive);

        onePushCheckBoxPtr -> setEnabled(propertyInfo.onePushCapable);
        onePushCheckBoxPtr -> setChecked(property.onePush);

        // Enable/disable group boxes
        intValueGroupBoxPtr -> setEnabled(propertyInfo.manualCapable);
        absValueGroupBoxPtr -> setEnabled(
                propertyInfo.manualCapable && propertyInfo.absoluteCapable
                );

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
            //////////////////////////////////////////////////////////////
            // FIX THIS  - change so that you are setting integer values
            // in the qslide perhaps by scaling the floating point numbers. 
            //////////////////////////////////////////////////////////////
            absValueSliderPtr -> setMinimum(int(100*propertyInfo.minAbsoluteValue));
            absValueSliderPtr -> setMaximum(int(100*propertyInfo.maxAbsoluteValue));
            absValueSliderPtr -> setSliderPosition(int(100*property.absoluteValue));

            minAbsValueLabelPtr -> setText(
                    QString::number(propertyInfo.minAbsoluteValue, 'f', 2)
                    );
            maxAbsValueLabelPtr -> setText(
                    QString::number(propertyInfo.maxAbsoluteValue, 'f', 2)
                    );
            absValueLineEditPtr -> setText(
                    QString::number(property.absoluteValue, 'f', 2)
                    );
            if (propertyInfo.manualCapable)
            {
                QPointer<QDoubleValidator> absValueValidatorPtr = new QDoubleValidator(
                        propertyInfo.minAbsoluteValue,
                        propertyInfo.maxAbsoluteValue,
                        2,
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
