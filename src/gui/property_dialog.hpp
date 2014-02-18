#ifndef BIAS_PROPERTY_DIALOG_HPP
#define BIAS_PROPTERy_DIALOG_HPP

#include <QDialog>
#include <QPointer>
#include <QDoubleValidator>
#include <memory>
#include "camera_facade_fwd.hpp"
#include "ui_property_dialog.h"

class QTimer;
class QString;

namespace bias
{
    template <class T> class Lockable;

    class PropertyDialog : public QDialog, public Ui::PropertyDialog
    {
        Q_OBJECT

        public:

            PropertyDialog(
                    QWidget *parent=0,
                    Qt::WindowFlags f=0
                    );

            PropertyDialog(
                    std::shared_ptr<Lockable<Camera>> cameraPtr,
                    PropertyType propertyType, 
                    QWidget *parent=0,
                    Qt::WindowFlags f=0
                    );

        private slots:

            void onIntValueSliderChanged();
            void onIntEditingFinished();
            void onAbsValueSliderChanged();
            void onAbsEditingFinished();
            void stopRefreshTimer();
            void startRefreshTimer();
            void onAutoStateChanged(int state);
            void onOffStateChanged(int state);
            void onOnePushButtonClicked();
            void onRefreshTimer();

        private:

            std::shared_ptr<Lockable<Camera>> cameraPtr_;
            PropertyType propertyType_;

            bool absoluteCapable_; 
            bool manualCapable_;
            bool autoCapable_;

            QPointer<QTimer> refreshTimerPtr_;

            void connectWidgets();
            void initialize();

            void updateDisplay(Property property, PropertyInfo propertyInfo);
            void getPropertyAndUpdateDisplay(bool showLockFailErrMsg);

            void setPropertyValue(unsigned int value); 
            void setPropertyAbsoluteValue(float absoluteValue);
            void setProperty(Property property);
            void cameraLockFailErrMsg(QString msg);

            Property getProperty();
            PropertyInfo getPropertyInfo();


    }; // class PropertyDialog


} // namespace bias

#endif // #ifndef BIAS_PROPTERY_DIALOG_HPP
