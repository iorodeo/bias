#ifndef STAMPEDE_EVENT_WIDGET_HPP
#define STAMPEDE_EVENT_WIDGET_HPP

#include "ui_stampede_event_widget.h"
#include <QPointer>
#include <QHBoxLayout>
#include <QSplitter>

namespace bias
{

    class StampedeEventWidget : public QWidget, public Ui::StampedeEventForm
    {
        Q_OBJECT

        public:

            StampedeEventWidget(QWidget *parent=0);
            void setNameText(QString name);
            QString getNameText();

        protected:

            QPointer<QHBoxLayout> timelineLayoutPtr_;
            QPointer<QSplitter> timelineSplitterPtr_;

            void initialize();


    };

}


#endif
