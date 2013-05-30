#include "alignment_settings_dialog.hpp"

namespace bias
{

    AlignmentSettingsDialog::AlignmentSettingsDialog(QWidget *parent)
        : QDialog(parent)
    {
        connectWidgets();
        initialize();
    }


    AlignmentSettingsDialog::AlignmentSettingsDialog(
            AlignmentSettings settings, 
            QWidget *parent
            ) : QDialog(parent)
    {
        settings_ = settings;
        initialize();
    }
    

    void AlignmentSettingsDialog::connectWidgets()
    {

    }

    void AlignmentSettingsDialog::initialize()
    {
        setupUi(this);

        setAttribute(Qt::WA_DeleteOnClose);

        updateSettings(settings_);
    }


    void AlignmentSettingsDialog::updateSettings(AlignmentSettings settings)
    {
        settings_ = settings;

        gridVisibleCheckBoxPtr_ -> setChecked(true);

    }


} // namespace bias
