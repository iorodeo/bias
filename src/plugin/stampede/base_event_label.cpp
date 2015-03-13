#include "base_event_label.hpp"
#include <QtDebug>
#include <QMenu>
#include <QMouseEvent>
#include <QContextMenuEvent>

namespace bias
{

    // Public Methods
    // ------------------------------------------------------------------------
    BaseEventLabel::BaseEventLabel(QWidget *parentPtr) : QLabel(parentPtr)
    {
        initialize();
    }


    // Protected Methods
    // ------------------------------------------------------------------------
    void BaseEventLabel::initialize()
    { 
        setupContextMenuActions();
    }


    void BaseEventLabel::setupContextMenuActions()
    {
        editActionPtr_ = new QAction("Edit", this);
        insertActionPtr_ = new QAction("Insert", this);
        removeActionPtr_ = new QAction("Remove", this);
        connect(editActionPtr_, SIGNAL(triggered()), this, SLOT(editActionTriggered()));
        connect(insertActionPtr_, SIGNAL(triggered()), this, SLOT(insertActionTriggered()));
        connect(removeActionPtr_, SIGNAL(triggered()), this, SLOT(removeActionTriggered()));
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


    void BaseEventLabel::contextMenuEvent(QContextMenuEvent *eventPtr)
    {
        qDebug() << "context Menu requested";
        QMenu contextMenu(this);

        contextMenu.addAction(editActionPtr_);
        contextMenu.addAction(insertActionPtr_);
        contextMenu.addAction(removeActionPtr_);
        contextMenu.exec(eventPtr -> globalPos());
    }

    // Private slots
    // ------------------------------------------------------------------------
    void BaseEventLabel::editActionTriggered()
    {
        qDebug() << "edit action triggered";
    }


    void BaseEventLabel::insertActionTriggered()
    {
        qDebug() << "insert action triggered";
    }


    void BaseEventLabel::removeActionTriggered()
    {
        qDebug() << "remove action triggered";
    }


}
