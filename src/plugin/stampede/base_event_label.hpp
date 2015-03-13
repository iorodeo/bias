#ifndef BASE_EVENT_LABEL_HPP
#define BASE_EVENT_LABEL_HPP

#include <QLabel>
#include <QWidget>
#include <QPointer>
#include <QAction>

namespace bias
{

    class BaseEventLabel : public QLabel
    {

        Q_OBJECT

        public:

            BaseEventLabel(QWidget *parentPtr=0);

        protected:

            QPointer<QAction> editActionPtr_;
            QPointer<QAction> insertActionPtr_;
            QPointer<QAction> removeActionPtr_;

            void initialize();
            void setupContextMenuActions();
            void mousePressEvent(QMouseEvent *eventPtr);
            void contextMenuEvent(QContextMenuEvent *eventPtr);

        private slots:

            void editActionTriggered();
            void insertActionTriggered();
            void removeActionTriggered();

    };

}

#endif
