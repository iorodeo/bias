#ifndef BIAS_PROPERTY_DIALOG_HPP
#define BIAS_PROPTERy_DIALOG_HPP

#include <QDialog>
#include <memory>
#include "camera_facade_fwd.hpp"
#include "ui_property_dialog.h"

namespace bias
{
    template <class T> class Lockable;

    class PropertyDialog : public QDialog, public Ui::PropertyDialog
    {
        Q_OBJECT

        public:

            PropertyDialog(QWidget *parent=0);

            PropertyDialog(
                    std::shared_ptr<Lockable<Camera>> cameraPtr,
                    PropertyType propertyType, 
                    QWidget *parent=0
                    );

        private slots:

            void onIntValueSliderChanged();
            void onIntEditingFinished();
            void onAbsValueSliderChanged();
            void onAbsEditingFinished();

        private:

            std::shared_ptr<Lockable<Camera>> cameraPtr_;
            PropertyType propertyType_;
            bool absoluteCapable_; 
            bool manualCapable_;
            bool autoCapable_;

            void connectWidgets();
            void initialize();

            void updateDisplayValues(
                    Property property, 
                    PropertyInfo propertyInfo
                    );

            void setPropertyValue(unsigned int value); 
            void setPropertyAbsoluteValue(float absoluteValue);


    }; // class PropertyDialog

} // namespace bias

#endif // #ifndef BIAS_PROPTERY_DIALOG_HPP
