#ifndef BIAS_FORMAT7_SETTINGS_DIALOG_HPP
#define BIAS_FORMAT7_SETTINGS_DIALOG_HPP

#include <QDialog>
#include "ui_format7_settings_dialog.h"

namespace bias
{

    class Format7SettingsDialog : public QDialog, public Ui::Format7SettingsDialog
    {
        Q_OBJECT

        public:
            Format7SettingsDialog(
                    QWidget *parent=0,
                    Qt::WindowFlags f=0
                    );

        protected:
            void resizeEvent(QResizeEvent *event);

        private:
            void initialize();

    }; // class Format7SettingsDialog

} // namespace bias


#endif // #ifndef BIAS_FORMAT7_SETTINGS_DIALOG_HPP
