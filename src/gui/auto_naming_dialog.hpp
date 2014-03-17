#ifndef AUTO_NAMING_DIALOG_HPP
#define AUTO_NAMING_DIALOG_HPP
#include "ui_auto_naming_dialog.h"
#include <QString>

namespace bias
{
    
    class AutoNamingOptions
    {
        public:
            AutoNamingOptions();

            int cameraIdentifier;
            static int DEFAULT_CAMERA_IDENTIFIER;
            enum {GUID_IDENTIFIER, CAMERA_NUMBER_IDENTIFIER};

            QString timeAndDateFormat;
            static QString DEFAULT_TIME_AND_DATE_FORMAT;

    };


    class AutoNamingDialog : public QDialog, public Ui::AutoNamingDialog
    {
        Q_OBJECT

        public:
            AutoNamingDialog(QWidget *parent=0);
            AutoNamingDialog(AutoNamingOptions options, QWidget *parent=0);

        private:
            void initialize(AutoNamingOptions options);


    };
}

#endif
