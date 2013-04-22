#include "format7_settings_dialog.hpp"
#include <iostream>
#include <algorithm>
#include <QGraphicsScene>
#include <QPointer>

namespace bias 
{
    Format7SettingsDialog::Format7SettingsDialog(QWidget *parent, Qt::WindowFlags f) 
        : QDialog(parent,f)
    {
        initialize();
    }

    void Format7SettingsDialog::initialize()
    {
        setupUi(this);
        setAttribute(Qt::WA_DeleteOnClose);

        // Temporary
        modeComboBoxPtr_ -> addItem("0");
        pixelFormatComboBoxPtr_ -> addItem("MONO8");
        xOffsetLineEditPtr_ -> setText("0");
        yOffsetLineEditPtr_ -> setText("0");
        xWidthLineEditPtr_ -> setText("1280");
        yHeightLineEditPtr_ -> setText("1024");

        QPointer<QGraphicsScene> scenePtr = new QGraphicsScene();
        roiGraphicsViewPtr_ -> setScene(scenePtr);
        scenePtr -> addRect(QRectF(0,0,200,200));

    }
    
    void Format7SettingsDialog::resizeEvent(QResizeEvent *event)
    {
        QSize size = roiWidgetPtr_ -> size();
        int minDim = std::min(size.width(), size.height());

        roiGraphicsViewPtr_ -> setGeometry(
                (size.width() - minDim)/2,
                (size.height() - minDim)/2,
                minDim,
                minDim
                );

        std::cout << "size: " << size.width() << ", " << size.height() << std::endl;

    }


    

} // namespace bias
