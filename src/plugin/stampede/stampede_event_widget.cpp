#include "stampede_event_widget.hpp"
#include "empty_event_label.hpp"
#include <QtDebug>
#include <QToolTip>

namespace bias
{

    // Public Methods
    // ------------------------------------------------------------------------
    StampedeEventWidget::StampedeEventWidget(QWidget *parentPtr) : QWidget(parentPtr)
    {
        setupUi(this);
        initialize();
    }


    void StampedeEventWidget::setNameText(QString name)
    {
        groupBoxPtr -> setTitle(name);
    }

    QString StampedeEventWidget::getNameText()
    {
        return groupBoxPtr -> title();
    }


    // Protected Methods
    // ------------------------------------------------------------------------
    void StampedeEventWidget::initialize()
    {
        timelineLayoutPtr_ = new QHBoxLayout(timelineWidgetPtr);
        timelineSplitterPtr_ = new QSplitter(timelineWidgetPtr);
        timelineSplitterPtr_ -> setHandleWidth(0);
        timelineSplitterPtr_ -> setOpaqueResize(true);
        connect(
                timelineSplitterPtr_, 
                SIGNAL(splitterMoved(int, int)), 
                this, 
                SLOT(timelineSplitterMoved(int, int))
                );


        timelineLayoutPtr_ -> addWidget(timelineSplitterPtr_);
        timelineWidgetPtr -> setLayout(timelineLayoutPtr_);

        int num = 5;
        int step = 255/num;
        for (int i =0; i<num; i++)
        {
            QPointer<QLabel> testLabelPtr = new EmptyEventLabel();
            testLabelPtr -> setFixedHeight(25);
            QPalette labelPalette;
            if (i%2==0)
            {
                labelPalette.setColor(QPalette::Window, QColor(100,100,100));
            }
            else
            {
                labelPalette.setColor(QPalette::Window, QColor(0,0,255));
            }
            labelPalette.setColor(QPalette::WindowText, QColor(0,0,0));
            testLabelPtr -> setAutoFillBackground(true);
            testLabelPtr -> setPalette(labelPalette);
            timelineSplitterPtr_  -> addWidget(testLabelPtr);
        }
        timelineSplitterPtr_ -> setChildrenCollapsible(false);
    }


    // Private slots
    // ------------------------------------------------------------------------

    void StampedeEventWidget::timelineSplitterMoved(int pos, int index)
    {
        QSize splitterSize = timelineSplitterPtr_ -> frameSize();
        int splitterWidth = splitterSize.width();

        double fracPos = double(pos)/double(splitterWidth);

        QPoint mousePosGlobal = QCursor::pos();

        QString toolTipMsg = QString("t=") + QString::number(fracPos,'g',2);
        QToolTip::showText(mousePosGlobal, toolTipMsg, this, rect());

        qDebug() << "index:        " << index; 
        qDebug() << "fracPos:      " << fracPos; 
        qDebug() << "global (x,y): " << mousePosGlobal.x() << mousePosGlobal.y();
        qDebug() << "--";
    }


}
