#include "alignment_settings_dialog.hpp"
#include <iostream>

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
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        initializeGridTab();
        initializeEllipseTab();
        updateSettings(settings_);
    }


    void AlignmentSettingsDialog::initializeGridTab()
    {
        unsigned int minNumRow = AlignmentSettings::GRID_MIN_NUM_ROW;
        unsigned int maxNumRow = AlignmentSettings::GRID_MAX_NUM_ROW;
        for (unsigned int i=minNumRow, cnt=0; i<maxNumRow; i++, cnt++)
        {
            gridRowsComboBoxPtr_ -> addItem(QString::number(i));
            gridRowsValueToIndexMap_[i] = cnt;

        }

        unsigned int minNumCol = AlignmentSettings::GRID_MIN_NUM_COL;
        unsigned int maxNumCol = AlignmentSettings::GRID_MAX_NUM_COL;
        for (unsigned int i=minNumCol, cnt=0; i<maxNumCol; i++, cnt++)
        {
            gridColsComboBoxPtr_ -> addItem(QString::number(i));
            gridColsValueToIndexMap_[i] = cnt;
        }
    }


    void AlignmentSettingsDialog::initializeEllipseTab()
    {
    }


    void AlignmentSettingsDialog::updateSettings(AlignmentSettings settings)
    {
        settings_ = settings;

        // Grid tab settings
        gridVisibleCheckBoxPtr_ -> setChecked(settings_.gridVisible);
        unsigned int rowsIndex = gridRowsValueToIndexMap_[settings_.gridNumRow];
        gridRowsComboBoxPtr_ -> setCurrentIndex(rowsIndex);
        unsigned int colsIndex = gridColsValueToIndexMap_[settings_.gridNumCol];
        gridColsComboBoxPtr_ -> setCurrentIndex(colsIndex);

        std::cout << rowsIndex << " " << colsIndex << std::endl;

        // Ellipse tab settings
        ellipseVisibleCheckBoxPtr_ -> setChecked(settings_.ellipseVisible);
    }


} // namespace bias
