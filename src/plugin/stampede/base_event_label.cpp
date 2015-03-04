#include "base_event_label.hpp"
#include <QtDebug>

namespace bias
{

    // Public Methods
    // ------------------------------------------------------------------------
    BaseEventLabel::BaseEventLabel(QWidget *parent) : QLabel(parent)
    {
    }


    // Protected Methods
    // ------------------------------------------------------------------------
    void BaseEventLabel::initialize()
    {

    }

    void BaseEventLabel::mousePressEvent(QMouseEvent *event)
    {
        qDebug() << "mouse press";
    }


}
