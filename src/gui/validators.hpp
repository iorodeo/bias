#ifndef BIAS_VALIDATORS_HPP
#define BIAS_VALIDATORS_HPP

#include <QIntValidator>
#include <QDoubleValidator>

class QString;

namespace bias
{
    class IntValidatorWithFixup : public QIntValidator
    {
        Q_OBJECT

        public:

            IntValidatorWithFixup(QWidget *parent=0);
            virtual void fixup(QString &input) const;

    };


    class DoubleValidatorWithFixup : public QDoubleValidator
    {
        Q_OBJECT

        public:

            DoubleValidatorWithFixup(QWidget *parent=0);
            virtual void fixup(QString &input) const;
    };


} // namesapce bias

#endif // #ifndef BIAS_VALIDATORS_HPP
