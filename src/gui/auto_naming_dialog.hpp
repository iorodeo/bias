#ifndef AUTO_NAMING_DIALOG_HPP
#define AUTO_NAMING_DIALOG_HPP
#include "ui_auto_naming_dialog.h"
#include "auto_naming_options.hpp"
#include <QString>

namespace bias
{

    class AutoNamingDialog : public QDialog, public Ui::AutoNamingDialog
    {
        Q_OBJECT

        public:
            AutoNamingDialog(QWidget *parent=0);
            AutoNamingDialog(
                    AutoNamingOptions options, 
                    unsigned int numberOfCameras, 
                    QWidget *parent=0
                    );
        signals:
            void autoNamingOptionsChanged(AutoNamingOptions options);

        private slots:
            void cameraIdGroupBoxToggled(bool checked);
            void guidRadioButtonToggled(bool checked);
            void timeAndDateGroupBoxToggled(bool checked);

        private:
            void connectWidgets();
            void initialize(
                    AutoNamingOptions options, 
                    unsigned int numberOfCameras
                    );


    };
}

#endif
