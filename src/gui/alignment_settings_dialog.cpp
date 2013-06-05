#include "alignment_settings_dialog.hpp"
#include <iostream>
#include <QColorDialog>
#include <QPalette>

namespace bias
{

    AlignmentSettingsDialog::AlignmentSettingsDialog(QWidget *parent)
        : QDialog(parent)
    {
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


    void AlignmentSettingsDialog::gridVisibleChanged(int state)
    {
        if (state == Qt::Unchecked)
        {
            settings_.gridVisible = false;
        }
        else
        {
            settings_.gridVisible = true;
        }
        emit alignmentSettingsChanged(settings_);
    }


    void AlignmentSettingsDialog::gridRowsChanged(int index)
    {
        settings_.gridNumRow = gridRowsValueToIndexMap_.key(index);
        emit alignmentSettingsChanged(settings_);
    }


    void AlignmentSettingsDialog::gridColsChanged(int index)
    {
        settings_.gridNumCol = gridColsValueToIndexMap_.key(index);
        emit alignmentSettingsChanged(settings_);
    }


    void AlignmentSettingsDialog::gridColorChangeClicked()
    {
        QColor color = QColorDialog::getColor(settings_.gridQColor, this);
        if (color.isValid())
        {
            settings_.gridQColor = color;
        }
        setGridColorLabel(settings_.gridQColor); 
        emit alignmentSettingsChanged(settings_);
    }


    void AlignmentSettingsDialog::ellipseVisibleChanged(int state)
    {
        std::cout << "ellipseVisible = " << state << std::endl;
        if (state == Qt::Unchecked)
        {
            settings_.ellipseVisible = false;
        }
        else
        {
            settings_.ellipseVisible = true;
        }
        emit alignmentSettingsChanged(settings_);
    }

    void AlignmentSettingsDialog::ellipseColorChangeClicked()
    {
        QColor color = QColorDialog::getColor(settings_.gridQColor, this);
        if (color.isValid())
        {
            settings_.ellipseQColor = color;
        }
        setEllipseColorLabel(settings_.ellipseQColor); 
        emit alignmentSettingsChanged(settings_);
    }
    

    void AlignmentSettingsDialog::connectWidgets()
    {

        connect(
                gridVisibleCheckBoxPtr_,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(gridVisibleChanged(int))
               );

        connect(
                gridRowsComboBoxPtr_,
                SIGNAL(currentIndexChanged(int)),
                this,
                SLOT(gridRowsChanged(int))
               );

        connect(
                gridColsComboBoxPtr_,
                SIGNAL(currentIndexChanged(int)),
                this,
                SLOT(gridColsChanged(int))
               );

        connect(
                gridColorChangePushButtonPtr_,
                SIGNAL(clicked()),
                this,
                SLOT(gridColorChangeClicked())
               );

        connect(
                ellipseVisibleCheckBoxPtr_,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(ellipseVisibleChanged(int))
               );

        connect(
                ellipseColorChangePushButtonPtr_,
                SIGNAL(clicked()),
                this,
                SLOT(ellipseColorChangeClicked())
               );
                    
    }


    void AlignmentSettingsDialog::initialize()
    {
        setupUi(this);
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        initializeGridTab();
        initializeEllipseTab();
        updateSettings(settings_);
        connectWidgets();
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
        setGridColorLabel(settings_.gridQColor);


        // Ellipse tab settings
        ellipseVisibleCheckBoxPtr_ -> setChecked(settings_.ellipseVisible);
        setEllipseColorLabel(settings_.ellipseQColor);
    }

    void AlignmentSettingsDialog::setGridColorLabel(QColor color)
    {
        if (color.isValid())
        {
            QPalette palette = gridColorImageLabelPtr_ -> palette();
            palette.setColor(gridColorImageLabelPtr_ -> backgroundRole(), color);
            gridColorImageLabelPtr_ -> setPalette(palette);
            gridColorImageLabelPtr_ -> setAutoFillBackground(true);
        }
    }

    void AlignmentSettingsDialog::setEllipseColorLabel(QColor color)
    {
        if (color.isValid())
        {
            QPalette palette = ellipseColorImageLabelPtr_ -> palette();
            palette.setColor(ellipseColorImageLabelPtr_ -> backgroundRole(), color);
            ellipseColorImageLabelPtr_ -> setPalette(palette);
            ellipseColorImageLabelPtr_ -> setAutoFillBackground(true);
        }
    }


} // namespace bias
