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
            AutoNamingDialog(AutoNamingOptions options, unsigned int numberOfCameras, QWidget *parent=0);

        signals:
            void autoNamingOptionsChanged(AutoNamingOptions options);

        private slots:
            void dialogWidgetChanged();
            void imageCaptureStarted(bool logging);
            void imageCaptureStopped();

        private:
            unsigned int numberOfCameras_;
            void connectWidgets();
            void initialize(AutoNamingOptions options, unsigned int numberOfCameras=1);
            void setDialogFromOptions(AutoNamingOptions options);
            AutoNamingOptions getOptionsFromDialog();
    };
}

#endif
