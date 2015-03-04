#include "stampede_event_widget.hpp"

namespace bias
{

    // Public Methods
    // ------------------------------------------------------------------------
    StampedeEventWidget::StampedeEventWidget(QWidget *parent) : QWidget(parent)
    {
        setupUi(this);
        initialize();
    }


    void StampedeEventWidget::setNameText(QString name)
    {
        nameLabelPtr -> setText(name);
    }

    QString StampedeEventWidget::getNameText()
    {
        return nameLabelPtr -> text();
    }

    // Protected Methods
    // ------------------------------------------------------------------------
    void StampedeEventWidget::initialize()
    {
        timelineLayoutPtr_ = new QHBoxLayout(timelineWidgetPtr);
        timelineSplitterPtr_ = new QSplitter(timelineWidgetPtr);
        timelineSplitterPtr_ -> setHandleWidth(0);

        timelineLayoutPtr_ -> addWidget(timelineSplitterPtr_);
        timelineWidgetPtr -> setLayout(timelineLayoutPtr_);

        int num = 5;
        int step = 255/num;
        for (int i =0; i<num; i++)
        {
            QPointer<QLabel> testLabelPtr = new QLabel();
            testLabelPtr -> setFixedHeight(25);
            QPalette labelPalette;
            labelPalette.setColor(QPalette::Window, QColor(255-step*i,255, step*i));
            labelPalette.setColor(QPalette::WindowText, QColor(0,0,0));
            testLabelPtr -> setAutoFillBackground(true);
            testLabelPtr -> setPalette(labelPalette);


            QString labelText = QString("item %1").arg(i);
            testLabelPtr -> setText(labelText);
            timelineSplitterPtr_  -> addWidget(testLabelPtr);
        }
        timelineSplitterPtr_ -> setChildrenCollapsible(false);
    }


}
