#include "validators.hpp"
#include <QWidget>
#include <QString>
#include <iostream>

namespace bias
{
    // IntValidatorWithFixup
    IntValidatorWithFixup::IntValidatorWithFixup(QWidget *parent) 
        : QIntValidator(parent)
    {}

    void IntValidatorWithFixup::fixup(QString &input) const
    {
        int value = input.toInt();
        if (value < bottom())
        {
            input = QString::number(bottom());
            std::cout << input.toStdString() << std::endl;
        }
        else if (value > top())
        {
            input = QString::number(top());
            std::cout << input.toStdString() << std::endl;
        }
    }


    // DoubleValidatorWithFixup
    DoubleValidatorWithFixup::DoubleValidatorWithFixup(QWidget *parent) 
        : QDoubleValidator(parent)
    {}

    void DoubleValidatorWithFixup::fixup(QString &input) const 
    {
        double value = input.toDouble();
        if (value < bottom())
        {
            input = QString::number(bottom(),'f', decimals());
        }
        else if (value > top())
        {
            input = QString::number(top(), 'f', decimals());
        }
    }

} // namespace bias
