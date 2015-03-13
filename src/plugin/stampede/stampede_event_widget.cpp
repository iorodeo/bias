#include "stampede_event_widget.hpp"
#include "empty_event_label.hpp"
#include <QtDebug>
#include <QToolTip>

namespace bias
{
    double StampedeEventWidget::DEFAULT_DURATION = 5.0;

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


    void StampedeEventWidget::setToolTipMsg(QString msg)
    {
        QPoint mousePosGlobal = QCursor::pos();
        QToolTip::showText(mousePosGlobal, msg, this, rect());
    }


    // Protected Methods
    // ------------------------------------------------------------------------
    void StampedeEventWidget::initialize()
    {
        duration_ = DEFAULT_DURATION;

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
        double timePos = duration_*fracPos;

        QString toolTipMsg = QString("t=") + QString::number(timePos,'g',3);
        setToolTipMsg(toolTipMsg);

        emit timelineMoved(timePos, index);
    }


}
