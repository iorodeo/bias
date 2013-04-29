#include "format7_settings_dialog.hpp"
#include <iostream>
#include <algorithm>
#include <QGraphicsScene>
#include <QPointer>

namespace bias 
{
    Format7SettingsDialog::Format7SettingsDialog(QWidget *parent) : QDialog(parent)
    {
        initialize();
    }

    void Format7SettingsDialog::initialize()
    {
        setupUi(this);
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }
    

} // namespace bias
