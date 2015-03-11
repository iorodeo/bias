#include "base_event_label.hpp"
#include <QtDebug>
#include <QMouseEvent>

namespace bias
{

    // Public Methods
    // ------------------------------------------------------------------------
    BaseEventLabel::BaseEventLabel(QWidget *parentPtr) : QLabel(parentPtr)
    {
    }


    // Protected Methods
    // ------------------------------------------------------------------------
    void BaseEventLabel::initialize()
    {

    }

    void BaseEventLabel::mousePressEvent(QMouseEvent *eventPtr)
    {
        Qt::MouseButton button = eventPtr -> button();
        switch (button)
        {
            case Qt::LeftButton:
                qDebug() << "left mouse press";
                break;

            case Qt::RightButton:
                qDebug() << "right mouse press";
                break;

            default:
                qDebug() << "general mouse press";
                break;
        }
        qDebug() << "mouse press";
    }


    //void BaseEventLabel::mouseMoveEvent(QMouseEvent *eventPtr)
    //{
    //    int x = eventPtr -> x();
    //    int y = eventPtr -> y();
    //    qDebug() << "move" << x << y;
    //}

}
