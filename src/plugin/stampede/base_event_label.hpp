#ifndef BASE_EVENT_LABEL_HPP
#define BASE_EVENT_LABEL_HPP

#include <QLabel>
#include <QWidget>

namespace bias
{

    class BaseEventLabel : public QLabel
    {

        Q_OBJECT

        public:

            BaseEventLabel(QWidget *parentPtr=0);

        protected:

            void initialize();
            void mousePressEvent(QMouseEvent *eventPtr);
            //void mouseMoveEvent(QMouseEvent *eventPtr);

    };

}

#endif
